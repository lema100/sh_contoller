#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "string.h"

#include "stm32f1xx_it.h"

#include "mb_slave_uart.h"
#include "update.h"
#include "arch.h"

#define MB_COUNT_MAX 1024
uint32_t offline, time_offline;
uint16_t ModBusSlaveData[MB_COUNT_MAX];
uint16_t default_par[MB_SETT_MAX - MB_SETT0] =
{
	0,			//	start not meter setting
	11520,		//	MB baudrate
	1,			//	MB slave
};

ModBusSlaveStruct ModBusSlave =
{
	.QuantReg = MB_COUNT_MAX,
	.SlaveAddrr = 1,
	.StopBit = 1,
	.Parity = 0,
	.MainClockUSART = 24000000,
	.Direct_GPIO = RS485_RX_TX_GPIO_Port,
	.Tx_GPIO = RS485_TX_GPIO_Port,
	.Rx_GPIO = RS485_RX_GPIO_Port,
	.Direct_pin = RS485_RX_TX_Pin_pos,
	.Tx_pin = RS485_TX_Pin_pos,
	.Rx_pin = RS485_RX_Pin_pos,
	.BaudRate = 115200,
	.Usart = USART3,
	.Tim = TIM2,
	.Data = ModBusSlaveData,
};

void SystemClock_Config(void);

void md_data_handling(void)
{
	static uint32_t out;
	uint32_t in = gpio_get_in();
	uint32_t out_rst = ((uint32_t)ModBusSlaveData[MB_OUT_RST_MSB] << 16) | ModBusSlaveData[MB_OUT_RST_LSB];
	uint32_t out_set = ((uint32_t)ModBusSlaveData[MB_OUT_SET_MSB] << 16) | ModBusSlaveData[MB_OUT_SET_LSB];
	uint32_t out_val = ((uint32_t)ModBusSlaveData[MB_OUT_MSB] << 16) | ModBusSlaveData[MB_OUT_LSB];
	out_val |= out_set;
	out_val &= ~out_rst;

	for (uint32_t i = 0; i < 32; i++)
	{
		uint8_t state = 0;
		uint8_t shift = offline ? 8 : 0;
		uint8_t mode = ModBusSlaveData[MB_SETT_OUT0 + i] >> shift;
		if ((mode >= OUT_TO_INV_IN_0) && ((mode - OUT_TO_INV_IN_0) < 20))
			state = in & (1 << (mode - OUT_TO_INV_IN_0)) ? 0 : 1;
		else if ((mode >= OUT_TO_IN_0) && ((mode - OUT_TO_IN_0) < 20))
			state = in & (1 << (mode - OUT_TO_IN_0)) ? 1 : 0;
		else if (mode == OUT_TO_HIGH)
			state = 1;
		else if (mode == OUT_TO_LOW)
			state = 0;
		else
			state = out_val & (1 << i) ? 1 : 0;

		if (state)
			out |= 1 << i;
		else
			out &= ~(1 << i);
	}

	ModBusSlaveData[MB_IN_MSB] = in >> 16;
	ModBusSlaveData[MB_IN_LSB] = in;
	ModBusSlaveData[MB_OUT_MSB] = out >> 16;
	ModBusSlaveData[MB_OUT_LSB] = out;
	ModBusSlaveData[MB_OUT_SET_MSB] = ModBusSlaveData[MB_OUT_SET_LSB] = 0;
	ModBusSlaveData[MB_OUT_RST_MSB] = ModBusSlaveData[MB_OUT_RST_LSB] = 0;

	gpio_set_out(out);

	for (uint32_t i = 0; i < AI_COUNT_MAX; i++)
		ModBusSlaveData[MB_AI_CH0 + i] = AI_rms[i] * 1000;

	uint8_t save = 0;
	for (uint32_t i = MB_SET_SETT0; i < MB_SET_SETT_MAX; i++)
	{
		if (ModBusSlaveData[i] != ModBusSlaveData[i - MB_SET_SETT0 + MB_SETT0])
		{
			ModBusSlaveData[i - MB_SET_SETT0 + MB_SETT0] = ModBusSlaveData[i];
			save = 1;
		}
	}
	if (save)
		arch_save_par((uint8_t *)&ModBusSlaveData[MB_SETT0], (MB_SETT_MAX - MB_SETT0) * 2);

	if (ModBusSlaveData[MB_RESET])
		NVIC_SystemReset();

	uint32_t address = ModBusSlaveData[MB_UPDATE_ADDR] * 4 + FLASH_BASE;
	if (address >= UPDATE_START && address < UPDATE_END)
	{
		switch(ModBusSlaveData[MB_UPDATE_CMD])
		{
		case UPDATE_CMD_READ:
		{
			memcpy(&ModBusSlaveData[MB_UPDATE_DATA], address, ModBusSlaveData[MB_UPDATE_LEN]);
			ModBusSlaveData[MB_UPDATE_CMD] |= 0x8000;
			break;
		}
		case UPDATE_CMD_WRITE:
		{
			HAL_FLASH_Unlock();
			for (uint32_t i = 0; i < ModBusSlaveData[MB_UPDATE_LEN]; i += 4)
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + i, *(uint32_t *)(((uint8_t *)&ModBusSlaveData[MB_UPDATE_DATA]) + i));
			HAL_FLASH_Lock();
			ModBusSlaveData[MB_UPDATE_CMD] |= 0x8000;
			break;
		}
		case UPDATE_CMD_ERASE:
		{
			FLASH_EraseInitTypeDef pEraseInit;
			uint32_t PageError;
			pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
			pEraseInit.Banks = FLASH_BANK_1;
			pEraseInit.NbPages = 1;
			pEraseInit.PageAddress = address;

			HAL_FLASH_Unlock();
			HAL_FLASHEx_Erase(&pEraseInit, &PageError);
			HAL_FLASH_Lock();
			ModBusSlaveData[MB_UPDATE_CMD] |= 0x8000;
			break;
		}
		case UPDATE_CMD_CHECK:
		{
			update_ctx_t su;
			ModBusSlaveData[MB_UPDATE_CMD] = update_check(&su) | 0x8000;
			break;
		}
		default:
			break;
		}
	}
	else
		ModBusSlaveData[MB_UPDATE_CMD] |= 0x4000;
}

void init_mb(void)
{
	memcpy((uint8_t *)&ModBusSlaveData[MB_SET_SETT0], (uint8_t *)&ModBusSlaveData[MB_SETT0], (MB_SETT_MAX - MB_SETT0) * 2);
	strcpy((char *)&ModBusSlaveData[MB_VERSION], VERSION " " __DATE__);

	ModBusSlave.BaudRate = ModBusSlaveData[MB_SETT_MB_BAUD] * 10;
	ModBusSlave.SlaveAddrr = ModBusSlaveData[MB_SETT_MB_SLAVE];

	__HAL_RCC_USART3_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();

	HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART3_IRQn);
	HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	ModBusSlaveInit(&ModBusSlave);
}

void setings(void)
{
	if (!arch_get_par((uint8_t *)&ModBusSlaveData[MB_SETT0], (MB_SETT_MAX - MB_SETT0) * 2) ||
		HAL_GPIO_ReadPin(RS485_DEFAULT_GPIO_Port, RS485_DEFAULT_Pin) == GPIO_PIN_SET)
	{
		memcpy((uint8_t *)&ModBusSlaveData[MB_SETT0], (uint8_t *)default_par, (MB_SETT_MAX - MB_SETT0) * 2);
	}
}

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_ADC1_Init();

	setings();
	init_mb();

	while (1)
	{
		if (!ModBusSlaveHandling(&ModBusSlave))
		{
			if (HAL_GetTick() > time_offline + 10000)
				offline = 1;
		}
		else
		{
			time_offline = HAL_GetTick();
			offline = 0;
		}

		md_data_handling();

		if (HAL_GetTick() / 300 % 2)
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
	}
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
								  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK);
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK);

	__HAL_RCC_HSE_CONFIG(RCC_HSE_OFF);
	__HAL_RCC_LSE_CONFIG(RCC_LSE_OFF);
}


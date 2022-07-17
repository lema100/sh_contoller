#include "gpio.h"
#include "adc.h"
#include "stm32f1xx_it.h"

GPIO_TypeDef * ports_out[] =
{
	LED1_GPIO_Port, LED2_GPIO_Port, LED3_GPIO_Port, LED4_GPIO_Port,
	HC595_GPIO_Port, HC595_GPIO_Port, HC595_GPIO_Port,
	NULL
};
uint16_t pins_out[] =
{
	LED1_Pin, LED2_Pin, LED3_Pin, LED4_Pin,
	HC595_CLK_Pin, HC595_LC_Pin, HC595_DATA_Pin,
};

GPIO_TypeDef * ports_in[] =
{
	DI1_GPIO_Port, DI2_GPIO_Port, DI3_GPIO_Port, DI4_GPIO_Port,
	DI5_GPIO_Port, DI6_GPIO_Port, DI7_GPIO_Port, DI8_GPIO_Port,
	DI9_GPIO_Port, DI10_GPIO_Port, (GPIO_TypeDef *)1, (GPIO_TypeDef *)1,
	DI13_GPIO_Port, DI14_GPIO_Port,	(GPIO_TypeDef *)1, (GPIO_TypeDef *)1,
	DI17_GPIO_Port, DI18_GPIO_Port,	(GPIO_TypeDef *)1, (GPIO_TypeDef *)1,
	RS485_DEFAULT_GPIO_Port,
	NULL
};
uint16_t pins_in[] =
{
	DI1_Pin, DI2_Pin, DI3_Pin, DI4_Pin,
	DI5_Pin, DI6_Pin, DI7_Pin, DI8_Pin,
	DI9_Pin, DI10_Pin, 1, 1,
	DI13_Pin, DI14_Pin,	1, 1,
	DI17_Pin, DI18_Pin,	1, 1,
	RS485_DEFAULT_Pin,
};

GPIO_TypeDef * ports_adc[] =
{
	ADC_DI11_GPIO_Port, ADC_VCC_GPIO_Port, ADC_DI19_GPIO_Port, ADC_DI20_GPIO_Port,
	ADC_DI15_GPIO_Port, ADC_DI16_GPIO_Port, ADC_DI12_GPIO_Port,
	NULL
};

uint16_t pins_adc[] =
{
	ADC_DI11_Pin, ADC_VCC_Pin, ADC_DI19_Pin, ADC_DI20_Pin,
	ADC_DI15_Pin, ADC_DI16_Pin, ADC_DI12_Pin,
};

uint32_t gpio_get_in(void)
{
	uint32_t ret = 0;
	for (uint8_t i = 0; ports_in[i] != NULL; i++)
	{
		if (ports_in[i] == (GPIO_TypeDef *)1)
			continue;
		if (HAL_GPIO_ReadPin(ports_in[i] , pins_in[i]) == GPIO_PIN_SET)
			ret |= (1 << i);
	}
	if (AI_rms[ADC_DI11] > ADC_THR)
		ret |= (1 << 10);
	if (AI_rms[ADC_DI12] > ADC_THR)
		ret |= (1 << 11);
	if (AI_rms[ADC_DI15] > ADC_THR)
		ret |= (1 << 14);
	if (AI_rms[ADC_DI16] > ADC_THR)
		ret |= (1 << 15);
	if (AI_rms[ADC_DI19] > ADC_THR)
		ret |= (1 << 18);
	if (AI_rms[ADC_DI20] > ADC_THR)
		ret |= (1 << 19);

	return ret;
}

static void gpio_out_delay(void)
{
	for(volatile uint32_t i = 0; i < 10; i++)
		asm("nop");
}

void gpio_set_out(uint32_t data)
{
	HAL_GPIO_WritePin(HC595_GPIO_Port, HC595_LC_Pin, GPIO_PIN_RESET);
	for (uint8_t i = 0; i < 32; i++)
	{
		HAL_GPIO_WritePin(HC595_GPIO_Port, HC595_CLK_Pin, GPIO_PIN_RESET);
		if (data & (1 << (31 - i)))
			HAL_GPIO_WritePin(HC595_GPIO_Port, HC595_DATA_Pin, GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(HC595_GPIO_Port, HC595_DATA_Pin, GPIO_PIN_RESET);
		gpio_out_delay();
		HAL_GPIO_WritePin(HC595_GPIO_Port, HC595_CLK_Pin, GPIO_PIN_SET);
	}
	gpio_out_delay();
	HAL_GPIO_WritePin(HC595_GPIO_Port, HC595_LC_Pin, GPIO_PIN_SET);
}

void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_AFIO_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_AFIO_REMAP_PD01_ENABLE();
	__HAL_AFIO_REMAP_SWJ_NOJTAG();

	for (uint8_t i = 0; ports_out[i] != NULL; i++)
	{
		GPIO_InitStruct.Pin = pins_out[i];
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(ports_out[i], &GPIO_InitStruct);
	}

	for (uint8_t i = 0; ports_in[i] != NULL; i++)
	{
		if (ports_in[i] == (GPIO_TypeDef *)1)
			continue;
		GPIO_InitStruct.Pin = pins_in[i];
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(ports_in[i], &GPIO_InitStruct);
	}

	for (uint8_t i = 0; ports_adc[i] != NULL; i++)
	{
		GPIO_InitStruct.Pin = pins_adc[i];
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(ports_adc[i], &GPIO_InitStruct);
	}
}


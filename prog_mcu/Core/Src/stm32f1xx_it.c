#include "main.h"
#include "stm32f1xx_it.h"
#include "mb_slave_uart.h"
#include "math.h"

extern ModBusSlaveStruct ModBusSlave;

void NMI_Handler(void)
{
	while (1)
	{
	}
}

void HardFault_Handler(void)
{
	while (1)
	{
	}
}

void MemManage_Handler(void)
{
	while (1)
	{
	}
}

void BusFault_Handler(void)
{
	while (1)
	{
	}
}

void UsageFault_Handler(void)
{
	while (1)
	{
	}
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
	HAL_IncTick();
}

void ADC1_2_IRQHandler(void)
{
}

void USART3_IRQHandler(void)
{
	ModBusSlaveInterruptUSART(&ModBusSlave);
}

void TIM2_IRQHandler(void)
{
	HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
	ModBusSlaveInterruptTIM(&ModBusSlave);
}

uint32_t AI_DMA[AI_COUNT_MAX];
uint32_t AI_acc[AI_COUNT_MAX];
float AI_rms[AI_COUNT_MAX];
float AI_rms_raw[AI_COUNT_MAX];
float AI_rms_scale[AI_COUNT_MAX];
uint32_t sample;

const float coeff[] =
{
	ADC_DI_DIVIDER,
	ADC_VCC_DIVIDER,
	ADC_DI_DIVIDER,
	ADC_DI_DIVIDER,
	ADC_DI_DIVIDER,
	ADC_DI_DIVIDER,
	ADC_DI_DIVIDER,
	ADC_DI_DIVIDER,
};

const int mul[] =
{
	1,
	1,
	1,
	1,
	1,
	1,
	1,
	1,
};


void DMA1_Channel1_IRQHandler(void)
{
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);

	for (uint8_t i = 0; i < AI_COUNT_MAX; i++)
	{
		AI_acc[i] += AI_DMA[i] * AI_DMA[i];
	}
	sample++;
	if (sample > SAMPLE_RATE)
	{
		for (uint8_t i = 0; i < AI_COUNT_MAX; i++)
		{
			AI_acc[i] *= mul[i];
			AI_rms_scale[i] = sqrt((float)AI_acc[i] / SAMPLE_RATE) / ADC_DESC;
			AI_rms_raw[i] = AI_rms_scale[i] * ADC_REF;
			AI_rms[i] = AI_rms_raw[i] * coeff[i];
			AI_acc[i] = 0;
		}

		sample = 0;
	}

	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
	DMA1->IFCR = 0x0FFFFFFF;
}

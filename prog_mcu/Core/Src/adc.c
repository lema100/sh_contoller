#include "adc.h"
#include "stm32f1xx_it.h"

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
TIM_HandleTypeDef htim1;

void MX_ADC1_Init(void)
{
	__HAL_RCC_DMA1_CLK_ENABLE();
	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_ADC1_CLK_ENABLE();

	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_OC_InitTypeDef sConfigOC;

	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 24 - 1;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 100 - 1;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK);
	if (HAL_TIM_PWM_Init(&htim1) != HAL_OK);

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 50;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK);

	ADC_ChannelConfTypeDef sConfig;

	hadc1.Instance = ADC1;
	hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = AI_COUNT_MAX;
	if (HAL_ADC_Init(&hadc1) != HAL_OK);

	const uint32_t ch_list[] =
	{
		ADC_CHANNEL_0,
		ADC_CHANNEL_1,
		ADC_CHANNEL_2,
		ADC_CHANNEL_3,
		ADC_CHANNEL_4,
		ADC_CHANNEL_8,
		ADC_CHANNEL_9,
	};

	for (uint32_t i = 0; i < AI_COUNT_MAX; i++)
	{
		sConfig.Channel = ch_list[i];
		sConfig.Rank = i + 1;
		sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
		if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK);
	}

	hdma_adc1.Instance = DMA1_Channel1;
	hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
	hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_adc1.Init.Mode = DMA_CIRCULAR;
	hdma_adc1.Init.Priority = DMA_PRIORITY_VERY_HIGH;
	if (HAL_DMA_Init(&hdma_adc1) != HAL_OK);

	__HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);

	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	HAL_DMA_Start_IT(&hdma_adc1, (uint32_t)&hadc1.Instance->DR, (uint32_t)AI_DMA, hadc1.Init.NbrOfConversion);

	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_1);

	SET_BIT(hadc1.Instance->CR2, ADC_CR2_DMA);
	SET_BIT(hadc1.Instance->CR2, ADC_CR2_EXTTRIG);
	ADC_Enable(&hadc1);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

}

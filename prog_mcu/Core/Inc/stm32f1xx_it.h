#ifndef __STM32F1xx_IT_H
#define __STM32F1xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif

#define ADC_THR			5.0	//	V
#define ADC_REF			3.3
#define ADC_DESC		4095.0
#define SAMPLE_RATE		200
#define AI_COUNT_MAX	7

#define ADC_VCC_DIVIDER		((10.0 + 1.0) / 1.0)
#define ADC_DI_DIVIDER		((2.4 + 0.5) / 0.5)

extern float AI_rms[AI_COUNT_MAX];
extern uint32_t AI_DMA[AI_COUNT_MAX];

enum
{
	ADC_DI11,
	ADC_VCC,
	ADC_DI19,
	ADC_DI20,
	ADC_DI15,
	ADC_DI16,
	ADC_DI12,
};


void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void ADC1_2_IRQHandler(void);
void USART3_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F1xx_IT_H */

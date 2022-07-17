#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define VERSION "V.1.0"

void Error_Handler(void);

#define RS485_TX_Pin_pos 		10
#define RS485_RX_Pin_pos 		11
#define RS485_RX_TX_Pin_pos		2

#define LED1_Pin 			GPIO_PIN_8
#define LED1_GPIO_Port 		GPIOA
#define LED2_Pin 			GPIO_PIN_9
#define LED2_GPIO_Port 		GPIOA
#define LED3_Pin 			GPIO_PIN_10
#define LED3_GPIO_Port 		GPIOA
#define LED4_Pin 			GPIO_PIN_11
#define LED4_GPIO_Port 		GPIOA

#define ADC_DI11_Pin		GPIO_PIN_0
#define ADC_DI11_GPIO_Port 	GPIOA
#define ADC_VCC_Pin			GPIO_PIN_1
#define ADC_VCC_GPIO_Port 	GPIOA
#define ADC_DI19_Pin		GPIO_PIN_2
#define ADC_DI19_GPIO_Port 	GPIOA
#define ADC_DI20_Pin		GPIO_PIN_3
#define ADC_DI20_GPIO_Port 	GPIOA
#define ADC_DI15_Pin		GPIO_PIN_4
#define ADC_DI15_GPIO_Port 	GPIOA
#define ADC_DI16_Pin		GPIO_PIN_0
#define ADC_DI16_GPIO_Port 	GPIOB
#define ADC_DI12_Pin		GPIO_PIN_1
#define ADC_DI12_GPIO_Port 	GPIOB

#define DI2_Pin				GPIO_PIN_15
#define DI2_GPIO_Port 		GPIOA
#define DI17_Pin			GPIO_PIN_0
#define DI17_GPIO_Port 		GPIOD
#define DI18_Pin			GPIO_PIN_1
#define DI18_GPIO_Port 		GPIOD
#define DI5_Pin				GPIO_PIN_3
#define DI5_GPIO_Port 		GPIOB
#define DI6_Pin				GPIO_PIN_4
#define DI6_GPIO_Port 		GPIOB
#define DI9_Pin				GPIO_PIN_5
#define DI9_GPIO_Port 		GPIOB
#define DI1_Pin				GPIO_PIN_6
#define DI1_GPIO_Port 		GPIOB
#define DI4_Pin				GPIO_PIN_7
#define DI4_GPIO_Port 		GPIOB
#define DI3_Pin				GPIO_PIN_13
#define DI3_GPIO_Port 		GPIOB
#define DI8_Pin				GPIO_PIN_14
#define DI8_GPIO_Port 		GPIOB
#define DI7_Pin				GPIO_PIN_15
#define DI7_GPIO_Port 		GPIOB
#define DI10_Pin			GPIO_PIN_13
#define DI10_GPIO_Port 		GPIOC
#define DI13_Pin			GPIO_PIN_14
#define DI13_GPIO_Port 		GPIOC
#define DI14_Pin			GPIO_PIN_15
#define DI14_GPIO_Port 		GPIOC

#define HC595_CLK_Pin		GPIO_PIN_5
#define HC595_LC_Pin		GPIO_PIN_6
#define HC595_DATA_Pin		GPIO_PIN_7
#define HC595_GPIO_Port		GPIOA

#define RS485_TX_Pin 		GPIO_PIN_10
#define RS485_TX_GPIO_Port 	GPIOB
#define RS485_RX_Pin 		GPIO_PIN_11
#define RS485_RX_GPIO_Port 	GPIOB
#define RS485_RX_TX_Pin 	GPIO_PIN_2
#define RS485_RX_TX_GPIO_Port 	GPIOB
#define RS485_DEFAULT_Pin 		GPIO_PIN_12
#define RS485_DEFAULT_GPIO_Port GPIOB

enum
{
	MB_IN_MSB = 0,
	MB_IN_LSB,
	MB_OUT_MSB,
	MB_OUT_LSB,
	MB_OUT_RST_MSB,
	MB_OUT_RST_LSB,
	MB_OUT_SET_MSB,
	MB_OUT_SET_LSB,
	MB_AI_CH0 = 10,
	MB_AI_CH1,
	MB_AI_CH2,
	MB_AI_CH3,
	MB_AI_CH4,
	MB_AI_CH5,
	MB_AI_CH6,
	MB_AI_CH7,
	MB_SETT0 = 30,
	MB_SETT_MB_BAUD,
	MB_SETT_MB_SLAVE,
	MB_SETT_OUT0 = 40,
	MB_SETT_MAX = 79,
	MB_VERSION = 80,
	MB_RESET = 99,
	MB_SET_SETT0 = 100,
	MB_SET_SETT_MB_BAUD,
	MB_SET_SETT_MB_SLAVE,
	MB_SET_SETT_OUT0 = 110,
	MB_SET_SETT_MAX = 149,
	MB_UPDATE_CMD = 256,
	MB_UPDATE_ADDR,
	MB_UPDATE_LEN,
	MB_UPDATE_DATA,
};

enum
{
	OUT_TO_MB = 0,
	OUT_TO_LOW,
	OUT_TO_HIGH,
	OUT_TO_IN_0 = 100,
	OUT_TO_INV_IN_0 = 120,
};

#define OUT_DEF	(OUT_TO_LOW << 8 | OUT_TO_MB)

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

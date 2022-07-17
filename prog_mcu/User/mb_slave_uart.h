#ifndef MB_SLAVE_UART_H_
#define MB_SLAVE_UART_H_

#include "main.h"
#include "crc16.h"

typedef struct
{
	uint16_t TransmitCount, RecievCount, CountTransData, EndFrame, QuantSend, CRC16W, TimeOutFlag;
	uint8_t RxD[256], TxD[256];
	uint32_t RS485_Driver_Cntl;
	uint16_t QuantReg, SlaveAddrr;
	GPIO_TypeDef *Direct_GPIO, *Tx_GPIO, *Rx_GPIO;
	uint32_t Direct_pin, Tx_pin, Rx_pin;
	uint8_t StopBit;					//	Количество Стоп битов (1,2)
	uint8_t Parity;						//	Контроль чётности (0 - None, 1 - Even, 2 - Odd)
	uint32_t BaudRate;					//	0...115200 бод
	uint32_t MainClockUSART;			//	Тактовая частота модуля переферии
	USART_TypeDef *Usart;
	TIM_TypeDef *Tim;
	uint16_t *Data;
}ModBusSlaveStruct;

void ModBusSlaveInterruptUSART(ModBusSlaveStruct *ctx);
void ModBusSlaveInterruptTIM(ModBusSlaveStruct *ctx);
uint8_t ModBusSlaveHandling(ModBusSlaveStruct *ctx);
void ModBusSlaveInit(ModBusSlaveStruct *ctx);

#endif

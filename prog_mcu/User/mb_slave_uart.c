#include "mb_slave_uart.h"
#include "main.h"
#include "crc16.h"

void ModBusSlaveInterruptUSART(ModBusSlaveStruct *ctx)
{
	if (ctx->Usart->SR & USART_SR_RXNE)
	{
		ctx->Usart->SR &= ~USART_SR_RXNE;
		ctx->RxD[ctx->RecievCount] = ctx->Usart->DR;
		ctx->RecievCount++;
		ctx->Tim->CNT = 0;
		ctx->Tim->CR1 |= TIM_CR1_CEN;
	}
	else if (ctx->Usart->SR & USART_SR_TC)
	{
		ctx->Usart->SR &= ~USART_SR_TC;
		if (ctx->TransmitCount >= ctx->QuantSend)
		{
			ctx->Usart->CR1 &= ~(USART_CR1_TCIE|USART_CR1_TE);
			ctx->Direct_GPIO->BRR |= 1 << ctx->Direct_pin;
		}
		else
		{
 			ctx->Direct_GPIO->BSRR |= 1 << ctx->Direct_pin;
			ctx->Usart->DR = ctx->TxD[ctx->TransmitCount];
			ctx->TransmitCount++;
		}
	}
	else
	{
		volatile uint8_t tmp = ctx->Usart->DR;
		ctx->Direct_GPIO->BRR |= 1 << ctx->Direct_pin;
		ctx->Usart->SR = 0;
	}
}

void ModBusSlaveInterruptTIM(ModBusSlaveStruct *ctx)
{
	ctx->Tim->SR = 0;
	ctx->Tim->CR1 &= ~TIM_CR1_CEN;
	ctx->TimeOutFlag = 1;
	ctx->EndFrame = ctx->RecievCount;
	ctx->RecievCount = 0;
}

uint8_t ModBusSlaveHandling(ModBusSlaveStruct *ctx)
{
	uint8_t ret = 0;

	if (ctx->TimeOutFlag)
	{
		if (ctx->RxD[0]==ctx->SlaveAddrr)
		{
			if (ctx->EndFrame > 2)
				ctx->CRC16W=CRC16((char *) &ctx->RxD, 0, (ctx->EndFrame - 2));
			if (((ctx->CRC16W&0x00FF)==ctx->RxD[ctx->EndFrame-2])&&((ctx->CRC16W>>8)==ctx->RxD[ctx->EndFrame-1]))
			{
				if (ctx->RxD[1]==0x03)
				{
					if ((((ctx->RxD[2]<<8)|ctx->RxD[3])+((ctx->RxD[4]<<8)|ctx->RxD[5]))<=ctx->QuantReg)
					{
						ctx->TxD[0]=ctx->SlaveAddrr;
						ctx->TxD[1]=0x03;
						uint16_t j =3;
						ctx->CountTransData=0;

						uint16_t start=ctx->RxD[2];
						start<<=8;
						start|=ctx->RxD[3];

						for (uint16_t i =start;i<(start+ctx->RxD[5]);++i){
							ctx->TxD[j]=*(ctx->Data+i)>>8;
							j++;
							ctx->TxD[j]=*(ctx->Data+i);
							j++;
							ctx->CountTransData++;
						}
						ctx->TxD[2]=ctx->CountTransData*2;
						ctx->CRC16W=CRC16((char *) &ctx->TxD, 0, (ctx->CountTransData*2+3));
						ctx->TxD[ctx->CountTransData*2+3]=ctx->CRC16W&0x00FF;
						ctx->TxD[ctx->CountTransData*2+4]=ctx->CRC16W>>8;
						ctx->TransmitCount=1;
						ctx->QuantSend=ctx->CountTransData*2+5;
						ctx->Direct_GPIO->BSRR |= 1<<ctx->Direct_pin;
						ctx->Usart->CR1 |= USART_CR1_TCIE|USART_CR1_TE;
						ctx->Usart->DR=ctx->TxD[0];
						ret = 1;
					}
					else
					{
						ctx->TxD[0]=ctx->SlaveAddrr;
						ctx->TxD[1]=0x80|ctx->RxD[1];
						ctx->TxD[2]=0x02;
						ctx->CRC16W=CRC16((char *) &ctx->TxD, 0, 3);
						ctx->TxD[3]=ctx->CRC16W&0x00FF;
						ctx->TxD[4]=ctx->CRC16W>>8;
						ctx->TransmitCount=1;
						ctx->QuantSend=5;
						ctx->Direct_GPIO->BSRR |= 1<<ctx->Direct_pin;
						ctx->Usart->CR1 |= USART_CR1_TCIE|USART_CR1_TE;
						ctx->Usart->DR=ctx->TxD[0];
					}
				}
				else
				{
					if (ctx->RxD[1]==0x10)
					{
						if ((((ctx->RxD[2]<<8)|ctx->RxD[3])+((ctx->RxD[4]<<8)|ctx->RxD[5]))<=ctx->QuantReg)
						{
							for (uint16_t i=((ctx->RxD[2]<<8)|ctx->RxD[3]);i<(((ctx->RxD[2]<<8)|ctx->RxD[3])+((ctx->RxD[4]<<8)|ctx->RxD[5]));++i)
							{
								*(ctx->Data+i)=(ctx->RxD[7+(i-((ctx->RxD[2]<<8)|ctx->RxD[3]))*2]<<8)|ctx->RxD[8+(i-((ctx->RxD[2]<<8)|ctx->RxD[3]))*2];
							}
							ctx->TxD[0]=ctx->SlaveAddrr;
							ctx->TxD[1]=ctx->RxD[1];
							ctx->TxD[2]=ctx->RxD[2];
							ctx->TxD[3]=ctx->RxD[3];
							ctx->TxD[4]=ctx->RxD[4];
							ctx->TxD[5]=ctx->RxD[5];
							ctx->CRC16W=CRC16((char *) &ctx->TxD, 0, 6);
							ctx->TxD[6]=ctx->CRC16W&0x00FF;
							ctx->TxD[7]=ctx->CRC16W>>8;
							ctx->TransmitCount=1;
							ctx->QuantSend=8;
							ctx->Direct_GPIO->BSRR |= 1<<ctx->Direct_pin;
							ctx->Usart->CR1 |= USART_CR1_TCIE|USART_CR1_TE;
							ctx->Usart->DR=ctx->TxD[0];
							ret = 1;
						}
						else
						{
							ctx->TxD[0]=ctx->SlaveAddrr;
							ctx->TxD[1]=0x80|ctx->RxD[1];
							ctx->TxD[2]=0x02;
							ctx->CRC16W=CRC16((char *) &ctx->TxD, 0, 3);
							ctx->TxD[3]=ctx->CRC16W&0x00FF;
							ctx->TxD[4]=ctx->CRC16W>>8;
							ctx->TransmitCount=1;
							ctx->QuantSend=5;
							ctx->Direct_GPIO->BSRR |= 1<<ctx->Direct_pin;
							ctx->Usart->CR1 |= USART_CR1_TCIE|USART_CR1_TE;
							ctx->Usart->DR=ctx->TxD[0];
						}
					}
					else
					{
						if (ctx->RxD[1]==0x06)
						{
							if (((ctx->RxD[2]<<8)|ctx->RxD[3])<ctx->QuantReg)
							{
								*(ctx->Data+((ctx->RxD[2]<<8)|ctx->RxD[3]))=(ctx->RxD[4]<<8)|ctx->RxD[5];
								ctx->TxD[0]=ctx->SlaveAddrr;
								ctx->TxD[1]=ctx->RxD[1];
								ctx->TxD[2]=ctx->RxD[2];
								ctx->TxD[3]=ctx->RxD[3];
								ctx->TxD[4]=ctx->RxD[4];
								ctx->TxD[5]=ctx->RxD[5];
								ctx->CRC16W=CRC16((char *) &ctx->TxD, 0, 6);
								ctx->TxD[6]=ctx->CRC16W&0x00FF;
								ctx->TxD[7]=ctx->CRC16W>>8;
								ctx->TransmitCount=1;
								ctx->QuantSend=8;
								ctx->Direct_GPIO->BSRR |= 1<<ctx->Direct_pin;
								ctx->Usart->CR1 |= USART_CR1_TCIE|USART_CR1_TE;
								ctx->Usart->DR=ctx->TxD[0];
								ret = 1;
							}
							else
							{
								ctx->TxD[0]=ctx->SlaveAddrr;
								ctx->TxD[1]=0x80|ctx->RxD[1];
								ctx->TxD[2]=0x02;
								ctx->CRC16W=CRC16((char *) &ctx->TxD, 0, 3);
								ctx->TxD[3]=ctx->CRC16W&0x00FF;
								ctx->TxD[4]=ctx->CRC16W>>8;
								ctx->TransmitCount=1;
								ctx->QuantSend=5;
								ctx->Direct_GPIO->BSRR |= 1<<ctx->Direct_pin;
								ctx->Usart->CR1 |= USART_CR1_TCIE|USART_CR1_TE;
								ctx->Usart->DR=ctx->TxD[0];
							}
						}
						else
						{
							ctx->TxD[0]=ctx->SlaveAddrr;
							ctx->TxD[1]=0x80|ctx->RxD[1];
							ctx->TxD[2]=0x01;
							ctx->CRC16W=CRC16((char *) &ctx->TxD, 0, 3);
							ctx->TxD[3]=ctx->CRC16W&0x00FF;
							ctx->TxD[4]=ctx->CRC16W>>8;
							ctx->TransmitCount=1;
							ctx->QuantSend=5;
							ctx->Direct_GPIO->BSRR |= 1<<ctx->Direct_pin;
							ctx->Usart->CR1 |= USART_CR1_TCIE|USART_CR1_TE;
							ctx->Usart->DR=ctx->TxD[0];
						}
					}
				}
			}
		}
		ctx->TimeOutFlag=0;
	}
	return ret;
}

void ModBusSlaveInit(ModBusSlaveStruct *ctx)
{
	//	Initialization GPIO
	//	Переводим нужный пин в состояние выхода
	if (ctx->Direct_pin>=8)
	{
		ctx->Direct_GPIO->CRH&=~(0b1111<<((ctx->Direct_pin-8)*4));
		ctx->Direct_GPIO->CRH|=0b0001<<((ctx->Direct_pin-8)*4);
	}
	else
	{
		ctx->Direct_GPIO->CRL&=~(0b1111<<((ctx->Direct_pin)*4));
		ctx->Direct_GPIO->CRL|=0b0001<<((ctx->Direct_pin)*4);
	}
	//	Переводим нужный пин в состояние выхода с альтернативным применением
	if (ctx->Tx_pin>=8)
	{
		ctx->Tx_GPIO->CRH&=~(0b1111<<((ctx->Tx_pin-8)*4));
		ctx->Tx_GPIO->CRH|=0b1001<<((ctx->Tx_pin-8)*4);
	}
	else
	{
		ctx->Tx_GPIO->CRL&=~(0b1111<<((ctx->Tx_pin)*4));
		ctx->Tx_GPIO->CRL|=0b1101<<((ctx->Tx_pin)*4);
	}
	//	Переводим нужный пин в состояние входа с альтернативным применением
	if (ctx->Rx_pin>=8)
	{
		ctx->Rx_GPIO->CRH&=~(0b1111<<((ctx->Rx_pin-8)*4));
		ctx->Rx_GPIO->CRH|=0b1000<<((ctx->Rx_pin-8)*4);
	}
	else
	{
		ctx->Rx_GPIO->CRL&=~(0b1111<<((ctx->Rx_pin)*4));
		ctx->Rx_GPIO->CRL|=0b1000<<((ctx->Rx_pin)*4);
	}

	ctx->Direct_GPIO->BRR|=1<<ctx->Direct_pin;

	//	Initialization USART
	ctx->Usart->CR1=USART_CR1_UE|USART_CR1_RXNEIE|USART_CR1_RE;
	if (ctx->Parity==1)
	{
		ctx->Usart->CR1|=USART_CR1_PCE;
		ctx->Usart->CR1&=~USART_CR1_PS;
	}
	else
	{
		if (ctx->Parity==2)
		{
			ctx->Usart->CR1|=USART_CR1_PCE;
			ctx->Usart->CR1|=USART_CR1_PS;
		}
		else
		{
			ctx->Usart->CR1&=~USART_CR1_PCE;
			ctx->Usart->CR1&=~USART_CR1_PS;
		}
	}

	if ((ctx->StopBit==0)||(ctx->StopBit==1))
	{
		ctx->Usart->CR2&=~USART_CR2_STOP_0;
		ctx->Usart->CR2&=~USART_CR2_STOP_1;
	}
	else
	{
		ctx->Usart->CR2&=~USART_CR2_STOP_0;
		ctx->Usart->CR2|=USART_CR2_STOP_1;
	}

	ctx->Usart->CR2 |= USART_CR2_LBDL;
	ctx->Usart->BRR=ctx->MainClockUSART/ctx->BaudRate;

	//	Initialization timer
	ctx->Tim->DIER|=TIM_DIER_UIE;
	ctx->Tim->PSC=ctx->MainClockUSART/ctx->BaudRate;
	ctx->Tim->ARR=39;
}

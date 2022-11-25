#ifndef _NET_IO_H_
#define _NET_IO_H_


#include "stm32f1xx.h"




typedef struct
{
	
	unsigned short dataLen;
	unsigned short dataLenPre;
	
	unsigned char buf[320];

} NET_IO_INFO;

#define REV_OK		0
#define REV_WAIT	1

#define NET_IO		(&huart2)

extern NET_IO_INFO netIOInfo;
extern uint8_t Uart1_ReadCache[10];
extern _Bool reconnect_flag;






void NET_IO_Init(void);

void NET_IO_Send(unsigned char *str, unsigned short len);

_Bool NET_IO_WaitRecive(void);

void NET_IO_ClearRecive(void);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);


#endif

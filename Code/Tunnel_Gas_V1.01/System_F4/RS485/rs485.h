#ifndef __RS485_H
#define __RS485_H	 
#include "sys.h"
#include "usart.h"

void RS485_GPIO_Init(uint8_t usart_num,uint32_t baud);
uint16_t RS485_CRC16_Calculate(uint8_t *buffer,uint8_t length);
Error_Code_t RS485_Send_Data(uint8_t usart_num,uint8_t *Send_buffer,uint8_t length);
uint8_t RS485_Receive_Data(uint8_t usart_num,uint8_t  *data);

#endif

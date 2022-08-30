#ifndef UART_H
#define UART_H

#include "gd32e230.h"
/*�����������ݻ�����*/
extern uint8_t rx0_data_buff[256];
/*���������ݸ���*/
extern  uint32_t USART0_RX_NUM;

/*�����������ݻ�����*/
extern uint8_t rx1_data_buff[256];
/*���������ݸ���*/
extern  uint32_t USART1_RX_NUM;

void usart_init(uint32_t usart_periph ,uint32_t baudrate);
void USART0_IRQHandler(void);
void USART1_IRQHandler(void);
void USART_Data_Send(uint32_t usart_periph,uint8_t ,uint8_t *data_point,uint8_t len);

#endif
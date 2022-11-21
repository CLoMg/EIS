#ifndef RS485_H
#define RS485_H

#include <stdint.h>




void rs485_init(uint32_t usart_periph ,uint32_t baudrate);

void rs485_Data_Send(uint32_t usart_periph,uint8_t *data_point,uint8_t len);
#endif

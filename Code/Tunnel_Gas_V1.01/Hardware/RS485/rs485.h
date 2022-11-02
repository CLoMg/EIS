#ifndef __RS485_H
#define __RS485_H	 
#include "sys.h"
#include "usart.h"


Error_Code_t RS485_Get_Value(enum gas_type gas,uint8_t device_addr,uint16_t memory_addr,uint16_t *data);
uint16_t RS485_CRC16_Calculate(uint8_t *buffer,uint8_t length);
Error_Code_t RS485_Send_Data(enum gas_type gas,uint8_t *Send_buffer,uint8_t length);
Error_Code_t RS485_Receive_Data(enum gas_type gas,uint16_t  *data);

#endif

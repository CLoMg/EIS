#ifndef __UESTC_GAS_H
#define __UESTC_GAS_H
#include "sys.h"

void UESTC_Gas_GPIO_Init(uint8_t usart_num,uint32_t buad);
void UESTC_Gas_Init(uint8_t usart_num,uint32_t buad);
Error_Code_t UESTC_Gas_Conc_Read(uint8_t usart_num,uint8_t slave_addr,uint8_t *data_buff);
void UESTC_Gas_ID_Read(uint8_t usart_num,uint8_t slave_addr,uint8_t *data_buff);
#endif

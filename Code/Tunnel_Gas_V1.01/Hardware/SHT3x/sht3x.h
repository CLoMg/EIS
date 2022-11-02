#ifndef SHT3x_H
#define SHT3x_H

#include "sys.h"

extern u8 humiture_buff1[20];
extern u8 humiture_buff2[20];
extern u8 Refresh_SHT30_Data;
extern u8 send_data_fleg;
extern u8 Temperature_L;
extern u8 Humidity_L;
extern u8 Temperature_H;
extern u8 Humidity_H;

Error_Code_t SHT30_Acquire (u8 slave_addr,uint8_t *data_buff);
Error_Code_t SHT30_read_result(u8 slave_addr,uint8_t *data_buff);
void SHT30_Init(void);

 

#endif

 

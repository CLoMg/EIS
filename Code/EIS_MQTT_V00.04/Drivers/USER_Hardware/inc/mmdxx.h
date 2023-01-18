#ifndef __MMDXX_H
#define __MMDXX_H
#include "sys.h"

//定义传感器3.3v供电使能引脚 旧版PC2 新版PD10
//#define Sensor3V3_Power PCout(2)
#define Sensor3V3_Power PDout(10)

void Sensor3V3_Power_init(void);
void MMDxx_Init(u8 sensor_num,u32 baud);
void MMDxx_GPIO_Init(u8 sensor_num,u32 baud);
void MMDxx_Update_Mode_Change(u8 sensor_num);
Error_Code_t MMDxx_Data_Query(uint8_t len, ...);
void MMDxx_Command_Send(uint8_t sensor_num,uint8_t *command,uint8_t length);
uint32_t MMDxx_Data_Decode(uint8_t *result_point,uint8_t *source_point);
_Bool MMDxx_Data_Receive(uint8_t sensor_num,uint8_t *data_buff);
uint8_t  MMDxx_getCheckSum(uint8_t *packet,uint8_t length);


#endif

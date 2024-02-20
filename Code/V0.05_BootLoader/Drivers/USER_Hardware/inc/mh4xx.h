#ifndef __MH4xx_H
#define __MH4xx_H	 
#include "usart.h"
#include "delay.h"
#include <stdarg.h>

typedef struct
{
	
	unsigned short dataLen;
	unsigned short dataLenPre;
	
	unsigned char buf[100];

} GAS_IO_INFO;

enum Gas_Type
{
	CO = 0x01,
	NO2,

};

extern float CO_Data,NO2_Data;
extern float precision[4];
//定义哪些串口连接了气体传感器， 对应位 1- 连接 0-未连接



#define resolution 1
#define ZE_Init_Mode 0x03 //ZExx系列主动上传模式
#define ZE_Q_A_Mode 0x04 //ZExx系列问答模式
#define ZH_Init_Mode 0x40 //ZExx系列主动上传模式
#define ZH_Q_A_Mode 0x41 //ZExx系列问答模式
#define ZH_Dormant_In 0x01 //ZExx系列主动上传模式
#define ZH_Dormant_Out 0x00 //ZExx系列问答模式

void MH4xx_ZE_Init_All(void);
_Bool MH4xx_ZE_Init(uint8_t,unsigned int baud);
int Gas_Get_Value(enum Gas_Type gas);
void Gas_GetValue_ALL(void);
void ZExx_State_Change(uint8_t sensor_num,uint8_t update_mode);
_Bool MH4xx_Calibrate(uint8_t,uint16_t span_value);
_Bool MH4xx_Write_Read(enum Gas_Type gas,uint8_t command,...);
_Bool MH4xx_Calibrate_Zero(uint8_t sensor_num);
_Bool MH4xx_Calibrate_Span(uint8_t sensor_num,uint16_t);

_Bool ZExx_Get_Value(uint8_t sensor_num,uint16_t *data);
void GAS_Seneor_State_Change(uint8_t sensor_num,uint8_t update_mode);

_Bool ZHxx_Get_Value(uint8_t sensor_num,uint16_t *data);
_Bool ZH_Dormant_Change(uint8_t sensor_num,uint8_t mode);

void GAS_IO_Init(void);
void GAS_IO_ClearRecive(enum Gas_Type gas);
void GAS_IO_Send(enum Gas_Type gas,unsigned char *str, unsigned short len);
_Bool GAS_IO_WaitRecive(enum Gas_Type gas);
void GAS_UARTEx_RxEventCallback(enum  Gas_Type,unsigned short);

char getCheckSum(uint8_t *);


			    
#endif


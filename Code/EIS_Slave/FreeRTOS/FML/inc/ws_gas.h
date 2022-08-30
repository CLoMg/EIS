#ifndef __WS_GAS_H
#define __WS_GAS_H	 

#include <stdarg.h>

#define Error_Code_t _Bool

extern float precision[4];
//定义哪些串口连接了气体传感器， 对应位 1- 连接 0-未连接

typedef enum WS_Sensor_Serial{
	WS_CH4 = 0x01,WS_NH3,WS_CO,WS_SO2=0x05,WS_H2S,WS_NO2,WS_CO2,
}WS_Serial_T;

typedef enum WS_Sensor_Mode{
	Active = 0x03, QA
}WS_Mode_T;

typedef struct WS_Sensor
{
	WS_Serial_T  Serial;
	WS_Mode_T Mode;
	unsigned int Usart_Periph;
	unsigned char Usart_Channel;
	unsigned int Usart_BaudRate;
	unsigned short Cur;
	unsigned short Pre;
	unsigned short Delta;
	unsigned short Buff[5];
	unsigned short Mean;
	unsigned char  Alar_Count;
}WS_Sensor_T;


extern WS_Sensor_T *CO2_instance;
/******MH4xx系列传感器接口函数******/
WS_Sensor_T *WS_Sensor_Creat(void);
void WS_Sensor_Param_Config(WS_Sensor_T *instance,WS_Serial_T serial,unsigned int usart_periph,unsigned int baudrate);
void WS_IO_Init(WS_Sensor_T *instance);
void WS_Sensor_Init();
void WS_Mode_Change(WS_Sensor_T *instance);
void WS_Acquire_ALL(void);
Error_Code_t WS_Get_Value(WS_Sensor_T *);
Error_Code_t WS_Write_Read(unsigned int usart_periph,unsigned char,int command,...);
//MH4xx校准零点
Error_Code_t 	WS_Calibrate_Zero(WS_Sensor_T *instance);
//MH4xx校准跨度值
Error_Code_t 	WS_Calibrate_Span(WS_Sensor_T *instance,unsigned short span_value);
char getCheckSum(unsigned char *packet);   
#endif


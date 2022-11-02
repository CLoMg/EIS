#ifndef __MH4xx_H
#define __MH4xx_H	 
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include <stdarg.h>

extern float precision[4];
//������Щ�������������崫������ ��Ӧλ 1- ���� 0-δ����

#define resolution 1
#define ZE_Init_Mode 0x03 //ZExxϵ�������ϴ�ģʽ
#define ZE_Q_A_Mode 0x04 //ZExxϵ���ʴ�ģʽ
#define ZH_Init_Mode 0x40 //ZExxϵ�������ϴ�ģʽ
#define ZH_Q_A_Mode 0x41 //ZExxϵ���ʴ�ģʽ
#define ZH_Dormant_In 0x01 //ZExxϵ�������ϴ�ģʽ
#define ZH_Dormant_Out 0x00 //ZExxϵ���ʴ�ģʽ

void MH4xx_ZE_Init_All(u32 baud);
Error_Code_t MH4xx_ZE_Init(uint8_t,u32 baud);
Error_Code_t MH4xx_ZE_Get_Value(uint8_t,uint8_t *data);
void ZExx_State_Change(uint8_t sensor_num,uint8_t update_mode);
Error_Code_t MH4xx_Calibrate(uint8_t,uint16_t span_value);
Error_Code_t MH4xx_Write_Read(uint8_t sensor_num,uint8_t command,...);
Error_Code_t 	MH4xx_Calibrate_Zero(uint8_t sensor_num);
Error_Code_t 	MH4xx_Calibrate_Span(uint8_t sensor_num,uint16_t);

Error_Code_t ZExx_Get_Value(uint8_t sensor_num,uint16_t *data);
Error_Code_t ZE_ZH_Mode_Change(uint8_t sensor_num,uint8_t mode);

Error_Code_t ZHxx_Get_Value(uint8_t sensor_num,uint16_t *data);
Error_Code_t ZH_Dormant_Change(uint8_t sensor_num,uint8_t mode);

char getCheckSum(uint8_t *);


			    
#endif


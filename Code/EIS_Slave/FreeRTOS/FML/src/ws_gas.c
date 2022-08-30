#include "ws_gas.h"
#include "stdlib.h"
#include <stdarg.h>
#include <string.h>
#include "uart.h"
#include "gd32e230c_eval.h"

WS_Sensor_T *CO2_instance;

		

/******MH4xx系列传感器接口函数******/
WS_Sensor_T *WS_Sensor_Creat(void)
{
	WS_Sensor_T *instance  = (WS_Sensor_T*)malloc(sizeof(WS_Sensor_T));
	return instance;
}

void WS_Sensor_Param_Config(WS_Sensor_T *instance,WS_Serial_T serial,uint32_t usart_periph,uint32_t baudrate)
{
	instance->Serial = serial;
	instance->Mode = QA;
	instance->Usart_Periph = usart_periph;
	instance->Usart_BaudRate = baudrate;
	instance->Alar_Count = 0;
	instance->Cur = 0;
	instance->Pre = 0;
	instance->Delta = 0;
	instance->Mean = 0;
	memset(instance->Buff,0,5);
};

void WS_IO_Init(WS_Sensor_T *instance)
{
	usart_init(instance->Usart_Periph,instance->Usart_BaudRate);		
}
void WS_Sensor_Init()
{
	CO2_instance = WS_Sensor_Creat();
	WS_Sensor_Param_Config(CO2_instance,WS_CO2,USART0,9600);
	WS_IO_Init(CO2_instance);
	WS_Mode_Change(CO2_instance);
}

//
void WS_Mode_Change(WS_Sensor_T *instance){
//	USART_TypeDef* USARTx;
	uint8_t newmode=instance->Mode;

	delay_ms(500);
	WS_Write_Read(instance->Usart_Periph,0x78,newmode);
	
	delay_ms(200);
}

void WS_Acquire_ALL(void)
{
	WS_Get_Value(CO2_instance);
}
//MH4xx 读取气体浓度值
Error_Code_t WS_Get_Value(WS_Sensor_T *instance){
	uint8_t *read_buff;
	Error_Code_t Error=0;
	uint8_t sensor_data[6];
	uint8_t count=3;
	while((!(Error=WS_Write_Read(instance->Usart_Periph,instance->Usart_Channel,0x86,sensor_data)))&&((count--)>0));
	//CHECK_ERROR(Error);
	if(Error){
		instance->Pre = instance->Cur;
		instance->Cur = (uint16_t)(sensor_data[0]<<8)|(sensor_data[1]);
		instance->Delta = instance->Cur - instance->Pre;
	}
	else
	{
		/*采集失败时如何处理？*/
	}
	////printf("sensor %d data is %d\r\n",sensor_num,((uint16_t)sensor_data[0]<<8|sensor_data[1]));
	return Error;
}

//MH4xx校准
Error_Code_t WS_Calibrate(WS_Sensor_T *instance,uint16_t span_value){
	Error_Code_t Error=0;
	Error=WS_Calibrate_Zero(instance);
	//delay_ms(5000);
	Error=WS_Calibrate_Span(instance,span_value);
	return Error;
}

//MH4xx串口发送接收数据
Error_Code_t WS_Write_Read(uint32_t usart_periph,uint8_t ch,int32_t command,...){
	Error_Code_t Error=0;
	
	uint8_t *data,span_value;
	uint8_t i,Send_buffer[9]={0xff,0x01,},Read_buffer[9];
	va_list argptr;
	va_start(argptr,command);
	
	//CHECK_ERROR(Error);	
	
	Send_buffer[2] = command;
	//根据可变参数个数判断buffer[3~4]是否写入
	if(command==0x88){
		span_value=va_arg(argptr,int32_t);
		Send_buffer[3] = (uint8_t)(span_value>>8);
		Send_buffer[4] = (uint8_t)(span_value);
	}
	else if(command==0x86)
		 data=va_arg(argptr,uint8_t *);
	else if(command==0x78||command==0xA7){
		Send_buffer[3] = va_arg(argptr,int32_t);
		data=va_arg(argptr,uint8_t *);
	}
	else ;
	va_end(argptr);
	
	 Send_buffer[8]=getCheckSum(&Send_buffer[0]);
	 USART_Data_Send(usart_periph,ch,Send_buffer,9);
	 delay_ms(30);
	 
	 //运用switch case：来判断对应串口接收的数据量？？？
	 
	 if(USART0_RX_NUM>=9){
		 memcpy(Read_buffer,&rx0_data_buff,9);
		 if((Read_buffer[0]==0xFF)&&(Read_buffer[1]==0x86||Read_buffer[1]==0x78||Read_buffer[1]==0xA7)&&(getCheckSum(Read_buffer)==Read_buffer[8])){
				for(int i=2;i<8;++i){
					 *(data++) = Read_buffer[i];	
					 Error=1;
				}
	//测试时输出至串口1，后续添加输出接口
			// //printf("gas_concentation of sensor %d: %d\n",sensor_num,*(data-2));
		 }
 
	}
	 memset(rx0_data_buff,0,256);
	 USART0_RX_NUM=0;
	 return Error;
}

//MH4xx校准零点
Error_Code_t 	WS_Calibrate_Zero(WS_Sensor_T *instance){
	Error_Code_t error=0;
	error=WS_Write_Read(instance->Usart_Periph,instance->Usart_Channel,0x87);
	return error;
}

//MH4xx校准跨度值
Error_Code_t 	WS_Calibrate_Span(WS_Sensor_T *instance,uint16_t span_value){
	Error_Code_t error=0;
	error=WS_Write_Read(instance->Usart_Periph,0x88,span_value);
	return error;
}
/********End*******************/

//数据校验值计算
char getCheckSum(uint8_t *packet){
	uint8_t i,checksum=0;
	for(i=1;i<8;++i){
		checksum +=packet[i];
	}
	checksum = 0xff - checksum;
	checksum += 1;
	return checksum;
}

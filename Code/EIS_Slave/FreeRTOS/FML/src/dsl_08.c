#include  "dsl_08.h"
#include "uart.h"
#include "stdlib.h"
#include "string.h"
#include "gd32e230c_eval.h"

#define PM_Delta_Max 10
static uint8_t DSL_CMD_Buff[9]={0xAA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xBB};
static uint8_t DSL_Receive_Buff[32];
unsigned char  DSL_buff_index = 0;

DSL_Sensor_T *dsl_instance;

/******MH4xx系列传感器接口函数******/
DSL_Sensor_T *DSL_Sensor_Creat(void)
{
	DSL_Sensor_T *instance  = (DSL_Sensor_T*)malloc(sizeof(DSL_Sensor_T));
	return instance;
}
void AQI_Process(void){
	if(DSL_Data_Read(dsl_instance)){
			unsigned char i = 0;
			dsl_instance->Index++;
			dsl_instance->Index %= 5;
			dsl_instance->Buff[dsl_instance->Index] = dsl_instance->Cur;
			dsl_instance->Delta = dsl_instance->Cur - dsl_instance->Pre;
			dsl_instance->Pre = dsl_instance->Cur;
			if(dsl_instance->Delta > PM_Delta_Max)
				dsl_instance->Alar_Count++;
			dsl_instance->Mean=0;
			for(; i < 5 ; i++){
				dsl_instance->Mean += dsl_instance->Buff[i];
				dsl_instance->Mean /=2;
			}
		}
}
void DSL_Sensor_Param_Config(DSL_Sensor_T *instance,uint32_t usart_periph,uint32_t baudrate,uint8_t channel)
{
	instance->Mode = DSL_QA;
	instance->Usart_Periph = usart_periph;
	instance->Usart_BaudRate = baudrate;
	instance->Usart_buff_point = usart_periph==USART0? rx0_data_buff : rx1_data_buff;
	instance->Usart_Channel = channel;
	instance->Usart_RX_NUM_Point = usart_periph==USART0? (&USART0_RX_NUM): (&USART1_RX_NUM);
	instance->Alar_Count = 0;
	instance->Index = 0;
	instance->Cur = 0;
	instance->Pre = 0;
	instance->Delta = 0;
	instance->Mean = 0;
	memset(instance->Buff,0,5);
};

/* 初始化DSL-08 IO口 usart 端口
Reset -- PC2    0-- reset
Set   -- PC1  : 1 -- Nomal  0--Sleep
*/
void DSL_IO_Init(DSL_Sensor_T *instance)
{
	usart_init(instance->Usart_Periph,instance->Usart_BaudRate);
	dsl_board_init();
}
void DSL_Sensor_Init()
{
	dsl_instance = DSL_Sensor_Creat();
	DSL_Sensor_Param_Config(dsl_instance,USART0,9600,3);
	DSL_IO_Init(dsl_instance);
	DSL_Mode_Change(dsl_instance);
}

void DSL_Mode_Change(DSL_Sensor_T *instance)
{
	DSL_Send_CMD(instance,Stop_Continuous_Measure);
}

_Bool DSL_Data_Read(DSL_Sensor_T *instance){
	_Bool Error=0;
	uint8_t count=5;
	do{
	DSL_Send_CMD(instance,PM25_PM10_Read);
	delay_ms(200);
	}
	while((!(Error=DSL_Receive_Data(instance)))&&((count--)>0));
	//CHECK_ERROR(Error);
	if(Error){
			instance->Cur = (uint16_t)(DSL_Receive_Buff[2]<<8) | DSL_Receive_Buff[3];
	}
	else 
	{
			
	}
	return Error;
}

static void DSL_Data_Calc(DSL_Sensor_T *instance)
{
	unsigned char i = 0;
	if(DSL_buff_index > 4)
		DSL_buff_index=0;
	instance->Buff[DSL_buff_index++] = instance->Cur;
	instance->Delta = instance->Cur - instance->Pre;
	instance->Pre = instance->Cur;
	if(instance->Delta > Delta_Max)
		instance->Alar_Count++;
	instance->Mean=0;
  for(; i < 5 ; i++){
		instance->Mean += instance->Buff[DSL_buff_index];
		instance->Mean /=2;
	}
}


void DSL_Send_CMD(DSL_Sensor_T *instance,enum DSL_CMD cmd){

	DSL_CMD_Buff[1]=cmd;

	uint16_t check_sum=DSL_LRC_Check(DSL_CMD_Buff,9);
	DSL_CMD_Buff[6]=(uint8_t)(check_sum>>8);
	DSL_CMD_Buff[7]=(uint8_t)check_sum;

	uint8_t length=9;
	memset(instance->Usart_buff_point,0,256);
	USART_Data_Send(instance->Usart_Periph,instance->Usart_Channel,DSL_CMD_Buff,length);
}

_Bool DSL_Receive_Data(DSL_Sensor_T *instance){
	_Bool Error=0;
	uint8_t Read_buffer[32];
//	usart_num=1;
	
	if((*instance->Usart_RX_NUM_Point)>=9){
		 uint8_t length=(*instance->Usart_RX_NUM_Point);
		 memcpy(Read_buffer,instance->Usart_buff_point,length);
		 if((Read_buffer[0]==0x42)&&(Read_buffer[0]==0x4D)&&(DSL_LRC_Check(Read_buffer,length)==((uint16_t)(Read_buffer[length-2]<<8)|(Read_buffer[length-1])))){
				for(int i=4;i<length;++i){
						DSL_Receive_Buff[i] = Read_buffer[i];	
				}
				 Error=1;
		 }
		 else if((Read_buffer[0]==0xAA)&&(Read_buffer[8]==0xBB)&&(DSL_LRC_Check(Read_buffer,length)==((uint16_t)(Read_buffer[length-3]<<8)|(Read_buffer[length-2])))){
				for(int i=1;i<length;++i){
						DSL_Receive_Buff[i] = Read_buffer[i];	
				}
				 Error=1;
		 }
 
	}
	*instance->Usart_RX_NUM_Point=0;
	return Error;
}

	
uint16_t DSL_LRC_Check(uint8_t *data_buffer,uint8_t length){
	uint16_t  sum=0;
	if(length<=9){
	for(int i=0;i<6;++i)
		sum+= data_buffer[i];
	sum+=data_buffer[8];
	}
	else{
		for(int i=0;i<length-2;++i)
		 sum+= data_buffer[i];
	}
	
	return sum;
}


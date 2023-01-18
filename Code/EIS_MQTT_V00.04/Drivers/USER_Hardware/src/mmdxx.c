#include "mmdxx.h"
#include "usart.h"
#include "string.h"
#include "delay.h"
#include "LORA.h"
#include "filter.h"
#include <stdarg.h>

static Data_Struct_T *GAS_Instance[7];
uint8_t Mode_Switch_Command[]={0xAA,0x00,0xFA};
uint8_t Data_Query_Command[]={0xAA,0x00,0xF9};
uint8_t Algorithm_Reset_Command[]={0xAA,0x00,0xFE,0xBB};
uint8_t Dec_Float_Output[]={0xAA,0x00,0xFB};

//MMDxx初始化，包括串口初始化和将数据上传模式修改为 查询模式
void MMDxx_Init(u8 sensor_num,u32 baud){
	uint8_t dummy_buff[4]={0};
	uint8_t i=0;
	char UART_Info[]="UART4:HW\r\n";
	
	for(i=0;i<7;++i)
		GAS_Instance[i] = Sensor_Instance_Init(ch4+i,100);
  Sensor3V3_Power_init();
	MMDxx_GPIO_Init(4,baud);
	MMDxx_GPIO_Init(5,baud);
	// 重启一次传感器，确保开机为主动上传模式，然后再切换为查询模式
	Sensor3V3_Power=0;
	delay_ms(1000);
	Sensor3V3_Power=1;
	delay_ms(4000);
	
	MMDxx_Update_Mode_Change(5);
	MMDxx_Update_Mode_Change(4);
	MMDxx_Data_Query(1,5);
	MMDxx_Data_Query(1,4);
	
	if(MMDxx_Data_Query(1,4)==1){
		while(UART_Info[i++]!='\0'){
			USART_SendData(USART3,UART_Info[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
		}
		LORA_send_data(UART_Info,sizeof(UART_Info));
	}
  if(MMDxx_Data_Query(1,5)==1){
		UART_Info[4]='5';
		while(UART_Info[i++]!='\0'){
			USART_SendData(USART3,UART_Info[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
		}
		LORA_send_data(UART_Info,sizeof(UART_Info));
	}

}

//慧闻传感器 3.3v 供电引脚控制 旧版PC2/新版PD10--NMDxx VCC
void Sensor3V3_Power_init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC总线时钟

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //GPIOC2
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //开漏模式
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //无上下拉
//	
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOC总线时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //GPIOC2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //开漏模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //无上下拉
	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

//MMDxx 串口初始化函数，用户根据自己所使用平台对串口进行初始化
void MMDxx_GPIO_Init(u8 sensor_num,u32 baud){
	
		USART_TypeDef* USARTx;
		switch (sensor_num){
		case 1: USARTx = USART1;break;
		case 4: USARTx = UART4;break;
		case 5: USARTx = UART5;break;
		case 6: USARTx = USART6;break;
    default: USARTx = USART1;break;
	}
	uart_init(USARTx,baud);
	
}

//MMDxx 切换数据上传模式，传感器数据在模式0（每3秒自动上传一次数据）和模式1（通过指令查询数据）之间切换
void MMDxx_Update_Mode_Change(u8 sensor_num){

	uint8_t command[3]={0xAA,0x00,0xFA};
	MMDxx_Command_Send(sensor_num,command,3);
}

//MMDxx数据查询，返回五个字节的数组
Error_Code_t MMDxx_Data_Query(uint8_t len, ...){
	Error_Code_t error_code=0;
	uint8_t data_buff[5];
	uint8_t *read_buff, i = 0;
	uint8_t usart_num,sensor_id;
	uint32_t new_value =0,mean_value=0;
	uint8_t receive_buff[24];
	Data_Struct_T *instance = NULL; 
	
	if(len == 1){
		va_list valist;
		va_start(valist,len);
		usart_num =(unsigned char)va_arg(valist,int); 
		
		MMDxx_Command_Send(usart_num,Data_Query_Command,sizeof(Data_Query_Command));
		delay_ms(500);
		if(MMDxx_Data_Receive(usart_num,receive_buff)){
			error_code=1;
		}
		va_end(valist);
		return error_code;
	}	
	else if(len == 3){
		va_list valist;
		va_start(valist,len);
		usart_num  = (unsigned char)va_arg(valist,int);
		sensor_id = (unsigned char)va_arg(valist,int);
		read_buff = (unsigned char *)va_arg(valist,int);
		while((GAS_Instance[i]->sensor_id == sensor_id) &&(sensor_id < 8))
			i++;
		if(sensor_id > 7)
			error_code =0;
		else{
			instance = GAS_Instance[i];
			MMDxx_Command_Send(usart_num,Data_Query_Command,sizeof(Data_Query_Command));
			delay_ms(500);
			if(MMDxx_Data_Receive(usart_num,receive_buff)){
				new_value = MMDxx_Data_Decode(data_buff,receive_buff);
				mean_value = SensorData_Filter(instance, new_value);
				error_code=1;
			}
			else
			{
				mean_value = SensorData_Filter(instance, instance->pre_value);
				error_code=0;
			}
		}
		for(i=0;i<4;++i)
			read_buff[i] = (uint8_t)(mean_value>>((3-i)*8));
		va_end(valist);
	}	
	else
		error_code = 0;
		
	return error_code;
}

//MMDxx指令发送函数，用户填入自己的串口发送函数 
void MMDxx_Command_Send(uint8_t sensor_num,uint8_t *command,uint8_t length){
		USART_TypeDef* USARTx;
		switch (sensor_num){
		case 1: USARTx = USART1;break;
		case 4: USARTx = UART4;break;
		case 5: USARTx = UART5;break;
		case 6: USARTx = USART6;break;
    default: USARTx = USART1;break;
	}
	memset(&USART_RX_BUF[sensor_num-1][0],0,USART_REC_LEN);
	for(uint8_t i=0;i<length;++i){
			USART_SendData(USARTx,command[i]);
			while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//等待发送结束
		  USART_ClearFlag(USARTx, USART_FLAG_TC);             //软件清除发送完成标志位
	 }

}

//MMDxx数据解析--将接收数据解析为温度、电压、气体类型、浓度等数据
uint32_t MMDxx_Data_Decode(uint8_t *result_point,uint8_t *source_point){
	//	float temperature,voltage1,voltage2,voltage3,gas_concentration;
		uint8_t i;
		uint32_t temp,vol1,vol2,vol3,gas;
		for(i=0;i<4;++i){
			temp|=(uint32_t)source_point[i]<<(8*i);
			vol1|=(uint32_t)source_point[i+4]<<(8*i);
			vol2|=(uint32_t)source_point[i+8]<<(8*i);
			vol3|=(uint32_t)source_point[i+12]<<(8*i);
			gas|=(uint32_t)source_point[i+17]<<(8*i);
		}
		for(i=20;i>16;--i)
			*(result_point++)=source_point[i];
		return gas;
}


//MMDxx 串口接收数据  指定传感器编号sensor_num 和数据存放起始地址data_buff(空间大于22bytes)
_Bool MMDxx_Data_Receive(uint8_t sensor_num,uint8_t *data_buff){  
	_Bool error_code=0;
	
	uint8_t Read_buffer[24]={0};
	if(USART_RX_CNT[sensor_num-1]>=24){
	memcpy(Read_buffer,&USART_RX_BUF[sensor_num-1],24);
	if((Read_buffer[0]==0xAA)&&(MMDxx_getCheckSum(Read_buffer,23)==Read_buffer[23])){
		for(int i=1;i<23;++i){
			 *(data_buff++) = Read_buffer[i];	
			}
		error_code=1;
		}
	}
	USART_RX_CNT[sensor_num-1]=0;
	memset(USART_RX_BUF[sensor_num-1],0,USART_REC_LEN);
	return error_code;
}

//数据校验值计算
uint8_t  MMDxx_getCheckSum(uint8_t *packet,uint8_t length){
	
	uint8_t i;
	uint32_t checksum=0;
	for(i=0;i<length;++i){
		checksum +=packet[i];
	}
	checksum &=0xff;
	return checksum;
	
}





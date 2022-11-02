#include "mh4xx.h"
#include <stdarg.h>
#include <string.h>
#include "delay.h"
#include "LORA.h"

/***厂家发货传感器精度（读取值*精度=实际值）****/
/*  CH4--0.01%VOL  NH3 --0.01PPM
    CO --1PPM      SO2 --0.1PPM
		H2S--0.01PPM   NO2 --0.1PPM
		CO2--1PPM                   */ 
		

/******MH4xx系列传感器接口函数******/
//
void MH4xx_ZE_Init_All(u32 baud){
	uint8_t mh_sensor_connect=0;
	uart_init(UART4,baud);
	uart_init(UART5,baud);
	if (device_type_num==0){
		uart_init(USART1,baud);
		delay_ms(1000);
		MH4xx_ZE_Init(1,baud);
	}
	else if(device_type_num==1){
		uart_init(USART1,baud);
		uart_init(USART6,baud);
		delay_ms(1000);
		MH4xx_ZE_Init(1,baud);
	  MH4xx_ZE_Init(6,baud);
	}

	else if(device_type_num==4){
		uart_init(USART6,baud);
		delay_ms(1000);
		MH4xx_ZE_Init(6,baud);
	}
	else
		delay_ms(1000);
	MH4xx_ZE_Init(4,baud);
  MH4xx_ZE_Init(5,baud);

}
//MH4xx 读取气体浓度值
Error_Code_t MH4xx_ZE_Init(uint8_t  gas,u32 baud){
	Error_Code_t Error=1;
	uint8_t dummy[2];
	uint8_t i=0;
	char init_info[]="UART1 Connect WS\r\n";
	delay_ms(1000);
	if(device_type_num){
		if((gas==1)||(gas==4)||(gas==5))
			ZExx_State_Change(gas,0x04);
	}
	else{
		if((gas==4)||(gas==5))
			ZExx_State_Change(gas,0x04);
	}
	delay_ms(100);
	//Error=MH4xx_Write_Read(gas,0x86,&dummy);
	Error=MH4xx_ZE_Get_Value(gas,dummy);
	if(Error)
	{
		sprintf(init_info,"UART%d Connect WS\r\n",gas);
			while(init_info[i++]!='\0'){
			USART_SendData(USART3,init_info[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
		}
		LORA_send_data(init_info,sizeof(init_info));
	}	
	return Error;
}
//
void ZExx_State_Change(uint8_t sensor_num,uint8_t update_mode){
//	USART_TypeDef* USARTx;
	uint8_t newmode=update_mode;

	delay_ms(500);
	MH4xx_Write_Read(sensor_num,0x78,newmode);
	delay_ms(200);
	
	memset(USART_RX_BUF[sensor_num-1],0,USART_REC_LEN);
	USART_RX_CNT[sensor_num-1]=0;
}
//MH4xx 读取气体浓度值
Error_Code_t MH4xx_ZE_Get_Value(uint8_t sensor_num,uint8_t *data){
	uint8_t *read_buff;
	read_buff=data;
	Error_Code_t Error=0;
	uint8_t sensor_data[6];
	uint8_t count=3;
	while((!(Error=MH4xx_Write_Read(sensor_num,0x86,sensor_data)))&&((count--)>0));
	//CHECK_ERROR(Error);
	if(Error){
		read_buff[0]=sensor_data[0];
		read_buff[1]=sensor_data[1];
	}
	else
	{
		read_buff[0]=0xFF;
		read_buff[1]=0xFF;
	}
	return Error;
}

//MH4xx校准
Error_Code_t MH4xx_Calibrate(uint8_t sensor_num,uint16_t span_value){
	Error_Code_t Error=0;
	Error=MH4xx_Calibrate_Zero(sensor_num);
	delay_ms(5000);
	Error=MH4xx_Calibrate_Span(sensor_num,span_value);
	return Error;
}

//MH4xx串口发送接收数据
Error_Code_t MH4xx_Write_Read(uint8_t sensor_num,uint8_t command,...){
	Error_Code_t Error=0;
	
	USART_TypeDef* USARTx;
	uint8_t *data,span_value;
	uint8_t i,Send_buffer[9]={0xff,0x01,},Read_buffer[9];
	va_list argptr;
	va_start(argptr,command);
	
	switch (sensor_num){

		
		case CH4: USARTx = UART4;break;
		case NH3: USARTx = UART5;break;
		case CO2: USARTx = USART6;break;
    default: Error = 0;
	}
	//CHECK_ERROR(Error);	
	
	Send_buffer[2] = command;
	//根据可变参数个数判断buffer[3~4]是否写入
	if(command==0x88){
		span_value=va_arg(argptr,uint16_t);
		Send_buffer[3] = (uint8_t)(span_value>>8);
		Send_buffer[4] = (uint8_t)(span_value);
	}
	else if(command==0x86)
		 data=va_arg(argptr,uint8_t *);
	else if(command==0x78||command==0xA7){
		Send_buffer[3] = va_arg(argptr,uint8_t);
		data=va_arg(argptr,uint8_t *);
	}
	else ;
	va_end(argptr);
	
	 memset(USART_RX_BUF[sensor_num-1],0,USART_REC_LEN);
	 USART_RX_CNT[sensor_num-1]=0;
	
	 Send_buffer[8]=getCheckSum(&Send_buffer[0]);
	 for(i=0;i<9;++i){
			USART_SendData(USARTx,Send_buffer[i]);
			while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//等待发送结束
		  USART_ClearFlag(USARTx, USART_FLAG_TC);             //软件清除发送完成标志位
	 }
	 delay_ms(30);
	 
	 //运用switch case：来判断对应串口接收的数据量？？？
	 
	 if(USART_RX_CNT[sensor_num-1]>=9){
		 memcpy(Read_buffer,&USART_RX_BUF[sensor_num-1],9);
		 if((Read_buffer[0]==0xFF)&&(Read_buffer[1]==0x86||Read_buffer[1]==0x78||Read_buffer[1]==0xA7)&&(getCheckSum(Read_buffer)==Read_buffer[8])){
				for(int i=2;i<8;++i){
					 *(data++) = Read_buffer[i];	
					 Error=1;
				}
		 }
 
	}
	 memset(USART_RX_BUF[sensor_num-1],0,USART_REC_LEN);
	 USART_RX_CNT[sensor_num-1]=0;
	 return Error;
}

//MH4xx校准零点
Error_Code_t 	MH4xx_Calibrate_Zero(uint8_t sensor_num){
	Error_Code_t error=0;
	error=MH4xx_Write_Read(sensor_num,0x87);
	return error;
}

//MH4xx校准跨度值
Error_Code_t 	MH4xx_Calibrate_Span(uint8_t sensor_num,uint16_t span_value){
	Error_Code_t error=0;
	error=MH4xx_Write_Read(sensor_num,0x88,span_value);
	return error;
}
/********End*******************/


/******ZE03系列传感器接口******/
Error_Code_t ZExx_Get_Value(uint8_t sensor_num,uint16_t *data){
	uint16_t gas_concentration;
	Error_Code_t Error=0;
	Error=MH4xx_Write_Read(sensor_num,0x86,&gas_concentration);
	*data=gas_concentration*resolution;
	return Error;
}

Error_Code_t ZE_ZH_Mode_Change(uint8_t sensor_num,uint8_t mode){
	uint16_t data;
	Error_Code_t Error=0;
	Error=MH4xx_Write_Read(sensor_num,0x78,mode,&data);
	Error=data>>8;
	return Error;
}


/********End*******************/

/******ZH06系列传感器接口******/
Error_Code_t ZHxx_Get_Value(uint8_t sensor_num,uint16_t *data){
	uint16_t gas_concentration[3];
	Error_Code_t Error=0;
	Error=MH4xx_Write_Read(sensor_num,0x86,gas_concentration);
	data=gas_concentration;
	return Error;
}

Error_Code_t ZH_Dormant_Change(uint8_t sensor_num,uint8_t mode){
	uint16_t data;
	Error_Code_t Error=0;
	Error=MH4xx_Write_Read(sensor_num,0xA7,mode,&data);
	Error=data>>8;
	return Error;
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

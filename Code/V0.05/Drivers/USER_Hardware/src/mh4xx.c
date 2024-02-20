#include "mh4xx.h"
#include "usart.h"
#include <stdarg.h>
#include <string.h>
#include "delay.h"
#include "cmsis_os.h"

GAS_IO_INFO COIOInfo, NO2IOInfo;
float CO_Data,NO2_Data;
/***厂家发货传感器精度（读取值*精度=实际值）****/
/*  CH4--0.01%VOL  NH3 --0.01PPM
    CO --1PPM      SO2 --0.1PPM
		H2S--0.01PPM   NO2 --0.1PPM
		CO--1PPM                   */ 
		

/******MH4xx系列传感器接口函数******/
//
void MH4xx_ZE_Init_All(){
	GAS_IO_Init();
	GAS_Seneor_State_Change(NO2,0x04);
  if(Gas_Get_Value(CO)!= -1)
		UsartPrintf(USART_DEBUG,"CO Sensor Init Success",sizeof("CO Sensor Init Success"));
	if(Gas_Get_Value(NO2) != -1)
		UsartPrintf(USART_DEBUG,"NO2 Sensor Init Success",sizeof("NO2 Sensor Init Success"));;
}

void Gas_GetValue_ALL(void)
{
	CO_Data = (float)Gas_Get_Value(CO);
	NO2_Data = (float)Gas_Get_Value(NO2);
}

void GAS_Seneor_State_Change(uint8_t sensor_num,uint8_t update_mode){
	uint8_t newmode=update_mode;

	osDelay(500);
	MH4xx_Write_Read(sensor_num,0x78,newmode);
	osDelay(300);

}
//MH4xx 读取气体浓度值
int Gas_Get_Value(enum Gas_Type gas){
	
	_Bool Error=0;
	uint8_t sensor_data[6];
	uint8_t count=10;
  uint8_t i = 0;
		
	do
	{
		if(MH4xx_Write_Read(gas, 0x86, sensor_data) == 1)
		{
				return ((uint16_t)sensor_data[0]<<8 | sensor_data[1]);	
		}
		else
			osDelay(50);
	} while(count--);
	
	return -1;
}

//MH4xx校准
_Bool MH4xx_Calibrate(uint8_t sensor_num,uint16_t span_value){
	_Bool Error=0;
	Error=MH4xx_Calibrate_Zero(sensor_num);
	osDelay(5000);
	Error=MH4xx_Calibrate_Span(sensor_num,span_value);
	return Error;
}

//MH4xx串口发送接收数据
_Bool MH4xx_Write_Read(enum Gas_Type gas,uint8_t command,...)
{
	_Bool Error=0;
	
	uint8_t *data,span_value;
	uint8_t i,Send_buffer[9]={0xff,0x01,},Read_buffer[9];
	va_list argptr;
	va_start(argptr,command);
	
	
	Send_buffer[2] = command;
	//根据可变参数个数判断buffer[3~4]是否写入
	if(command==0x88){
		span_value=(unsigned short)va_arg(argptr,int);
		Send_buffer[3] = (uint8_t)(span_value>>8);
		Send_buffer[4] = (uint8_t)(span_value);
	}
	else if(command==0x86)
		 data=va_arg(argptr,uint8_t *);
	else if(command==0x78||command==0xA7){
		Send_buffer[3] = (unsigned char)va_arg(argptr,int);
		data=va_arg(argptr,uint8_t *);
	}
	else ;
	va_end(argptr);
	

	
	 Send_buffer[8]=getCheckSum(&Send_buffer[0]);
	 GAS_IO_ClearRecive(gas);
	 GAS_IO_Send(gas,Send_buffer,sizeof(Send_buffer));
	
	
	 osDelay(200);
	 uint8_t time_out = 10;
	 do{
			if(GAS_IO_WaitRecive(gas) == 1)
			{
				switch(gas)
				{
					case CO:
						if(COIOInfo.dataLen>=9){
							memcpy(Read_buffer,COIOInfo.buf,9);
							if((Read_buffer[0]==0xFF)&&(Read_buffer[1]==0x86||Read_buffer[1]==0x78||Read_buffer[1]==0xA7)&&(getCheckSum(Read_buffer)==Read_buffer[8]))
							{
								for(int i=2;i<8;++i)
								{
									*(data++) = Read_buffer[i];	
									Error=1;
								}
							}
						}
						break;
					case NO2:
					 if(NO2IOInfo.dataLen>=9){
							memcpy(Read_buffer,NO2IOInfo.buf,9);
							if((Read_buffer[0]==0xFF)&&(Read_buffer[1]==0x86||Read_buffer[1]==0x78||Read_buffer[1]==0xA7)&&(getCheckSum(Read_buffer)==Read_buffer[8]))
							{
								for(int i=2;i<8;++i)
								{
									*(data++) = Read_buffer[i];	
									Error=1;
								}
							}
						}
						break;
					default:
						break;
				}
			}
			else 
				osDelay(200);
	}while((time_out--)&&(!Error));
	GAS_IO_ClearRecive(gas);
	return Error;
}

//MH4xx校准零点
_Bool 	MH4xx_Calibrate_Zero(uint8_t sensor_num){
	_Bool error=0;
	error=MH4xx_Write_Read(sensor_num,0x87);
	return error;
}

//MH4xx校准跨度值
_Bool 	MH4xx_Calibrate_Span(uint8_t sensor_num,uint16_t span_value){
	_Bool error=0;
	error=MH4xx_Write_Read(sensor_num,0x88,span_value);
	return error;
}
/********End*******************/


/******ZE03系列传感器接口******/
_Bool ZExx_Get_Value(uint8_t sensor_num,uint16_t *data){
	uint16_t gas_concentration;
	_Bool Error=0;
	Error=MH4xx_Write_Read(sensor_num,0x86,&gas_concentration);
	*data=gas_concentration*resolution;
	return Error;
}

_Bool ZE_ZH_Mode_Change(uint8_t sensor_num,uint8_t mode){
	uint16_t data;
	_Bool Error=0;
	Error=MH4xx_Write_Read(sensor_num,0x78,mode,&data);
	Error=data>>8;
	return Error;
}


/********End*******************/

/******ZH06系列传感器接口******/
_Bool ZHxx_Get_Value(uint8_t sensor_num,uint16_t *data){
	uint16_t gas_concentration[3];
	_Bool Error=0;
	Error=MH4xx_Write_Read(sensor_num,0x86,gas_concentration);
	data=gas_concentration;
	return Error;
}

_Bool ZH_Dormant_Change(uint8_t sensor_num,uint8_t mode){
	uint16_t data;
	_Bool Error=0;
	Error=MH4xx_Write_Read(sensor_num,0xA7,mode,&data);
	Error=data>>8;
	return Error;
}


/********End*******************/

void GAS_IO_Init(void)
{
	MX_UART4_UART_Init();	
	HAL_UARTEx_ReceiveToIdle_IT(&huart4,COIOInfo.buf,100);
	GAS_IO_ClearRecive(CO);
	
	MX_UART5_UART_Init();	
	HAL_UARTEx_ReceiveToIdle_IT(&huart5,NO2IOInfo.buf,100);
	GAS_IO_ClearRecive(NO2);
}

//==========================================================
//	函数名称：	NET_IO_Send
//
//	函数功能：	发送数据
//
//	入口参数：	str：需要发送的数据
//				len：数据长度
//
//	返回参数：	无
//
//	说明：		底层的数据发送驱动
//
//==========================================================
void GAS_IO_Send(enum Gas_Type gas,unsigned char *str, unsigned short len)
{
	switch(gas)
	{
		case CO:
			HAL_UART_Transmit(&huart4,str,len,0xFFFF);
			break;
		case NO2:
			HAL_UART_Transmit(&huart5,str,len,0xFFFF);
			break;
		default:
			break;
	}
}

//==========================================================
//	函数名称：	NET_IO_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	1-接收完成		0-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool GAS_IO_WaitRecive(enum Gas_Type gas)
{
	switch(gas)
	{
		case CO:
			if(COIOInfo.dataLen == 0)
				return 0;
			else
				return 1;
			break;
		case NO2:
			if(NO2IOInfo.dataLen == 0)
				return 0;
			else
				return 1;
			break;
		default:
			break;
	}
}

//==========================================================
//	函数名称：	NET_IO_ClearRecive
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void GAS_IO_ClearRecive(enum Gas_Type gas)
{
	switch(gas)
	{
		case CO:
			COIOInfo.dataLen = 0;
			memset(COIOInfo.buf, 0, sizeof(COIOInfo.buf));
			break;
		case NO2:
			NO2IOInfo.dataLen = 0;
			memset(NO2IOInfo.buf, 0, sizeof(NO2IOInfo.buf));
			break;
		default:
			break;
		
	}
}



void GAS_UARTEx_RxEventCallback(enum Gas_Type gas,unsigned short Size)
{
		switch(gas)
		{
			case CO:
				COIOInfo.dataLen = Size;
				HAL_UARTEx_ReceiveToIdle_IT(&huart4,COIOInfo.buf,100);
				break;
			case NO2:
				NO2IOInfo.dataLen = Size;
				HAL_UARTEx_ReceiveToIdle_IT(&huart5,NO2IOInfo.buf,100);
				break;
		}
}

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

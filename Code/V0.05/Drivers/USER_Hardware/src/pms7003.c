#include "pms7003.h"
#include "usart.h"
#include "string.h"
#include "delay.h"
#include "cmsis_os.h"
#include "gpio.h"
//#include "filter.h"

//static Data_Struct_T *PM25_Instance = NULL;
//static Data_Struct_T *PM10_Instance = NULL;
PMS_IO_INFO pmsIOInfo;
uint16_t PM25_Data=0,PM10_Data=0;
static uint8_t PMS_CMD_Buff[7]={0x42,0x4d};
static uint8_t PMS_Receive_Buff[32];

void  PMS_init(void){
	uint8_t dummy_buff[4];
	char init_info[]="UART6:PMS7003\r\n";

//	PM25_Instance = Sensor_Instance_Init(pm25,50);
//	PM10_Instance = Sensor_Instance_Init(pm10,50);
	
	PMS_IO_Init();
	
	osDelay(2000);
	PMS_State_Change(passive);
	if(PMS_Data_Read(dummy_buff)==1){
		UsartPrintf(USART_DEBUG,init_info,sizeof(init_info));
	}	

}
/* 初始化PSM_7003 IO口
Reset -- PC2    0-- reset
Set   -- PC1  : 1 -- Nomal  0--Sleep

*/

void PMS_GPIO_Config(void){


}

void PMS_State_Change(enum update_state newstate){

	PMS_Send_CMD(State_Chanage,newstate);
	DelayMs(100);
	PMS_Receive_Data();

}

void PMS_standby_Change(enum standby_mode newmode){

	PMS_Send_CMD(State_Chanage,newmode);
	DelayMs(100);
	PMS_Receive_Data();
}

_Bool PMS_Data_Read(uint8_t *data){
	uint8_t timeout =5;

	PMS_Send_CMD(Data_Read,0);
	while(timeout--)
	{
		if(PMS_IO_WaitRecive() ==1 )
		{
			if(PMS_Receive_Data() == 1)
			{
				PM25_Data = (uint16_t)PMS_Receive_Buff[6]<<8 | PMS_Receive_Buff[7];
				PM10_Data = (uint16_t)PMS_Receive_Buff[8]<<8 | PMS_Receive_Buff[9];
				return 1;
			}
		}
		else
			osDelay(100);
	}
	return 0;
}


void PMS_Send_CMD(enum PSM_CMD cmd,uint8_t state_bytes){
	PMS_CMD_Buff[2]=cmd;
	PMS_CMD_Buff[4]=state_bytes;
	uint16_t check_sum=PMS_LRC_Check(PMS_CMD_Buff,5);
	PMS_CMD_Buff[5]=(uint8_t)(check_sum>>8);
	PMS_CMD_Buff[6]=(uint8_t)check_sum;

	uint8_t length=7;
	PMS_IO_ClearRecive();
	PMS_IO_Send(PMS_CMD_Buff,length);
}

_Bool PMS_Receive_Data(void){
	_Bool Error=0;
	uint8_t Read_buffer[60];
	if(pmsIOInfo.dataLen>=7){
		 uint8_t length = pmsIOInfo.dataLen;
		 memcpy(Read_buffer,pmsIOInfo.buf,pmsIOInfo.dataLen);
		 if((Read_buffer[0]==0x42)&&(Read_buffer[1]==0x4D)&&(PMS_LRC_Check(Read_buffer,length-2)==((uint16_t)(Read_buffer[length-2]<<8)|(Read_buffer[length-1])))){
				for(int i=2;i<length;++i){
						PMS_Receive_Buff[i] = Read_buffer[i];	
					  Error=1;
				}
		 }
 
	}
	PMS_IO_ClearRecive();
	return Error;
}

	
uint16_t PMS_LRC_Check(uint8_t *data_buffer,uint8_t length){
	uint16_t  sum=0;
	for(int i=0;i<length;++i)
		sum+= data_buffer[i];
	return sum;
}

void PMS_IO_Init(void)
{
	PWR5V_GPIO_Init();
	MX_USART6_UART_Init();	
	HAL_UARTEx_ReceiveToIdle_IT(&huart6,pmsIOInfo.buf,100);
	PMS_IO_ClearRecive();
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
void PMS_IO_Send(unsigned char *str, unsigned short len)
{

	HAL_UART_Transmit(&huart6,str,len,0xFFFF);

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
_Bool PMS_IO_WaitRecive(void)
{

	if(pmsIOInfo.dataLen == 0) 						//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return 0;
	else
		return 1;								//返回接收完成标志
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
void PMS_IO_ClearRecive(void)
{

	pmsIOInfo.dataLen = 0;
	
	memset(pmsIOInfo.buf, 0, sizeof(pmsIOInfo.buf));
	
}

void PMS_UARTEx_RxEventCallback(unsigned short Size)
{
		pmsIOInfo.dataLen = Size;
	
		//HAL_UART_Transmit(&huart1,netIOInfo.buf,netIOInfo.dataLen,0xff);
		HAL_UARTEx_ReceiveToIdle_IT(&huart6,pmsIOInfo.buf,100);
//		HAL_UART_Transmit(&huart1,Uart2_ReadCache,Size,0xff);
//		HAL_UARTEx_ReceiveToIdle_IT(&huart2,Uart2_ReadCache,400);

}

/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	net_IO.c
	*
	*	作者： 		CL
	*
	*	日期： 		2022-10-07
	*
	*	版本： 		V1.0
	*
	*	说明： 		网络设备数据IO层
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

#include "stm32f1xx.h"  //单片机头文件

#include "usart.h"

#include "net_io.h"		//网络设备数据IO

#include "dsl_08.h"

#include "mh4xx.h"

#include <string.h>		//C库




NET_IO_INFO netIOInfo;
uint8_t Uart2_ReadCache[10];
_Bool reconnect_flag = 0; 
_Bool ChOcp_flag = 0;




//==========================================================
//	函数名称：	NET_IO_Init
//
//	函数功能：	初始化网络设备IO驱动层
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		底层的数据收发驱动
//==========================================================
void NET_IO_Init(void)
{
	MX_USART1_UART_Init();	
	HAL_UART_Receive_IT(&huart1,Uart2_ReadCache,1);
//	HAL_UARTEx_ReceiveToIdle_IT(&huart1,netIOInfo.buf,1);
	NET_IO_ClearRecive();
	ChOcp_flag = 0;

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
void NET_IO_Send(unsigned char *str, unsigned short len)
{

//	unsigned short count = 0;
	
	HAL_UART_Transmit(&huart1,str,len,0xFFFF);
//	for(; count < len; count++)											//发送一帧数据
//	{
//		USART_SendData(NET_IO, *str++);
//		while(USART_GetFlagStatus(NET_IO, USART_FLAG_TC) == RESET);
//	}

}

//==========================================================
//	函数名称：	NET_IO_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool NET_IO_WaitRecive(void)
{

//	if(netIOInfo.dataLen == 0) 						//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
//		return REV_WAIT;
//		
//	{
//		return REV_OK;								//返回接收完成标志
//	}
//	
	if(netIOInfo.dataLen == 0) 						//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(netIOInfo.dataLen == netIOInfo.dataLenPre)	//如果上一次的值和这次相同，则说明接收完毕
	{
		//netIOInfo.dataLen = 0;						//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	netIOInfo.dataLenPre = netIOInfo.dataLen;		//置为相同

	return REV_WAIT;								//返回接收未完成标志

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
void NET_IO_ClearRecive(void)
{

	netIOInfo.dataLen = 0;
	netIOInfo.dataLenPre = 0; 
	
	memset(netIOInfo.buf, 0, sizeof(netIOInfo.buf));
	
}

//==========================================================
//	函数名称：	USART2_IRQHandler
//
//	函数功能：	接收中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
//void USART2_IRQHandler(void)
//{
//	
//	RTOS_EnterInt();

////	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
////	{
////		if(netIOInfo.dataLen >= sizeof(netIOInfo.buf))	netIOInfo.dataLen = 0; //防止串口被刷爆
////		netIOInfo.buf[netIOInfo.dataLen++] = USART2->DR;
////		
////		USART_ClearFlag(USART2, USART_FLAG_RXNE);
////	}
//	
//	RTOS_ExitInt();

//}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	//	RTOS_EnterInt();
	if(huart == &huart1){
		  ChOcp_flag = 1;
			if(netIOInfo.dataLen >= sizeof(netIOInfo.buf))	netIOInfo.dataLen = 0; //防止串口被刷爆
			netIOInfo.buf[netIOInfo.dataLen++] = USART1->DR;
			HAL_UART_Receive_IT(&huart1,Uart2_ReadCache,1);	
	}

	
	//RTOS_ExitInt();
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){

	if(huart->Instance == USART2)
	{
////			netIOInfo.dataLen = Size;
////			HAL_UARTEx_ReceiveToIdle_IT(&huart1,netIOInfo.buf,320);
//		
//		netIOInfo.dataLen += Size;
//		if(netIOInfo.dataLen >= sizeof(netIOInfo.buf))	netIOInfo.dataLen = 0;
//		HAL_UARTEx_ReceiveToIdle_IT(&huart1,&netIOInfo.buf[netIOInfo.dataLen],1);
		
	}
	else	if(huart->Instance == USART3)
	{
		PM_UARTEx_RxEventCallback(Size);
	}
 }

/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	net_IO.c
	*
	*	���ߣ� 		CL
	*
	*	���ڣ� 		2022-10-07
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		�����豸����IO��
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

#include "stm32f1xx.h"  //��Ƭ��ͷ�ļ�

#include "usart.h"

#include "net_io.h"		//�����豸����IO

#include "dsl_08.h"

#include "mh4xx.h"

#include <string.h>		//C��




NET_IO_INFO netIOInfo;
uint8_t Uart2_ReadCache[10];
_Bool reconnect_flag = 0; 
_Bool ChOcp_flag = 0;




//==========================================================
//	�������ƣ�	NET_IO_Init
//
//	�������ܣ�	��ʼ�������豸IO������
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		�ײ�������շ�����
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
//	�������ƣ�	NET_IO_Send
//
//	�������ܣ�	��������
//
//	��ڲ�����	str����Ҫ���͵�����
//				len�����ݳ���
//
//	���ز�����	��
//
//	˵����		�ײ�����ݷ�������
//
//==========================================================
void NET_IO_Send(unsigned char *str, unsigned short len)
{

//	unsigned short count = 0;
	
	HAL_UART_Transmit(&huart1,str,len,0xFFFF);
//	for(; count < len; count++)											//����һ֡����
//	{
//		USART_SendData(NET_IO, *str++);
//		while(USART_GetFlagStatus(NET_IO, USART_FLAG_TC) == RESET);
//	}

}

//==========================================================
//	�������ƣ�	NET_IO_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool NET_IO_WaitRecive(void)
{

//	if(netIOInfo.dataLen == 0) 						//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
//		return REV_WAIT;
//		
//	{
//		return REV_OK;								//���ؽ�����ɱ�־
//	}
//	
	if(netIOInfo.dataLen == 0) 						//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(netIOInfo.dataLen == netIOInfo.dataLenPre)	//�����һ�ε�ֵ�������ͬ����˵���������
	{
		//netIOInfo.dataLen = 0;						//��0���ռ���
			
		return REV_OK;								//���ؽ�����ɱ�־
	}
		
	netIOInfo.dataLenPre = netIOInfo.dataLen;		//��Ϊ��ͬ

	return REV_WAIT;								//���ؽ���δ��ɱ�־

}

//==========================================================
//	�������ƣ�	NET_IO_ClearRecive
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void NET_IO_ClearRecive(void)
{

	netIOInfo.dataLen = 0;
	netIOInfo.dataLenPre = 0; 
	
	memset(netIOInfo.buf, 0, sizeof(netIOInfo.buf));
	
}

//==========================================================
//	�������ƣ�	USART2_IRQHandler
//
//	�������ܣ�	�����ж�
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
//void USART2_IRQHandler(void)
//{
//	
//	RTOS_EnterInt();

////	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
////	{
////		if(netIOInfo.dataLen >= sizeof(netIOInfo.buf))	netIOInfo.dataLen = 0; //��ֹ���ڱ�ˢ��
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
			if(netIOInfo.dataLen >= sizeof(netIOInfo.buf))	netIOInfo.dataLen = 0; //��ֹ���ڱ�ˢ��
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

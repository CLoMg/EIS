/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	fault.c
	*
	*	作者： 		CL
	*
	*	日期： 		2022-10-07
	*
	*	版本： 		V1.0
	*
	*	说明： 		网络状态错误处理
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

#include "fault.h"
#include "selfcheck.h"
#include "usart.h"
#include "onenet.h"

#include "net_device.h"





unsigned char faultType = FAULT_NONE;			//初始为无错误类型
unsigned char faultTypeReport = FAULT_NONE;		//报告错误类型用的



/*
************************************************************
*	函数名称：	Fault_Process
*
*	函数功能：	网络状态错误处理
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		针对死机重启、协议错误、设备错误的处理
************************************************************
*/
void Fault_Process(void)
{

	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);						//设置为命令收发模式
	
	switch(faultType)
	{
		case FAULT_NONE:
			
			UsartPrintf(USART_DEBUG, "WARN:	FAULT_NONE\r\n");
		
		break;
		
		case FAULT_REBOOT: 											//死机重启的处理
			
			UsartPrintf(USART_DEBUG, "WARN:	FAULT_REBOOT\r\n");
		
			faultType = FAULT_NONE;									//死机重启后，设备会被复位，所以是重新初始化
		
		break;
		
		case FAULT_EDP: //协议出错的处理
			
			UsartPrintf(USART_DEBUG, "WARN:	FAULT_MQTT\r\n");
		
			faultType = FAULT_NONE;
		
			oneNetInfo.netWork = 0; 								//这里设置重连平台
			
			NET_DEVICE_ReLink(oneNetInfo.ip, oneNetInfo.port);		//连接IP
		
		break;
		
		case FAULT_NODEVICE: 										//设备出错的处理
			
			UsartPrintf(USART_DEBUG, "WARN:	FAULT_NODEVICE\r\n");
		
			faultType = FAULT_NONE;		

			checkInfo.NET_DEVICE_OK = DEV_ERR;						//设备出错
			//NET_DEVICE_Reset();										//网络设备复位(注释于202210.13，这里应该只修改标志位，复位操作放到NET_Task里面)
			NET_DEVICE_ReConfig(0);									//重新初始化
			oneNetInfo.netWork = 0;									//网络未连接
		
		break;
		
		default:
			break;
	}
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);						//设置为数据收发模式

}

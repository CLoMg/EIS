/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	net_device.c
	*
	*	作者： 		CL
	*
	*	日期： 		2022-10-07
	*
	*	版本： 		V1.1
	*
	*	说明： 		网络设备应用层
	*
	*	修改记录：	V1.1：平台IP和PORT通过参数传入的方式确定，解决了不同协议网络设备驱动不通用的问题。
	************************************************************
	************************************************************
	************************************************************
**/

#include "stm32f1xx.h"	//单片机头文件

#include "net_device.h"	//网络设备应用层
#include "net_io.h"		//网络设备数据IO层

//硬件驱动

#include "usart.h"
#include "gpio.h"
#include "delay.h"
#include "cmsis_os.h"
#include "sysparams.h"

//C库
#include <string.h>
#include <stdlib.h>
#include <stdio.h>




NET_DEVICE_INFO netDeviceInfo = {0, 0, 1, 0, 0, 0};


//在开机下调用一次则关机，再关机下调用则开机
void NET_DEVICE_PowerCtl(void)
{
	
	//RTOS_TimeDly(200);
	//NET_DEVICE_PWRK_OFF;
   // RTOS_TimeDly(240);
 //   NET_DEVICE_PWRK_ON;
  //  RTOS_TimeDly(140);

}

//==========================================================
//	函数名称：	NET_DEVICE_IO_Init
//
//	函数功能：	初始化网络设备IO层
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		初始化网络设备的控制引脚、数据收发功能等
//==========================================================
void NET_DEVICE_IO_Init(void)
{
		NET_DEVICE_GPIO_Init();

	
//	if(netDeviceInfo.reboot == 0)
//	{

//		
//		NET_DEVICE_PERSTN_OFF;
//		DelayMs(100);
//		NET_DEVICE_PWR_ON;
//		DelayMs(1000);
//	}
//	else{
//		NET_DEVICE_PERSTN_OFF;
//		DelayMs(100);
//		NET_DEVICE_PWR_ON;
//		DelayMs(1000);
//	}
	netDeviceInfo.reboot = 0;
	
	NET_IO_Init();
	DelayMs(1000);
	NET_IO_Send((unsigned char *)mac_strings,sizeof(mac_strings));
}

//==========================================================
//	函数名称：	NET_DEVICE_Exist
//
//	函数功能：	网络设备存在检查
//
//	入口参数：	无
//
//	返回参数：	返回结果
//
//	说明：		0-成功		1-失败
//==========================================================
_Bool NET_DEVICE_Exist(void)
{

	return NET_DEVICE_SendCmd("AT\r\n", "OK", 1);

}

//==========================================================
//	函数名称：	NET_DEVICE_Init
//
//	函数功能：	网络设备初始化
//
//	入口参数：	无
//
//	返回参数：	返回初始化结果
//
//	说明：		0-成功		1-失败
//==========================================================
_Bool NET_DEVICE_Init(char *ip, char *port)
{
	
	char cfgBuffer[50];
	
	switch(netDeviceInfo.initStep)
	{
		
		case 0:
			
			UsartPrintf(USART_DEBUG, "AT\r\n");
			if(!NET_DEVICE_SendCmd("AT\r\n", "OK", 1))					//SIM卡是否解锁
				netDeviceInfo.initStep++;
			else
				break;
	  case 1:
			
			UsartPrintf(USART_DEBUG, "ATE0\r\n");
			if(!NET_DEVICE_SendCmd("ATE0\r\n", "OK", 1))					//SIM卡是否解锁
				netDeviceInfo.initStep++;
			else
				break;
		case 2:
			
			UsartPrintf(USART_DEBUG, "AT+CPIN?\r\n");
			if(!NET_DEVICE_SendCmd("AT+CPIN?\r\n", "OK", 1))					//SIM卡是否解锁
				netDeviceInfo.initStep++;
			else
				break;
			
		case 3:																	//自动判断卡类型
		{
			char resBuf[5] = {0, 0, 0, 0, 0};
			char text[2] = {0, 0};
			
			strcpy(resBuf, "0,");
			sprintf(text, "%d", netDeviceInfo.cardType);
			strcat(resBuf, text);
			
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CREG?  %d\r\n",
										netDeviceInfo.cardType);
			if(!NET_DEVICE_SendCmd("AT+CREG?\r\n", resBuf, 1)) 					//确认网络搜索成功,OK
				netDeviceInfo.initStep++;
			else 																//如果失败则检测返回的内容
			{
				if(netIOInfo.buf[11] != 48)
					netDeviceInfo.cardType = netIOInfo.buf[11] - 48;
				
				NET_DEVICE_ClrData();
				break;
			}
		}
	
			
		case 4:
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CSQ\r\n");
			if(!NET_DEVICE_SendCmd("AT+CSQ\r\n","OK", 1))						//查询信号强度,OK
				netDeviceInfo.initStep++;
			else
				break;
			
		case 5:
				UsartPrintf(USART_DEBUG, "STA Tips:	AT+CGREG?\r\n");			//检查网络注册状态
				if(!NET_DEVICE_SendCmd("AT+CGREG?\r\n","OK", 1))
					netDeviceInfo.initStep++;
				else
					break;
			
		case 6:
			
			UsartPrintf(USART_DEBUG, "AT+QIPCSGP=1,\"CMNET\"\r\n");
			if(!NET_DEVICE_SendCmd("AT+QICSGP=1,1,\"CMNET\",\"\",\"\",1\r\n", "OK", 1))		//设置为GPRS连接模式
				netDeviceInfo.initStep++;
			else
				break;
		case 7:
			
			UsartPrintf(USART_DEBUG, "AT+QIDEACT=1\r\n");
			if(!NET_DEVICE_SendCmd("AT+QIDEACT=1\r\n", "OK", 1))		//设置为GPRS连接模式
				netDeviceInfo.initStep++;
			else
				break;
		case 8:
			
			UsartPrintf(USART_DEBUG, "AT+QIACT=1\r\n");
			if(!NET_DEVICE_SendCmd("AT+QIACT=1\r\n", "OK", 1))		//设置为GPRS连接模式
				netDeviceInfo.initStep++;
			else
				break;
		case 9:
			
			memset(cfgBuffer, 0, sizeof(cfgBuffer));
			
			strcpy(cfgBuffer, "AT+QIOPEN=1,0,\"TCP\",\"");
			strcat(cfgBuffer, ip);
			strcat(cfgBuffer, "\",");
			strcat(cfgBuffer, port);
			strcat(cfgBuffer,",0,2");
			strcat(cfgBuffer, "\r\n");
			
			UsartPrintf(USART_DEBUG, "STA Tips:	%s", cfgBuffer);

			if(!NET_DEVICE_SendCmd(cfgBuffer, "CONNECT", 1)) 					//连接平台
				netDeviceInfo.initStep++;
			else
				break;
			
//		case 10:
//			
//			UsartPrintf(USART_DEBUG, "AT+CIPSTATUS\r\n");
//			if(!NET_DEVICE_SendCmd("AT+CIPSTATUS\r\n", "CONNECT OK", 1))		//连接平台
//				netDeviceInfo.initStep++;
//			else
//			{
//				NET_DEVICE_SendCmd("AT+CIPCLOSE=1\r\n", "CLOSE OK", 1);			//关闭连接
//				NET_DEVICE_SendCmd("AT+CIPSHUT\r\n", "SHUT OK", 1);				//关闭移动场景
//				RTOS_TimeDly(20);
//				NET_DEVICE_ReConfig(4);
//				break;
//			}
		
		
			
		default:
			break;
	}
    
	if(netDeviceInfo.initStep == 10)
		return 0;
	else
		return 1;

}

//==========================================================
//	函数名称：	NET_DEVICE_Reset
//
//	函数功能：	网络设备复位
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void NET_DEVICE_Reset(void)
{
		uint16_t cnt;
		cnt=6;
	do{
			
			NET_IO_ClearRecive();
			//GPIO_ResetBits(GPIOC,GPIO_Pin_1);
			NET_DEVICE_PERSTN_ON;
			osDelay(1000);
			//GPIO_SetBits(GPIOC,GPIO_Pin_1);
			NET_DEVICE_PERSTN_OFF;
			UsartPrintf(USART_DEBUG,"WAIT FOR READY",sizeof("WAIT FOR READY"));
			osDelay(1000);
			unsigned short timeOut = 20;
			while(timeOut != 0)
			{
				if(NET_IO_WaitRecive() == REV_OK)
				{
					UsartPrintf(USART_DEBUG,(char*)&netIOInfo.buf,netIOInfo.dataLen);
					if(strstr((const char *)netIOInfo.buf, "RDY") != NULL)
					//if(strstr((const char *)Uart2_ReadCache, "RDY") != NULL)
					{
							
							NET_IO_ClearRecive();
							UsartPrintf(USART_DEBUG,"EC20 Module is ready!\r\n",sizeof"EC20 Module is ready!\r\n");
							return;
					}
				}
				timeOut--;
				osDelay(500);
			}
		}while((cnt--)!=0);
		if(!cnt)
			UsartPrintf(USART_DEBUG,"EC20 Module isn't ready!\r\n",sizeof"EC20 Module isn't ready!\r\n");
}

//==========================================================
//	函数名称：	NET_DEVICE_ReLink
//
//	函数功能：	重连平台
//
//	入口参数：	无
//
//	返回参数：	返回连接结果
//
//	说明：		0-成功		1-失败
//==========================================================
_Bool NET_DEVICE_ReLink(char *ip, char *port)
{
	
	_Bool status = 0;
	char cfgBuffer[32];
	
	NET_DEVICE_SendCmd("AT+QICLOSE=0\r\n", "OK", 1); 	//关闭连接
	RTOS_TimeDly(20);
	
	memset(cfgBuffer, 0, sizeof(cfgBuffer));
			
	strcpy(cfgBuffer, "AT+QIOPEN=1,0,\"TCP\",\"");
	strcat(cfgBuffer, ip);
	strcat(cfgBuffer, "\",");
	strcat(cfgBuffer, port);
	strcat(cfgBuffer,",0,2");
	strcat(cfgBuffer, "\r\n");
	UsartPrintf(USART_DEBUG, "STA Tips:	%s", cfgBuffer);

	status = NET_DEVICE_SendCmd(cfgBuffer, "CONNECT OK", 1); 	//重连平台
	
	return status;

}

//==========================================================
//	函数名称：	NET_DEVICE_SendCmd
//
//	函数功能：	向网络设备发送一条命令，并等待正确的响应
//
//	入口参数：	cmd：需要发送的命令
//				res：需要检索的响应
//				mode：1-清除接收		0-不清除(能获取返回信息)
//
//	返回参数：	返回连接结果
//
//	说明：		0-成功		1-失败
//==========================================================
_Bool NET_DEVICE_SendCmd(char *cmd, char *res, _Bool mode)
{
	
	unsigned short timeOut = 300 ;
//	while(retry--){
		NET_IO_Send((unsigned char *)cmd, strlen((const char *)cmd));
		timeOut = 300;
		while(timeOut--)
		{
			if(NET_IO_WaitRecive() == REV_OK)
			{
				if(strstr((const char *)netIOInfo.buf, res) != NULL)
				{
					if(mode)
						NET_IO_ClearRecive();
					
					return 0;
				}
			}
			
			osDelay(20);
		}
		//osDelay(2);
//	}
	return 1;

}

//==========================================================
//	函数名称：	NET_DEVICE_SendData
//
//	函数功能：	使网络设备发送数据到平台
//
//	入口参数：	data：需要发送的数据
//				len：数据长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void NET_DEVICE_SendData(unsigned char *data, unsigned short len)
{
	
#if(NET_DEVICE_TRANS == 1)
	NET_IO_Send(data, len);  //发送设备连接请求数据
#else
	char cmdBuf[30];

	RTOS_TimeDly(10);
	
	NET_IO_ClearRecive();
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);
	//if(!NET_DEVICE_SendCmd(cmdBuf, ">", 1))			//收到‘>’时可以发送数据
	NET_DEVICE_SendCmd(cmdBuf, ">", 1);
	{
		NET_IO_Send(data, len);  //发送设备连接请求数据
	}
#endif

}

//==========================================================
//	函数名称：	NET_DEVICE_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//				如ESP8266的返回格式为	"+IPD,x:yyy"	x代表数据长度，yyy是数据内容
//==========================================================
unsigned char *NET_DEVICE_GetIPD(unsigned short timeOut)
{

#if(NET_DEVICE_TRANS == 0)
	unsigned char byte = 0, count = 0;
	char sByte[5];
	char *ptrIPD;
#endif
	
	do
	{
		if(NET_IO_WaitRecive() == REV_OK)
		{
			#if(NET_DEVICE_TRANS == 0)
			//UsartPrintf(USART_DEBUG, "\r\n%s\r\n", netIOInfo.buf);
			ptrIPD = strstr((char *)netIOInfo.buf, "IPD,");
			if(ptrIPD == NULL)
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strstr(ptrIPD, ",");ptrIPD++;
				
				while(*ptrIPD != ':')
				{
					sByte[count++] = *ptrIPD++;
				}
				byte = (unsigned char)atoi(sByte);
				
				ptrIPD++;
				for(count = 0; count < byte; count++)
				{
					netIOInfo.buf[count] = *ptrIPD++;
				}
				netDeviceInfo.ipdBytes = byte;
				return netIOInfo.buf;
			}
				#else
					netDeviceInfo.ipdBytes = netIOInfo.dataLen;
					return netIOInfo.buf;
				#endif
			}
		osDelay(300);
	} while(timeOut--);
	
	return NULL;

}

//==========================================================
//	函数名称：	NET_DEVICE_ClrData
//
//	函数功能：	清空网络设备数据接收缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void NET_DEVICE_ClrData(void)
{

	NET_IO_ClearRecive();

}

//==========================================================
//	函数名称：	NET_DEVICE_Check
//
//	函数功能：	检查网络设备连接状态
//
//	入口参数：	无
//
//	返回参数：	返回状态
//
//	说明：		
//==========================================================
unsigned char NET_DEVICE_Check(void)
{
	
	unsigned char status = 0;
	unsigned char timeOut = 200;

	NET_IO_ClearRecive();
	NET_IO_Send((unsigned char *)"AT+CIPSTATUS\r\n",  14);
	
	while(--timeOut)
	{
		if(NET_IO_WaitRecive() == REV_OK)
		{
			if(strstr((const char *)netIOInfo.buf, "TCP CLOSED"))			//连接关闭
			{
				status = 1;
				UsartPrintf(USART_DEBUG, "SIM800C TCP CLOSED\r\n");
			}
			
			break;
		}
		
		RTOS_TimeDly(2);
	}
	
	if(timeOut == 0)
	{
		status = 1;
		UsartPrintf(USART_DEBUG, "Check TimeOut\r\n");
	}
	
	return status;

}

//==========================================================
//	函数名称：	NET_DEVICE_ReConfig
//
//	函数功能：	设备网络设备初始化的步骤
//
//	入口参数：	步骤值
//
//	返回参数：	无
//
//	说明：		该函数设置的参数在网络设备初始化里边用到
//==========================================================
void NET_DEVICE_ReConfig(unsigned char step)
{

	netDeviceInfo.initStep = step;

}

//==========================================================
//	函数名称：	NET_DEVICE_Set_DataMode
//
//	函数功能：	设置设备数据收发模式
//
//	入口参数：	模式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void NET_DEVICE_Set_DataMode(unsigned char mode)
{

	netDeviceInfo.dataType = mode;

}

//==========================================================
//	函数名称：	NET_DEVICE_Get_DataMode
//
//	函数功能：	获取设备数据收发模式
//
//	入口参数：	无
//
//	返回参数：	模式
//
//	说明：		
//==========================================================
unsigned char NET_DEVICE_Get_DataMode(void)
{

	return netDeviceInfo.dataType;

}


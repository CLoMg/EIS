/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	net_device.c
	*
	*	���ߣ� 		CL
	*
	*	���ڣ� 		2022-10-07
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		�����豸Ӧ�ò�
	*
	*	�޸ļ�¼��	V1.1��ƽ̨IP��PORTͨ����������ķ�ʽȷ��������˲�ͬЭ�������豸������ͨ�õ����⡣
	************************************************************
	************************************************************
	************************************************************
**/

#include "stm32f1xx.h"	//��Ƭ��ͷ�ļ�

#include "net_device.h"	//�����豸Ӧ�ò�
#include "net_io.h"		//�����豸����IO��

//Ӳ������

#include "usart.h"
#include "gpio.h"
#include "delay.h"
#include "cmsis_os.h"
#include "sysparams.h"

//C��
#include <string.h>
#include <stdlib.h>
#include <stdio.h>




NET_DEVICE_INFO netDeviceInfo = {0, 0, 1, 0, 0, 0};


//�ڿ����µ���һ����ػ����ٹػ��µ����򿪻�
void NET_DEVICE_PowerCtl(void)
{
	
	//RTOS_TimeDly(200);
	//NET_DEVICE_PWRK_OFF;
   // RTOS_TimeDly(240);
 //   NET_DEVICE_PWRK_ON;
  //  RTOS_TimeDly(140);

}

//==========================================================
//	�������ƣ�	NET_DEVICE_IO_Init
//
//	�������ܣ�	��ʼ�������豸IO��
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		��ʼ�������豸�Ŀ������š������շ����ܵ�
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
//	�������ƣ�	NET_DEVICE_Exist
//
//	�������ܣ�	�����豸���ڼ��
//
//	��ڲ�����	��
//
//	���ز�����	���ؽ��
//
//	˵����		0-�ɹ�		1-ʧ��
//==========================================================
_Bool NET_DEVICE_Exist(void)
{

	return NET_DEVICE_SendCmd("AT\r\n", "OK", 1);

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Init
//
//	�������ܣ�	�����豸��ʼ��
//
//	��ڲ�����	��
//
//	���ز�����	���س�ʼ�����
//
//	˵����		0-�ɹ�		1-ʧ��
//==========================================================
_Bool NET_DEVICE_Init(char *ip, char *port)
{
	
	char cfgBuffer[50];
	
	switch(netDeviceInfo.initStep)
	{
		
		case 0:
			
			UsartPrintf(USART_DEBUG, "AT\r\n");
			if(!NET_DEVICE_SendCmd("AT\r\n", "OK", 1))					//SIM���Ƿ����
				netDeviceInfo.initStep++;
			else
				break;
	  case 1:
			
			UsartPrintf(USART_DEBUG, "ATE0\r\n");
			if(!NET_DEVICE_SendCmd("ATE0\r\n", "OK", 1))					//SIM���Ƿ����
				netDeviceInfo.initStep++;
			else
				break;
		case 2:
			
			UsartPrintf(USART_DEBUG, "AT+CPIN?\r\n");
			if(!NET_DEVICE_SendCmd("AT+CPIN?\r\n", "OK", 1))					//SIM���Ƿ����
				netDeviceInfo.initStep++;
			else
				break;
			
		case 3:																	//�Զ��жϿ�����
		{
			char resBuf[5] = {0, 0, 0, 0, 0};
			char text[2] = {0, 0};
			
			strcpy(resBuf, "0,");
			sprintf(text, "%d", netDeviceInfo.cardType);
			strcat(resBuf, text);
			
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CREG?  %d\r\n",
										netDeviceInfo.cardType);
			if(!NET_DEVICE_SendCmd("AT+CREG?\r\n", resBuf, 1)) 					//ȷ�����������ɹ�,OK
				netDeviceInfo.initStep++;
			else 																//���ʧ�����ⷵ�ص�����
			{
				if(netIOInfo.buf[11] != 48)
					netDeviceInfo.cardType = netIOInfo.buf[11] - 48;
				
				NET_DEVICE_ClrData();
				break;
			}
		}
	
			
		case 4:
			UsartPrintf(USART_DEBUG, "STA Tips:	AT+CSQ\r\n");
			if(!NET_DEVICE_SendCmd("AT+CSQ\r\n","OK", 1))						//��ѯ�ź�ǿ��,OK
				netDeviceInfo.initStep++;
			else
				break;
			
		case 5:
				UsartPrintf(USART_DEBUG, "STA Tips:	AT+CGREG?\r\n");			//�������ע��״̬
				if(!NET_DEVICE_SendCmd("AT+CGREG?\r\n","OK", 1))
					netDeviceInfo.initStep++;
				else
					break;
			
//		case 6:
//			
//			UsartPrintf(USART_DEBUG, "AT+CIPHEAD=1\r\n");
//			if(!NET_DEVICE_SendCmd("AT+CIPHEAD=1\r\n", "OK", 1))				//��ʾ����ͷ
//				netDeviceInfo.initStep++;
//			else
//				break;
//			
//		case 7:
//			
//			UsartPrintf(USART_DEBUG, "AT+CGCLASS=\"B\"\r\n");
//			if(!NET_DEVICE_SendCmd("AT+CGCLASS=\"B\"\r\n", "OK", 1))			//����GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ���
//				netDeviceInfo.initStep++;
//			else
//				break;
//			
//		case 8:
//			
//			UsartPrintf(USART_DEBUG, "AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");
//			if(!NET_DEVICE_SendCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", "OK", 1))//����PDP������,��������Э��,��������Ϣ
//				netDeviceInfo.initStep++;
//			else
//				break;
//			
//		case 9:
//			
//			UsartPrintf(USART_DEBUG, "AT+CGATT=1\r\n");
//			if(!NET_DEVICE_SendCmd("AT+CGATT=1\r\n", "OK", 1))					//����GPRSҵ��
//				netDeviceInfo.initStep++;
//			else
//				break;
			
		case 6:
			
			UsartPrintf(USART_DEBUG, "AT+QIPCSGP=1,\"CMNET\"\r\n");
			if(!NET_DEVICE_SendCmd("AT+QICSGP=1,1,\"CMNET\",\"\",\"\",1\r\n", "OK", 1))		//����ΪGPRS����ģʽ
				netDeviceInfo.initStep++;
			else
				break;
		case 7:
			
			UsartPrintf(USART_DEBUG, "AT+QIDEACT=1\r\n");
			if(!NET_DEVICE_SendCmd("AT+QIDEACT=1\r\n", "OK", 1))		//����ΪGPRS����ģʽ
				netDeviceInfo.initStep++;
			else
				break;
		case 8:
			
			UsartPrintf(USART_DEBUG, "AT+QIACT=1\r\n");
			if(!NET_DEVICE_SendCmd("AT+QIACT=1\r\n", "OK", 1))		//����ΪGPRS����ģʽ
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

			if(!NET_DEVICE_SendCmd(cfgBuffer, "CONNECT", 1)) 					//����ƽ̨
				netDeviceInfo.initStep++;
			else
				break;
			
//		case 10:
//			
//			UsartPrintf(USART_DEBUG, "AT+CIPSTATUS\r\n");
//			if(!NET_DEVICE_SendCmd("AT+CIPSTATUS\r\n", "CONNECT OK", 1))		//����ƽ̨
//				netDeviceInfo.initStep++;
//			else
//			{
//				NET_DEVICE_SendCmd("AT+CIPCLOSE=1\r\n", "CLOSE OK", 1);			//�ر�����
//				NET_DEVICE_SendCmd("AT+CIPSHUT\r\n", "SHUT OK", 1);				//�ر��ƶ�����
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
//	�������ƣ�	NET_DEVICE_Reset
//
//	�������ܣ�	�����豸��λ
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
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
//	�������ƣ�	NET_DEVICE_ReLink
//
//	�������ܣ�	����ƽ̨
//
//	��ڲ�����	��
//
//	���ز�����	�������ӽ��
//
//	˵����		0-�ɹ�		1-ʧ��
//==========================================================
_Bool NET_DEVICE_ReLink(char *ip, char *port)
{
	
	_Bool status = 0;
	char cfgBuffer[32];
	
	NET_DEVICE_SendCmd("AT+QICLOSE=0\r\n", "OK", 1); 	//�ر�����
	RTOS_TimeDly(20);
	
	memset(cfgBuffer, 0, sizeof(cfgBuffer));
			
	strcpy(cfgBuffer, "AT+QIOPEN=1,0,\"TCP\",\"");
	strcat(cfgBuffer, ip);
	strcat(cfgBuffer, "\",");
	strcat(cfgBuffer, port);
	strcat(cfgBuffer,",0,2");
	strcat(cfgBuffer, "\r\n");
	UsartPrintf(USART_DEBUG, "STA Tips:	%s", cfgBuffer);

	status = NET_DEVICE_SendCmd(cfgBuffer, "CONNECT OK", 1); 	//����ƽ̨
	
	return status;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_SendCmd
//
//	�������ܣ�	�������豸����һ��������ȴ���ȷ����Ӧ
//
//	��ڲ�����	cmd����Ҫ���͵�����
//				res����Ҫ��������Ӧ
//				mode��1-�������		0-�����(�ܻ�ȡ������Ϣ)
//
//	���ز�����	�������ӽ��
//
//	˵����		0-�ɹ�		1-ʧ��
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
//	�������ƣ�	NET_DEVICE_SendData
//
//	�������ܣ�	ʹ�����豸�������ݵ�ƽ̨
//
//	��ڲ�����	data����Ҫ���͵�����
//				len�����ݳ���
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void NET_DEVICE_SendData(unsigned char *data, unsigned short len)
{
	
#if(NET_DEVICE_TRANS == 1)
	NET_IO_Send(data, len);  //�����豸������������
#else
	char cmdBuf[30];

	RTOS_TimeDly(10);
	
	NET_IO_ClearRecive();
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);
	//if(!NET_DEVICE_SendCmd(cmdBuf, ">", 1))			//�յ���>��ʱ���Է�������
	NET_DEVICE_SendCmd(cmdBuf, ">", 1);
	{
		NET_IO_Send(data, len);  //�����豸������������
	}
#endif

}

//==========================================================
//	�������ƣ�	NET_DEVICE_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
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
//	�������ƣ�	NET_DEVICE_ClrData
//
//	�������ܣ�	��������豸���ݽ��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void NET_DEVICE_ClrData(void)
{

	NET_IO_ClearRecive();

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Check
//
//	�������ܣ�	��������豸����״̬
//
//	��ڲ�����	��
//
//	���ز�����	����״̬
//
//	˵����		
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
			if(strstr((const char *)netIOInfo.buf, "TCP CLOSED"))			//���ӹر�
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
//	�������ƣ�	NET_DEVICE_ReConfig
//
//	�������ܣ�	�豸�����豸��ʼ���Ĳ���
//
//	��ڲ�����	����ֵ
//
//	���ز�����	��
//
//	˵����		�ú������õĲ����������豸��ʼ������õ�
//==========================================================
void NET_DEVICE_ReConfig(unsigned char step)
{

	netDeviceInfo.initStep = step;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Set_DataMode
//
//	�������ܣ�	�����豸�����շ�ģʽ
//
//	��ڲ�����	ģʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void NET_DEVICE_Set_DataMode(unsigned char mode)
{

	netDeviceInfo.dataType = mode;

}

//==========================================================
//	�������ƣ�	NET_DEVICE_Get_DataMode
//
//	�������ܣ�	��ȡ�豸�����շ�ģʽ
//
//	��ڲ�����	��
//
//	���ز�����	ģʽ
//
//	˵����		
//==========================================================
unsigned char NET_DEVICE_Get_DataMode(void)
{

	return netDeviceInfo.dataType;

}


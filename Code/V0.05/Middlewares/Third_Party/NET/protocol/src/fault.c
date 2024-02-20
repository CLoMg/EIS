/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	fault.c
	*
	*	���ߣ� 		CL
	*
	*	���ڣ� 		2022-10-07
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		����״̬������
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

#include "fault.h"
#include "selfcheck.h"
#include "usart.h"
#include "onenet.h"

#include "net_device.h"





unsigned char faultType = FAULT_NONE;			//��ʼΪ�޴�������
unsigned char faultTypeReport = FAULT_NONE;		//������������õ�



/*
************************************************************
*	�������ƣ�	Fault_Process
*
*	�������ܣ�	����״̬������
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		�������������Э������豸����Ĵ���
************************************************************
*/
void Fault_Process(void)
{

	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);						//����Ϊ�����շ�ģʽ
	
	switch(faultType)
	{
		case FAULT_NONE:
			
			UsartPrintf(USART_DEBUG, "WARN:	FAULT_NONE\r\n");
		
		break;
		
		case FAULT_REBOOT: 											//���������Ĵ���
			
			UsartPrintf(USART_DEBUG, "WARN:	FAULT_REBOOT\r\n");
		
			faultType = FAULT_NONE;									//�����������豸�ᱻ��λ�����������³�ʼ��
		
		break;
		
		case FAULT_EDP: //Э������Ĵ���
			
			UsartPrintf(USART_DEBUG, "WARN:	FAULT_MQTT\r\n");
		
			faultType = FAULT_NONE;
		
			oneNetInfo.netWork = 0; 								//������������ƽ̨
			
			NET_DEVICE_ReLink(oneNetInfo.ip, oneNetInfo.port);		//����IP
		
		break;
		
		case FAULT_NODEVICE: 										//�豸�����Ĵ���
			
			UsartPrintf(USART_DEBUG, "WARN:	FAULT_NODEVICE\r\n");
		
			faultType = FAULT_NONE;		

			checkInfo.NET_DEVICE_OK = DEV_ERR;						//�豸����
			//NET_DEVICE_Reset();										//�����豸��λ(ע����202210.13������Ӧ��ֻ�޸ı�־λ����λ�����ŵ�NET_Task����)
			NET_DEVICE_ReConfig(0);									//���³�ʼ��
			oneNetInfo.netWork = 0;									//����δ����
		
		break;
		
		default:
			break;
	}
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);						//����Ϊ�����շ�ģʽ

}
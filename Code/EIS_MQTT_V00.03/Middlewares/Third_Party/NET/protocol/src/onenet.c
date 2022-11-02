/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
	*	���ߣ� 		CL
	*
	*	���ڣ� 		2022-10-07
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		��onenetƽ̨�����ݽ�����Э���
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f1xx.h"

//�����豸
#include "net_device.h"

//Э���ļ�
#include "onenet.h"
#include "fault.h"
#include "cJSON.h"

//�û�����ͷ�ļ�
#include "sysparams.h"

//Ӳ������
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "sht3x.h"
#include "pms7003.h"
#include "net_io.h"
#include "mh4xx.h"
#include "rtc.h"
//#include "beep.h"
//#include "iwdg.h"
//#include "hwtimer.h"
#include "selfcheck.h"
#include "direct.h"
#include "scd4x_i2c.h"
//ͼƬ�����ļ�
//#include "image_2k.h"

//C��
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



struct MqttSampleContext ctx[1];
char *topics[] = {"$thing/down/property/YCWTRQGPFI/EIS"};


//ONETNET_INFO oneNetInfo = {"YCWTRQGPFIDev001", "", "YCWTRQGPFIDev001;12010126;9KYWH", "b17b8b0d043cb6cfd70758cb574407d5ff04aef5bdee31e5d294fd9afda66040;hmacsha256", "175.178.30.200", "1883", 0, 0, 0, 0};
ONETNET_INFO oneNetInfo = {"YCWTRQGPFIDev001","","YCWTRQGPFIDev001;12010126;M5ADW;1666146085",  "2c9ea89ffc984a4c544872fc9331bc5cea120e6ab4c5daec9956577ed56f9731;hmacsha256","175.178.30.200", "1883", 0, 0, 0, 0};
extern DATA_STREAM dataStream[];


/*
************************************************************
*	�������ƣ�	OneNet_DevLink
* 
*	�������ܣ�	��onenet��������
*
*	��ڲ�����	devid�������豸��devid
*				auth_key�������豸��masterKey��apiKey
*
*	���ز�����	��
*
*	˵����		��onenetƽ̨�������ӣ��ɹ������oneNetInfo.netWork����״̬��־
************************************************************
*/
void OneNet_DevLink(const char* devid, const char* auth_key)
{
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                        "DEVID: %s,     APIKEY: %s\r\n"
                        , devid, auth_key);
	
	if(MqttSample_ReInit(ctx) < 0)
	{
		UsartPrintf(USART_DEBUG, "MqttSample_ReInit Failed\r\n");
		return;
	}
	
	if(MqttSample_Connect(ctx, oneNetInfo.proID, oneNetInfo.auif, oneNetInfo.devID, KEEP_ALIVE, C_SESSION) == 0)
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	MqttBuffer_Reset(ctx->mqttbuf);
	
	RTOS_TimeDly(1000/portTICK_PERIOD_MS);	
}


/*
************************************************************
*	�������ƣ�	OneNet_DevDisLink
* 
*	�������ܣ�	��onenet�Ͽ�����
*
*	��ڲ�����	devid�������豸��devid
*				auth_key�������豸��masterKey��apiKey
*
*	���ز�����	��
*
*	˵����		��onenetƽ̨�������ӣ��ɹ������oneNetInfo.netWork����״̬��־
************************************************************
*/
void OneNet_DevDisLink(void)
{
	
	uint8_t DisLink_CMD[]={0xE0, 0x00};
	UsartPrintf(USART_DEBUG, "OneNet_DevDisLink\r\n");
	

	NET_IO_Send(DisLink_CMD, 2);
	
	RTOS_TimeDly(1000/portTICK_PERIOD_MS);	
}
/*
************************************************************
*	�������ƣ�	OneNet_toString
*
*	�������ܣ�	����ֵתΪ�ַ���
*
*	��ڲ�����	dataStream��������
*				buf��ת����Ļ���
*				pos���������е��ĸ�����
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void OneNet_toString(DATA_STREAM *dataStream, char *buf, unsigned short pos)
{
	
	memset(buf, 0, 48);

	switch((unsigned char)dataStream[pos].dataType)
	{
		case TYPE_BOOL:
			snprintf(buf, 48, "%d", *(_Bool *)dataStream[pos].data);
		break;
		
		case TYPE_CHAR:
			snprintf(buf, 48, "%d", *(char *)dataStream[pos].data);
		break;
		
		case TYPE_UCHAR:
			snprintf(buf, 48, "%d", *(unsigned char *)dataStream[pos].data);
		break;
		
		case TYPE_SHORT:
			snprintf(buf, 48, "%d", *(short *)dataStream[pos].data);
		break;
		
		case TYPE_USHORT:
			snprintf(buf, 48, "%d", *(unsigned short *)dataStream[pos].data);
		break;
		
		case TYPE_INT:
			snprintf(buf, 48, "%d", *(int *)dataStream[pos].data);
		break;
		
		case TYPE_UINT:
			snprintf(buf, 48, "%d", *(unsigned int *)dataStream[pos].data);
		break;
		
		case TYPE_LONG:
			snprintf(buf, 48, "%ld", *(long *)dataStream[pos].data);
		break;
		
		case TYPE_ULONG:
			snprintf(buf, 48, "%ld", *(unsigned long *)dataStream[pos].data);
		break;
			
		case TYPE_FLOAT:
			snprintf(buf, 48, "%f", *(float *)dataStream[pos].data);
		break;
		
		case TYPE_DOUBLE:
			snprintf(buf, 48, "%f", *(double *)dataStream[pos].data);
		break;
		
		case TYPE_GPS:
			snprintf(buf, 48, "{\"lon\":%s,\"lat\":%s}", (char *)dataStream[pos].data, (char *)dataStream[pos].data + 16);
		break;
	}

}

/*
************************************************************
*	�������ƣ�	OneNet_Load_DataStream
*
*	�������ܣ�	��������װ
*
*	��ڲ�����	type���������ݵĸ�ʽ
*				send_buf�����ͻ���ָ��
*				len�������������ĸ���
*
*	���ز�����	��
*
*	˵����		��װ��������ʽ
************************************************************
*/
void OneNet_Load_DataStream(char *send_buf, unsigned char len)
{
	
	unsigned char count = 0;
	char stream_buf[96];
	char data_buf[48];
	char *ptr = send_buf;

	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-kTypeFullJson\r\n");
		
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-kTypeFullJson\r\n");
		
	strncpy(send_buf, "{\"datastreams\":[", strlen("{\"datastreams\":["));
	for(; count < len; count++)
	{
		if(dataStream[count].flag) //���ʹ�ܷ��ͱ�־λ
		{
			memset(stream_buf, 0, sizeof(stream_buf));

			OneNet_toString(dataStream, data_buf, count);
			snprintf(stream_buf, sizeof(stream_buf), "{\"id\":\"%s\",\"datapoints\":[{\"value\":%s}]},", dataStream[count].name, data_buf);
			
			strncat(send_buf, stream_buf, strlen(stream_buf));
		}
	}
	
	while(*ptr != '\0')					//�ҵ�������
		ptr++;
	*(--ptr) = '\0';					//������','�滻Ϊ������
	
	strncat(send_buf, "]}", 2);

}

/*
************************************************************
*	�������ƣ�	OneNet_SendData
*
*	�������ܣ�	�ϴ����ݵ�ƽ̨
*
*	��ڲ�����	len�������������ĸ���
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/

_Bool OneNet_SendData(unsigned char len)
{
	char *cjson_send = NULL;
	
	if(!oneNetInfo.netWork)															//�������δ����
		return 1;
	 
	RTOS_ENTER_CRITICAL();
	cJSON *Dev_Propert = NULL;
	Dev_Propert = cJSON_CreateObject();
	
	cJSON_AddStringToObject(Dev_Propert,"version","1.0");
	cJSON_AddStringToObject(Dev_Propert,"type","property");
	cJSON_AddStringToObject(Dev_Propert,"ack","1");
	char id_str[14] ="";
	sprintf(id_str,"%lld",Read_Stamp());
	cJSON_AddStringToObject(Dev_Propert,"id",id_str);
	cJSON_AddStringToObject(Dev_Propert,"timestamp",id_str);
	cJSON_AddStringToObject(Dev_Propert,"method","property");
	
	cJSON *params = NULL;
	params = cJSON_CreateObject();
	
	cJSON *property = cJSON_CreateObject();
	cJSON_AddNumberToObject(property,"Temp_Env",Temp_Value);
	cJSON_AddNumberToObject(property,"Humi_Env",Humi_Value);
	cJSON_AddNumberToObject(property,"CO2",CO2_Value);
	cJSON_AddNumberToObject(property,"PM2.5",4000.032);
	cJSON_AddNumberToObject(property,"PM10",-1.09);
	cJSON_AddNumberToObject(property,"Lgt_Dir",0);
	cJSON_AddStringToObject(property,"Mac",mac_strings);
	
	cJSON_AddItemToObject(params,"property",property);
	
	cJSON_AddItemToObject(Dev_Propert,"params",params);
	cjson_send = cJSON_Print(Dev_Propert);
	cJSON_Delete(Dev_Propert);
	
	if(MqttSample_Savedata(ctx, cjson_send) == 0)
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	else
		UsartPrintf(USART_DEBUG, "WARN:		MqttSample_Savedata Failed\r\n");
	//cJSON_Print() ������ڴ���Ҫ�Լ��ͷ�
	vPortFree(cjson_send);
	RTOS_EXIT_CRITICAL();
	
	MqttBuffer_Reset(ctx->mqttbuf);
	
	//NET_DEVICE_ClrData();

	faultTypeReport = FAULT_NONE;													//����֮��������
	
	return 0;

}

/*
************************************************************
*	�������ƣ�	OneNet_Publish
*
*	�������ܣ�	������Ϣ
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
_Bool OneNet_Publish(void)
{

	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//�������δ���� �� ��Ϊ�����շ�ģʽ
		return 1;
	
	NET_DEVICE_ClrData();
	
	UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
	
	if(MqttSample_Publish(ctx, "OneNet MQTT Publish Test") == 0)
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	MqttBuffer_Reset(ctx->mqttbuf);
	
	return 0;

}

/*
************************************************************
*	�������ƣ�	OneNet_Subscribe
*
*	�������ܣ�	������Ϣ
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
_Bool OneNet_Subscribe(void)
{
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//�������δ���� �� ��Ϊ�����շ�ģʽ
		return 1;
	
	NET_DEVICE_ClrData();
	
	UsartPrintf(USART_DEBUG, "OneNet_Subscribe\r\n");

	if(MqttSample_Subscribe(ctx, topics, 1) == 0)									//��һ�ζ��Ķ��
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	MqttBuffer_Reset(ctx->mqttbuf);
	
	return 0;

}

/*
************************************************************
*	�������ƣ�	OneNet_UnSubscribe
*
*	�������ܣ�	ȡ������
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
_Bool OneNet_UnSubscribe(void)
{
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//�������δ���� �� ��Ϊ�����շ�ģʽ
		return 1;
	
	NET_DEVICE_ClrData();
	
	UsartPrintf(USART_DEBUG, "OneNet_UnSubscribe\r\n");

	if(MqttSample_Unsubscribe(ctx, topics, 2) == 0)									//��һ�ζ��Ķ��
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	MqttBuffer_Reset(ctx->mqttbuf);
	
	return 0;

}

/*
************************************************************
*	�������ƣ�	OneNet_HeartBeat
*
*	�������ܣ�	�������
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
_Bool OneNet_HeartBeat(void)
{
	
	unsigned char sCount = 5;
	unsigned char errType = 0;
	
	if(!oneNetInfo.netWork)															//�������δ����
		return 1;
	
	oneNetInfo.heartBeat = 0;
	
	while(--sCount)																	//ѭ��������
	{
		if(Mqtt_PackPingReqPkt(ctx->mqttbuf) == MQTTERR_NOERROR)
			Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
		else
			UsartPrintf(USART_DEBUG, "WARN:		Mqtt_PackPingReqPkt Failed\r\n");
		MqttBuffer_Reset(ctx->mqttbuf);
		
		//Mqtt_RecvPkt(ctx->mqttctx);
		RTOS_TimeDly(1000/portTICK_PERIOD_MS);
		
		if(oneNetInfo.heartBeat == 1)
		{
			errType = CHECK_NO_ERR;													//����޴���
			MqttBuffer_Reset(ctx->mqttbuf);
			UsartPrintf(USART_DEBUG, "Tips:	HeartBeat OK\r\n");
			break;
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Check Device\r\n");
			
			//errType = NET_DEVICE_Check();
		}
	}
	
	if(sCount == 0)																//����
	{
		UsartPrintf(USART_DEBUG, "HeartBeat TimeOut\r\n");
		
		//errType = NET_DEVICE_Check();											//�����豸״̬���
	}
	
	errType = CHECK_NO_ERR;
	if(errType == CHECK_CONNECTED || errType == CHECK_CLOSED || errType == CHECK_GOT_IP)
		faultTypeReport = faultType = FAULT_EDP;								//���ΪЭ�����
	else if(errType == CHECK_NO_DEVICE)
		faultTypeReport = faultType = FAULT_NODEVICE;							//���Ϊ�豸����
	else
		faultTypeReport = faultType = FAULT_NONE;								//�޴���
	
//	NET_DEVICE_ClrData();														//�������
	
	return 0;

}

/*
************************************************************
*	�������ƣ�	OneNet_App
*
*	�������ܣ�	ƽ̨�·��������������
*
*	��ڲ�����	cmd��ƽ̨�·�������
*
*	���ز�����	��
*
*	˵����		��ȡ�������Ӧ����
************************************************************
*/
void OneNet_App(char *cmd)
{

	char *dataPtr;
	char numBuf[10];
	int num = 0;
	cJSON	*cjson_cmd = NULL;
	cjson_cmd = cJSON_Parse(cmd);

	if(cjson_cmd == NULL)
	{
		UsartPrintf(USART_DEBUG,"Can't find valid cmd\r\n");
	}
	else
	{
		cJSON *cjson_params = cJSON_GetObjectItem(cjson_cmd,"params");
		if(cjson_params == NULL)
		{
			UsartPrintf(USART_DEBUG,"Can't find valid params objects\r\n");
		}
		else
		{
			cJSON *cjson_Chg_DUP = cJSON_GetObjectItem(cjson_params,"Chg_DUP");
			if(cjson_Chg_DUP !=NULL){
				num = cjson_Chg_DUP->valueint;
				/*����ָ�����*/
				Dev_UpLoad_Per_Change((unsigned char) num);
				oneNetInfo.sendData = 1;					//������ݷ���
			}
			cJSON *cjson_Read_Ver = cJSON_GetObjectItem(cjson_params,"Read_Ver");
			if(cjson_Read_Ver !=NULL){
				/*����ָ�����*/
				char cur_ver_str[7] ="";
				unsigned short cur_ver = 0;
				Dev_FwVer_Read(&cur_ver);
			//	sprintf(cur_ver_str,"%2.2f",(float)cur_ver/100);
				oneNetInfo.sendData = 1;					//������ݷ���
			}
			cJSON *cjson_Send_FW = cJSON_GetObjectItem(cjson_params,"Send_FW");
			if(cjson_Send_FW !=NULL){
				/*����ָ�����*/
				
				oneNetInfo.sendData = 1;					//������ݷ���
			}
			cJSON *cjson_Sync_Time = cJSON_GetObjectItem(cjson_params,"Sync_Time");
			if(cjson_Sync_Time !=NULL){
				/*����ָ�����*/
				unsigned int timestamp =cjson_Sync_Time->valueint;
				Dev_Set_Time(timestamp);
				oneNetInfo.sendData = 1;					//������ݷ���
			}
			cJSON *cjson_Reboot = cJSON_GetObjectItem(cjson_params,"Reboot");
			if(cjson_Reboot !=NULL){
				/*����ָ�����*/
				
				oneNetInfo.sendData = 1;					//������ݷ���
			}
			cJSON *cjson_Chg_Loc = cJSON_GetObjectItem(cjson_params,"Chg_Loc");
			if(cjson_Chg_Loc !=NULL){
				unsigned short new_dev_loc[2] ={0,0};
				int array_size  = cJSON_GetArraySize(cjson_Chg_Loc);
				unsigned char i = 0;
				for(i=0 ; i<array_size ; ++i)
					new_dev_loc[i] = cJSON_GetArrayItem(cjson_Chg_Loc,i)->valueint;
				/*����ָ�����*/
				Dev_Location_Change(new_dev_loc);
				oneNetInfo.sendData = 1;					//������ݷ���
			}
			cJSON *cjson_Send_AbnLoc = cJSON_GetObjectItem(cjson_params,"Send_AbnLoc");
			if(cjson_Send_AbnLoc !=NULL){
				unsigned short abn_loc[2] ={0,0};
				int array_size  = cJSON_GetArraySize(cjson_Send_AbnLoc);
				unsigned char i = 0;
				for(i=0 ; i<array_size ; ++i)
					abn_loc[i] = cJSON_GetArrayItem(cjson_Send_AbnLoc,i)->valueint;
				/*����ָ�����*/
				AbneEvt_Dispose(abn_loc);
				oneNetInfo.sendData = 1;					//������ݷ���
			}
			
			cJSON *cjson_DIR = cJSON_GetObjectItem(cjson_params,"DIR");
			if(cjson_DIR !=NULL){
				/*����ָ�����*/
				Direct_Light_Control(cjson_DIR->valueint);
				oneNetInfo.sendData = 1;					//������ݷ���
			}
		}
		cJSON_Delete(cjson_cmd);
	}
//	NET_DEVICE_ClrData();								//��ջ���
}

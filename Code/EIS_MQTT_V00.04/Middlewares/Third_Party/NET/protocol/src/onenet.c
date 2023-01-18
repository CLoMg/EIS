/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		CL
	*
	*	日期： 		2022-10-07
	*
	*	版本： 		V1.0
	*
	*	说明： 		与onenet平台的数据交互，协议层
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f1xx.h"

//网络设备
#include "net_device.h"

//协议文件
#include "onenet.h"
#include "fault.h"
#include "cJSON.h"

//用户参数头文件
#include "sysparams.h"

//硬件驱动
#include "sysparams.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "sht3x.h"
#include "dsl_08.h"
#include "net_io.h"
#include "mh4xx.h"
#include "rtc.h"
//#include "beep.h"
//#include "iwdg.h"
//#include "hwtimer.h"
#include "selfcheck.h"
#include "direct.h"
#include "scd4x_i2c.h"
//图片数据文件
//#include "image_2k.h"

//C库
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "cmsis_os.h"



struct MqttSampleContext ctx[1];
//char *topic_str[] = {"100601/ssb_uuid/12333131354838a3/property/down",
//										"100601/ssb_uuid/12333131354838a3/function/down",
//										"100601/ssb_uuid/12333131354838a3/event/down"};

ONETNET_INFO oneNetInfo = {"YCWTRQGPFIDev001", "", "YCWTRQGPFIDev001;12010126;9KYWH", "b17b8b0d043cb6cfd70758cb574407d5ff04aef5bdee31e5d294fd9afda66040;hmacsha256", "175.178.30.200", "1883", 0, 0, 0, 0};
//ONETNET_INFO oneNetInfo = {"YCWTRQGPFIDev001","","YCWTRQGPFIDev001;12010126;M5ADW;1666146085",  "2c9ea89ffc984a4c544872fc9331bc5cea120e6ab4c5daec9956577ed56f9731;hmacsha256","175.178.30.200", "1883", 0, 0, 0, 0};
extern DATA_STREAM dataStream[];
unsigned char wait_reply = 0;

/*
************************************************************
*	函数名称：	OneNet_Topic_Init
* 
*	函数功能：	根据设备Mac 地址 创建topic
*
*	入口参数：	devid：创建设备的devid
*				auth_key：创建设备的masterKey或apiKey
*
*	返回参数：	无
*
*	说明：		与onenet平台建立连接，成功或会标记oneNetInfo.netWork网络状态标志
************************************************************
*/


/*
************************************************************
*	函数名称：	OneNet_DevLink
* 
*	函数功能：	与onenet创建连接
*
*	入口参数：	devid：创建设备的devid
*				auth_key：创建设备的masterKey或apiKey
*
*	返回参数：	无
*
*	说明：		与onenet平台建立连接，成功或会标记oneNetInfo.netWork网络状态标志
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
*	函数名称：	OneNet_DevDisLink
* 
*	函数功能：	与onenet断开连接
*
*	入口参数：	devid：创建设备的devid
*				auth_key：创建设备的masterKey或apiKey
*
*	返回参数：	无
*
*	说明：		与onenet平台建立连接，成功或会标记oneNetInfo.netWork网络状态标志
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
*	函数名称：	OneNet_SendData
*
*	函数功能：	上传数据到平台
*
*	入口参数：	len：发送数据流的个数
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/

_Bool OneNet_SendData(unsigned char len)
{
	char *cjson_send = NULL;
	wait_reply++;
	if(wait_reply > 5)
		oneNetInfo.netWork=0;
	if(!oneNetInfo.netWork)															//如果网络未连接
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
	
	//cJSON *property = cJSON_CreateObject();
	cJSON_AddNumberToObject(params,"Temp_Env",Temp_Value);
	cJSON_AddNumberToObject(params,"Humi_Env",Humi_Value);
	cJSON_AddNumberToObject(params,"CO2",CO2_Value);
	cJSON_AddNumberToObject(params,"PM2.5",PM25_Data);
	cJSON_AddNumberToObject(params,"PM10",PM10_Data);
	cJSON_AddNumberToObject(params,"Lgt_Dir",(unsigned char)Direct_Light_Get());
	cJSON_AddStringToObject(params,"Mac",mac_strings);
	
	//cJSON_AddItemToObject(params,"property",property);
	
	cJSON_AddItemToObject(Dev_Propert,"params",params);
	cjson_send = cJSON_Print(Dev_Propert);
	cJSON_Delete(Dev_Propert);
	
	if(MqttSample_Savedata(ctx, cjson_send) == 0)
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	else
		UsartPrintf(USART_DEBUG, "WARN:		MqttSample_Savedata Failed\r\n");
	//cJSON_Print() 申请的内存需要自己释放
	vPortFree(cjson_send);
	RTOS_EXIT_CRITICAL();
	
	MqttBuffer_Reset(ctx->mqttbuf);
	
	//NET_DEVICE_ClrData();

	faultTypeReport = FAULT_NONE;													//发送之后清除标记
	
	return 0;

}

/*
************************************************************
*	函数名称：	OneNet_SendData
*
*	函数功能：	上传数据到平台
*
*	入口参数：	len：发送数据流的个数
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/

_Bool OneNet_Send_Process(void)
{
	unsigned char send_buff[400];
	unsigned short len = 0;
	unsigned short window_start = 0;
	unsigned short window_stop = 0;
	unsigned short sub_sec = 0;
	sub_sec = Read_MS()%(Upload_Period*1000);
	//如果没有初始化
	if((Group_Info[0] == 0)||(Group_Info[1] == 0))
	{
		//发送链表函数
		struct MqttExtent *q;
		if(Send_List != NULL)
		{
			q = Send_List->next;
			Send_List->next = NULL;
			len = Send_List->len;
			memcpy(send_buff,Send_List->payload,len);
			vPortFree(Send_List->payload);
			vPortFree(Send_List);
			Send_List = q;
		}
		memset(send_buff,0,400);
	}
	else
	{
		window_start = Upload_Period * 1000 / Group_Info[0] * (Group_Info[1]-1);
		window_stop = window_start + 800;
		while((sub_sec >= window_start)&&(sub_sec<=window_stop))
		{
			//发送链表函数
			struct MqttExtent *q;
			if(Send_List != NULL)
			{
				q = Send_List->next;
				Send_List->next = NULL;
				len = Send_List->len;
				memcpy(send_buff,Send_List->payload,len);
				vPortFree(Send_List->payload);
				vPortFree(Send_List);
				Send_List = q;
				NET_DEVICE_SendData(send_buff,len);
				osDelay(150);
			}
			memset(send_buff,0,400);
			sub_sec = Read_MS()%(Upload_Period*1000);
		}
	}
		
	return 0;
}
/*
************************************************************
*	函数名称：	OneNet_Publish
*
*	函数功能：	发布消息
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
_Bool OneNet_Publish(const char *topic, const char *payload)
{

	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//如果网络未连接 或 不为数据收发模式
		return 1;
	
	NET_DEVICE_ClrData();
	
	UsartPrintf(USART_DEBUG, "OneNet_Publish\r\n");
	
	if(MqttSample_Publish(topic,ctx, (char *)payload) == 0)
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	MqttBuffer_Reset(ctx->mqttbuf);
	
	return 0;

}

/*
************************************************************
*	函数名称：	OneNet_Subscribe
*
*	函数功能：	订阅消息
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
_Bool OneNet_Subscribe(void)
{
	char *type_str[] = {"/property/","/function/","/event/"};
	char i = 0;
	if(!oneNetInfo.netWork)		//如果网络未连接 或 不为数据收发模式
		return 1;
	wait_reply = 0;
	NET_DEVICE_ClrData();
	
//	UsartPrintf(USART_DEBUG, "OneNet_Subscribe\r\n");
	char *topic_string = pvPortMalloc(strlen(CMD_TOPIC_PREFIX)+strlen(mac_strings)+strlen("/property/down"));
	for(i = 0;i < 3;++i){
		memset(topic_string,0,strlen(topic_string));
		topic_string  = strcpy(topic_string,CMD_TOPIC_PREFIX);
		topic_string = strcat(topic_string,mac_strings);
		topic_string = strcat(topic_string,type_str[i]);
		topic_string = strcat(topic_string,"down");
		if(MqttSample_Subscribe(ctx,&topic_string, 1) == 0)									//可一次订阅多个
			Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
		MqttBuffer_Reset(ctx->mqttbuf);
	}
	
	
	vPortFree(topic_string);
	return 0;

}

/*
************************************************************
*	函数名称：	OneNet_OnLine
*
*	函数功能：	 上线消息
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
_Bool OneNet_OnLine(void)
{
	char *cjson_send = NULL;

	 
	RTOS_ENTER_CRITICAL();
	cJSON *Dev_Propert = NULL;
	Dev_Propert = cJSON_CreateObject();
	
	cJSON_AddStringToObject(Dev_Propert,"version","1.0");
	cJSON_AddStringToObject(Dev_Propert,"type","event");
	cJSON_AddStringToObject(Dev_Propert,"ack","1");
	char id_str[14] ="";
	sprintf(id_str,"%lld",Read_Stamp());
	cJSON_AddStringToObject(Dev_Propert,"id",id_str);
	cJSON_AddStringToObject(Dev_Propert,"timestamp",id_str);
	cJSON_AddStringToObject(Dev_Propert,"method","OnLine");
	
	cJSON *params = NULL;
	params = cJSON_CreateObject();
	cJSON_AddStringToObject(params,"Mac",mac_strings);
	
	
	cJSON_AddItemToObject(Dev_Propert,"params",params);
	cjson_send = cJSON_Print(Dev_Propert);
	cJSON_Delete(Dev_Propert);
	
	char *topic_string = pvPortMalloc(strlen(CMD_TOPIC_PREFIX)+strlen(mac_strings)+strlen("/event/up"));
	
		memset(topic_string,0,strlen(topic_string));
		topic_string  = strcpy(topic_string,CMD_TOPIC_PREFIX);
		topic_string = strcat(topic_string,mac_strings);
		topic_string = strcat(topic_string,"/event/up");
	if(MqttSample_Publish(topic_string,ctx, cjson_send) == 0)
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	else
		UsartPrintf(USART_DEBUG, "WARN:		MqttSample_Savedata Failed\r\n");
	//cJSON_Print() 申请的内存需要自己释放
	vPortFree(topic_string);
	vPortFree(cjson_send);
	MqttBuffer_Reset(ctx->mqttbuf);
	RTOS_EXIT_CRITICAL();
	
	return 0;

}

///*
//************************************************************
//*	函数名称：	OneNet_Sync_Time
//*
//*	函数功能：	请求时间同步
//*
//*	入口参数：	无
//*
//*	返回参数：	无
//*
//*	说明：		
//************************************************************
//*/
//_Bool OneNet_Query(void)
//{
//	char *cjson_send = NULL;

//	if(!oneNetInfo.netWork)															//如果网络未连接
//		return 1;
//	 
//	RTOS_ENTER_CRITICAL();
//	cJSON *Dev_Propert = NULL;
//	Dev_Propert = cJSON_CreateObject();
//	
//	cJSON_AddStringToObject(Dev_Propert,"version","1.0");
//	cJSON_AddStringToObject(Dev_Propert,"type","event");
//	cJSON_AddStringToObject(Dev_Propert,"ack","1");
//	char id_str[14] ="";
//	sprintf(id_str,"%lld",Read_Stamp());
//	cJSON_AddStringToObject(Dev_Propert,"id",id_str);
//	cJSON_AddStringToObject(Dev_Propert,"timestamp",id_str);
//	cJSON_AddStringToObject(Dev_Propert,"method","OnLine");
//	
//	cJSON *params = NULL;
//	params = cJSON_CreateObject();
//	cJSON_AddStringToObject(params,"Mac",mac_strings);
//	
//	
//	cJSON_AddItemToObject(Dev_Propert,"params",params);
//	cjson_send = cJSON_Print(Dev_Propert);
//	cJSON_Delete(Dev_Propert);
//	
//	char *topic_string = pvPortMalloc(strlen(CMD_TOPIC_PREFIX)+strlen(mac_strings)+strlen("/event/up"));
//	
//		memset(topic_string,0,strlen(topic_string));
//		topic_string  = strcpy(topic_string,CMD_TOPIC_PREFIX);
//		topic_string = strcat(topic_string,mac_strings);
//		topic_string = strcat(topic_string,"/event/up");
//	if(MqttSample_Publish(topic_string,ctx, cjson_send) == 0)
//		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
//	else
//		UsartPrintf(USART_DEBUG, "WARN:		MqttSample_Savedata Failed\r\n");
//	//cJSON_Print() 申请的内存需要自己释放
//	vPortFree(cjson_send);
//	MqttBuffer_Reset(ctx->mqttbuf);
//	RTOS_EXIT_CRITICAL();
//	
//	return 0;

//}


/*
************************************************************
*	函数名称：	OneNet_UnSubscribe
*
*	函数功能：	取消订阅
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
_Bool OneNet_UnSubscribe(void)
{
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//如果网络未连接 或 不为数据收发模式
		return 1;
	
	NET_DEVICE_ClrData();
	
	UsartPrintf(USART_DEBUG, "OneNet_UnSubscribe\r\n");

//	if(MqttSample_Unsubscribe(ctx, topics, 2) == 0)									//可一次订阅多个
//		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
//	MqttBuffer_Reset(ctx->mqttbuf);
	
	return 0;

}

/*
************************************************************
*	函数名称：	OneNet_HeartBeat
*
*	函数功能：	心跳检测
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
_Bool OneNet_HeartBeat(void)
{
	
	unsigned char sCount = 5;
	unsigned char errType = 0;
	
	if(!oneNetInfo.netWork)															//如果网络未连接
		return 1;
	
	oneNetInfo.heartBeat = 0;
	
	while(--sCount)																	//循环检测计数
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
			errType = CHECK_NO_ERR;													//标记无错误
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
	
	if(sCount == 0)																//超出
	{
		UsartPrintf(USART_DEBUG, "HeartBeat TimeOut\r\n");
		
		//errType = NET_DEVICE_Check();											//网络设备状态检查
	}
	
	errType = CHECK_NO_ERR;
	if(errType == CHECK_CONNECTED || errType == CHECK_CLOSED || errType == CHECK_GOT_IP)
		faultTypeReport = faultType = FAULT_EDP;								//标记为协议错误
	else if(errType == CHECK_NO_DEVICE)
		faultTypeReport = faultType = FAULT_NODEVICE;							//标记为设备错误
	else
		faultTypeReport = faultType = FAULT_NONE;								//无错误
	
//	NET_DEVICE_ClrData();														//情况缓存
	
	return 0;

}

/*
************************************************************
*	函数名称：	OneNeT_Evesdrop
*
*	函数功能：	窃听同信道其他设备消息
*
*	入口参数：	cmd：平台下发的命令
*
*	返回参数：	无
*
*	说明：		提取出有用消息，进行处理
************************************************************
*/
void OneNet_Evesdrop(char *cmd)
{

	cJSON	*cjson_cmd = NULL;
	cjson_cmd = cJSON_Parse(cmd);

	if(cjson_cmd == NULL)
	{
		UsartPrintf(USART_DEBUG,"Can't find valid cmd\r\n");
	}
	else
	{

		cJSON *cjson_timestamp = cJSON_GetObjectItem(cjson_cmd,"timestamp");
		if(cjson_timestamp != NULL)
		{		
			double timestamp = strtod(cjson_timestamp->valuestring,NULL);
			Dev_Set_Time(timestamp/1000);
			oneNetInfo.syncTime = 1;
		}
	}
	cJSON_Delete(cjson_cmd);
}

/*
************************************************************
*	函数名称：	OneNet_App
*
*	函数功能：	平台下发命令解析、处理
*
*	入口参数：	cmd：平台下发的命令
*
*	返回参数：	无
*
*	说明：		提取出命令，响应处理
************************************************************
*/
void OneNet_App(char *cmd)
{

	//char *dataPtr;
//	char numBuf[10];
//	int num = 0;
	cJSON	*cjson_cmd = NULL;
	cjson_cmd = cJSON_Parse(cmd);

	if(cjson_cmd == NULL)
	{
		UsartPrintf(USART_DEBUG,"Can't find valid cmd\r\n");
	}
	else
	{	
		cJSON *cjson_type = cJSON_GetObjectItem(cjson_cmd,"type");
		if(strstr(cjson_type->valuestring,"reply")!=NULL)
		{
			wait_reply = 0;
		}
		cJSON *cjson_method = cJSON_GetObjectItem(cjson_cmd,"method");
		if(cjson_method == NULL)
		{
			UsartPrintf(USART_DEBUG,"Can't find valid method key:value\r\n");
		}
		else
		{
			char *topic_string = pvPortMalloc(strlen(CMD_TOPIC_PREFIX)+strlen(mac_strings)+strlen("/function/up")+1);
			topic_string  = strcpy(topic_string,CMD_TOPIC_PREFIX);
			topic_string = strcat(topic_string,mac_strings);
			topic_string = strcat(topic_string,"/function/up");
			if(strstr(cjson_method->valuestring,"Sync_Time")!=NULL)
			{
				cJSON *cjson_params= cJSON_GetObjectItem(cjson_cmd,"params");
				if(cjson_params !=NULL){
						cJSON *cjson_timestamp = cJSON_GetObjectItem(cjson_params,"Timestamp");	
						if(cjson_timestamp != NULL)
						{
						/*控制指令处理函数*/
							long  timestamp = cjson_timestamp->valuedouble/1000;
							char *re_msg = NULL;
							Dev_Set_Time(timestamp);
											//标记数据反馈
							cJSON_AddNumberToObject(cjson_params,"exec_result",1);
							cJSON_ReplaceItemInObject(cjson_cmd,"type",cJSON_CreateString("reply"));
							re_msg = cJSON_Print(cjson_cmd);
							OneNet_Publish(topic_string,re_msg);
						
							vPortFree(re_msg);
							oneNetInfo.sendData = 1;					//标记数据反馈
						}
				}
			}
			else if(strstr(cjson_method->valuestring,"Chg_LgtDir")!=NULL)
			{
				cJSON *cjson_params= cJSON_GetObjectItem(cjson_cmd,"params");
				if(cjson_params !=NULL){
						cJSON *cjson_Lgt_Dir= cJSON_GetObjectItem(cjson_params,"Lgt_Dir");	
						if(cjson_Lgt_Dir != NULL)
						{
						/*控制指令处理函数*/
							unsigned int Lgt_Dir = cjson_Lgt_Dir->valueint;
							char re_code = 0x00,*re_msg = NULL;
							
							re_code  = Direct_Light_Control((Directions_T)Lgt_Dir);
							cJSON_DeleteItemFromObject(cjson_params,"Lgt_Dir");
							cJSON_AddNumberToObject(cjson_params,"exec_result",re_code);
							cJSON_ReplaceItemInObject(cjson_cmd,"type",cJSON_CreateString("reply"));
							re_msg = cJSON_Print(cjson_cmd);
							OneNet_Publish(topic_string,re_msg);
							
							vPortFree(re_msg);
							oneNetInfo.sendData = 1;					//标记数据反馈
						}
				}
			}
			else if(strstr(cjson_method->valuestring,"Send_GroupInfo")!=NULL)
			{
				cJSON *cjson_params= cJSON_GetObjectItem(cjson_cmd,"params");
				if(cjson_params !=NULL){
						
						cJSON *cjson_groupinfo = cJSON_GetObjectItem(cjson_params,"GroupInfo");
						if(cjson_groupinfo != NULL)
						{
							int array_size = 0;
							array_size = cJSON_GetArraySize(cjson_groupinfo);
							if(array_size ==2 )
							{
									/*控制指令处理函数*/
								unsigned short group_buff[2] = {0,};
								for(int i =0; i < array_size; ++i)
									group_buff[i] = (unsigned short)cJSON_GetArrayItem(cjson_groupinfo,i)->valueint;
								char *re_msg = NULL;
								Dev_GroupInfo_Change(group_buff);
												//标记数据反馈
								cJSON_AddNumberToObject(cjson_params,"exec_result",1);
								cJSON_ReplaceItemInObject(cjson_cmd,"type",cJSON_CreateString("reply"));
								re_msg = cJSON_Print(cjson_cmd);
								OneNet_Publish(topic_string,re_msg);
							
								vPortFree(re_msg);
								oneNetInfo.sendData = 1;					//标记数据反馈
							}
						
						}
				}
			}
			else if(strstr(cjson_method->valuestring,"Read_DUP")!=NULL)
			{
				cJSON *cjson_params= cJSON_GetObjectItem(cjson_cmd,"params");
				if(cjson_params !=NULL){
						unsigned short dev_upload_per = 0; 
						char re_code = 0x00,*re_msg;
						/*控制指令处理函数*/
						re_code = Dev_UpLoad_Per_Read(&dev_upload_per);	
							
						
						//标记数据反馈
						cJSON_AddNumberToObject(cjson_params,"Dup",dev_upload_per);
						cJSON_AddNumberToObject(cjson_params,"result",re_code);
						cJSON_ReplaceItemInObject(cjson_cmd,"type",cJSON_CreateString("reply"));
						re_msg = cJSON_Print(cjson_cmd);
						OneNet_Publish(topic_string,re_msg);
					
						vPortFree(re_msg);
				}
			}
			else if(strstr(cjson_method->valuestring,"Chg_DUP")!=NULL)
			{
				cJSON *cjson_params= cJSON_GetObjectItem(cjson_cmd,"params");
				if(cjson_params !=NULL){
					cJSON *cjson_chg_dup = NULL;
					cjson_chg_dup = cJSON_GetObjectItem(cjson_params,"DUP");
					if(cjson_chg_dup != NULL){
						unsigned int dup_new = cjson_chg_dup->valueint;
						char re_code = 0x00,*re_msg;
						/*控制指令处理函数*/
						re_code = Dev_UpLoad_Per_Change(dup_new);	
							
							//标记数据反馈
						cJSON_AddNumberToObject(cjson_params,"exec_result",re_code);
						cJSON_ReplaceItemInObject(cjson_cmd,"type",cJSON_CreateString("reply"));
						re_msg = cJSON_Print(cjson_cmd);
						OneNet_Publish(topic_string,re_msg);
						vPortFree(re_msg);
					}
				}
			}

			else if(strstr(cjson_method->valuestring,"Read_FWVer")!=NULL)
			{
				cJSON *cjson_params= cJSON_GetObjectItem(cjson_cmd,"params");
				if(cjson_params !=NULL){
					unsigned short dev_FWVer = 0; 
					char re_code = 0x00,*re_msg;
					/*控制指令处理函数*/
					re_code = Dev_FwVer_Read(&dev_FWVer);
					//标记数据反馈
					cJSON_AddNumberToObject(cjson_params,"FwVer",dev_FWVer);
					cJSON_AddNumberToObject(cjson_params,"result",re_code);
					cJSON_ReplaceItemInObject(cjson_cmd,"type",cJSON_CreateString("reply"));
					re_msg = cJSON_Print(cjson_cmd);
					OneNet_Publish(topic_string,re_msg);
					vPortFree(re_msg);
				}
			}
			else if(strstr(cjson_method->valuestring,"Chg_DevLoc")!=NULL)
			{
				cJSON *cjson_params= cJSON_GetObjectItem(cjson_cmd,"params");
				if(cjson_params !=NULL){
					cJSON *cjson_chg_devloc = NULL;
					cjson_chg_devloc = cJSON_GetObjectItem(cjson_params,"Dev_Loc");
					if(cjson_chg_devloc != NULL){
						unsigned short devloc_new[2];
						char re_code = 0x00,*re_msg;
						/*控制指令处理函数*/
						devloc_new[0] = cJSON_GetArrayItem(cjson_chg_devloc,0)->valuedouble;
						devloc_new[1] = cJSON_GetArrayItem(cjson_chg_devloc,1)->valuedouble;
						re_code = Dev_Location_Change(devloc_new);	
							//标记数据反馈
						cJSON_AddNumberToObject(cjson_params,"exec_result",re_code);
						cJSON_ReplaceItemInObject(cjson_cmd,"type",cJSON_CreateString("reply"));
						re_msg = cJSON_Print(cjson_cmd);
						OneNet_Publish(topic_string,re_msg);
						vPortFree(re_msg);
					}
				}
			}
			else if(strstr(cjson_method->valuestring,"Read_DevLoc")!=NULL)
			{
				cJSON *cjson_params= cJSON_GetObjectItem(cjson_cmd,"params");
				if(cjson_params !=NULL){
						int dev_loc[2] = {0,0}; 
						char re_code = 0x00,*re_msg;
						/*控制指令处理函数*/
						re_code = Dev_Loc_Read(dev_loc);	
							
						//标记数据反馈
						cJSON* DevLoc_Array = cJSON_CreateIntArray(dev_loc,2);
						cJSON_AddItemToObject(cjson_params,"Dev_Loc",DevLoc_Array);
						cJSON_AddNumberToObject(cjson_params,"result",re_code);
						cJSON_ReplaceItemInObject(cjson_cmd,"type",cJSON_CreateString("reply"));
						re_msg = cJSON_Print(cjson_cmd);
						OneNet_Publish(topic_string,re_msg);
					
						vPortFree(re_msg);
				}
			}
			else if(strstr(cjson_method->valuestring,"Update_Fw")!=NULL)
			{
				;
			}
			else if(strstr(cjson_method->valuestring,"Send_AbnEvtLoc")!=NULL)
			{
				cJSON *cjson_params= cJSON_GetObjectItem(cjson_cmd,"params");
				if(cjson_params !=NULL){
					cJSON *cjson_abnevtloc = NULL;
					cjson_abnevtloc = cJSON_GetObjectItem(cjson_params,"AbnEvt_Loc");
					if(cjson_abnevtloc != NULL){
						unsigned short abnevt_loc[2];
						char re_code = 0x00,*re_msg;
						/*控制指令处理函数*/
						abnevt_loc[0] = cJSON_GetArrayItem(cjson_abnevtloc,0)->valuedouble;
						abnevt_loc[1] = cJSON_GetArrayItem(cjson_abnevtloc,1)->valuedouble;
						re_code = AbneEvt_Dispose(abnevt_loc);
							//标记数据反馈
						cJSON_AddNumberToObject(cjson_params,"exec_result",re_code);
						cJSON_ReplaceItemInObject(cjson_cmd,"type",cJSON_CreateString("reply"));
						re_msg = cJSON_Print(cjson_cmd);
						OneNet_Publish(topic_string,re_msg);
						vPortFree(re_msg);
					}
				}
			}
			else if(strstr(cjson_method->valuestring,"Reboot_Device")!=NULL)
			{
				cJSON *cjson_params= cJSON_GetObjectItem(cjson_cmd,"params");
				if(cjson_params !=NULL){
				
						char re_code = 0x00,*re_msg;
						/*控制指令处理函数*/
						re_code  = 0x01;
							//标记数据反馈
						cJSON_AddNumberToObject(cjson_params,"exec_result",re_code);
						cJSON_ReplaceItemInObject(cjson_cmd,"type",cJSON_CreateString("reply"));
						re_msg = cJSON_Print(cjson_cmd);
						OneNet_Publish(topic_string,re_msg);
						vPortFree(re_msg);
						Device_Reboot();
				}
			}
			vPortFree(topic_string);
		}
	}
	cJSON_Delete(cjson_cmd);
//	NET_DEVICE_ClrData();								//清空缓存
}

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
//图片数据文件
//#include "image_2k.h"

//C库
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
*	函数名称：	OneNet_toString
*
*	函数功能：	将数值转为字符串
*
*	入口参数：	dataStream：数据流
*				buf：转换后的缓存
*				pos：数据流中的哪个数据
*
*	返回参数：	无
*
*	说明：		
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
*	函数名称：	OneNet_Load_DataStream
*
*	函数功能：	数据流封装
*
*	入口参数：	type：发送数据的格式
*				send_buf：发送缓存指针
*				len：发送数据流的个数
*
*	返回参数：	无
*
*	说明：		封装数据流格式
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
		if(dataStream[count].flag) //如果使能发送标志位
		{
			memset(stream_buf, 0, sizeof(stream_buf));

			OneNet_toString(dataStream, data_buf, count);
			snprintf(stream_buf, sizeof(stream_buf), "{\"id\":\"%s\",\"datapoints\":[{\"value\":%s}]},", dataStream[count].name, data_buf);
			
			strncat(send_buf, stream_buf, strlen(stream_buf));
		}
	}
	
	while(*ptr != '\0')					//找到结束符
		ptr++;
	*(--ptr) = '\0';					//将最后的','替换为结束符
	
	strncat(send_buf, "]}", 2);

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
_Bool OneNet_Publish(void)
{

	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//如果网络未连接 或 不为数据收发模式
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
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//如果网络未连接 或 不为数据收发模式
		return 1;
	
	NET_DEVICE_ClrData();
	
	UsartPrintf(USART_DEBUG, "OneNet_Subscribe\r\n");

	if(MqttSample_Subscribe(ctx, topics, 1) == 0)									//可一次订阅多个
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	MqttBuffer_Reset(ctx->mqttbuf);
	
	return 0;

}

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

	if(MqttSample_Unsubscribe(ctx, topics, 2) == 0)									//可一次订阅多个
		Mqtt_SendPkt(ctx->mqttctx, ctx->mqttbuf, 0);
	MqttBuffer_Reset(ctx->mqttbuf);
	
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
				/*控制指令处理函数*/
				Dev_UpLoad_Per_Change((unsigned char) num);
				oneNetInfo.sendData = 1;					//标记数据反馈
			}
			cJSON *cjson_Read_Ver = cJSON_GetObjectItem(cjson_params,"Read_Ver");
			if(cjson_Read_Ver !=NULL){
				/*控制指令处理函数*/
				char cur_ver_str[7] ="";
				unsigned short cur_ver = 0;
				Dev_FwVer_Read(&cur_ver);
			//	sprintf(cur_ver_str,"%2.2f",(float)cur_ver/100);
				oneNetInfo.sendData = 1;					//标记数据反馈
			}
			cJSON *cjson_Send_FW = cJSON_GetObjectItem(cjson_params,"Send_FW");
			if(cjson_Send_FW !=NULL){
				/*控制指令处理函数*/
				
				oneNetInfo.sendData = 1;					//标记数据反馈
			}
			cJSON *cjson_Sync_Time = cJSON_GetObjectItem(cjson_params,"Sync_Time");
			if(cjson_Sync_Time !=NULL){
				/*控制指令处理函数*/
				unsigned int timestamp =cjson_Sync_Time->valueint;
				Dev_Set_Time(timestamp);
				oneNetInfo.sendData = 1;					//标记数据反馈
			}
			cJSON *cjson_Reboot = cJSON_GetObjectItem(cjson_params,"Reboot");
			if(cjson_Reboot !=NULL){
				/*控制指令处理函数*/
				
				oneNetInfo.sendData = 1;					//标记数据反馈
			}
			cJSON *cjson_Chg_Loc = cJSON_GetObjectItem(cjson_params,"Chg_Loc");
			if(cjson_Chg_Loc !=NULL){
				unsigned short new_dev_loc[2] ={0,0};
				int array_size  = cJSON_GetArraySize(cjson_Chg_Loc);
				unsigned char i = 0;
				for(i=0 ; i<array_size ; ++i)
					new_dev_loc[i] = cJSON_GetArrayItem(cjson_Chg_Loc,i)->valueint;
				/*控制指令处理函数*/
				Dev_Location_Change(new_dev_loc);
				oneNetInfo.sendData = 1;					//标记数据反馈
			}
			cJSON *cjson_Send_AbnLoc = cJSON_GetObjectItem(cjson_params,"Send_AbnLoc");
			if(cjson_Send_AbnLoc !=NULL){
				unsigned short abn_loc[2] ={0,0};
				int array_size  = cJSON_GetArraySize(cjson_Send_AbnLoc);
				unsigned char i = 0;
				for(i=0 ; i<array_size ; ++i)
					abn_loc[i] = cJSON_GetArrayItem(cjson_Send_AbnLoc,i)->valueint;
				/*控制指令处理函数*/
				AbneEvt_Dispose(abn_loc);
				oneNetInfo.sendData = 1;					//标记数据反馈
			}
			
			cJSON *cjson_DIR = cJSON_GetObjectItem(cjson_params,"DIR");
			if(cjson_DIR !=NULL){
				/*控制指令处理函数*/
				Direct_Light_Control(cjson_DIR->valueint);
				oneNetInfo.sendData = 1;					//标记数据反馈
			}
		}
		cJSON_Delete(cjson_cmd);
	}
//	NET_DEVICE_ClrData();								//清空缓存
}

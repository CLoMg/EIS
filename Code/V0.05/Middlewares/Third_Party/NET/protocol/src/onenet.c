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

#include "selfcheck.h"
#include "direct.h"
#include "scd4x_i2c.h"
//图片数据文件
//#include "image_2k.h"

//C库
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"

#include "cmsis_os.h"

extern TIM_HandleTypeDef htim2;

struct SendMsg *SendList_Head = NULL;
struct ReSend *ReSend_List_Head = NULL;
struct OneNETSampleContext ctx[1];
//char *topic_str[] = {"100601/ssb_uuid/12333131354838a3/property/down",
//										"100601/ssb_uuid/12333131354838a3/function/down",
//										"100601/ssb_uuid/12333131354838a3/event/down"};

ONETNET_INFO oneNetInfo = {"YCWTRQGPFIDev001", "", "YCWTRQGPFIDev001;12010126;9KYWH", "b17b8b0d043cb6cfd70758cb574407d5ff04aef5bdee31e5d294fd9afda66040;hmacsha256", "175.178.30.200", "1883", 0, 0, 0, 0};
//ONETNET_INFO oneNetInfo = {"YCWTRQGPFIDev001","","YCWTRQGPFIDev001;12010126;M5ADW;1666146085",  "2c9ea89ffc984a4c544872fc9331bc5cea120e6ab4c5daec9956577ed56f9731;hmacsha256","175.178.30.200", "1883", 0, 0, 0, 0};
extern DATA_STREAM dataStream[];
unsigned char wait_reply = 0;

static int OneNET_InitContext(struct OneNETContext *ctx, uint32_t buf_size)
{
    memset(ctx, 0, sizeof(*ctx));

    ctx->bgn = (char*)pvPortMalloc(buf_size);
    if(NULL == ctx->bgn) {
        return OneNETERR_OUTOFMEMORY;
    }

    ctx->end = ctx->bgn + buf_size;
    ctx->pos = ctx->bgn;

    return OneNETERR_NOERROR;
}


int OneNET_Init(struct OneNETSampleContext *ctx)
{
    //struct epoll_event event;
    int err;
		struct SendMsg *p = (struct SendMsg *) pvPortMalloc(sizeof(struct SendMsg)); 
		p->next = NULL;
		SendList_Head = p;
	
		struct ReSend *q = (struct ReSend *) pvPortMalloc(sizeof(struct ReSend)); 
		q->next = NULL;
		ReSend_List_Head = q;
	
    err = OneNET_InitContext(ctx->mqttctx, 400);
    if(OneNETERR_NOERROR != err) {
        UsartPrintf(USART_DEBUG, "Failed to init MQTT context errcode is %d", err);
        return -1;
    }
    //MqttBuffer_Init(ctx->mqttbuf);

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
static unsigned short upload_sn = 0;
_Bool OneNet_SendData(unsigned char len)
{
	unsigned char msg_dir  = 0x03;
	unsigned char light_dir = 0x00;
	unsigned char state = 0xD0;
	unsigned short data_len = 5 * 5 + 1;
	unsigned char index = 0;
	unsigned char *data_buff = (unsigned char *)pvPortMalloc(sizeof(unsigned char)*(data_len));
	wait_reply++;
	if(wait_reply > 5)
//		oneNetInfo.netWork=0;
	if(!oneNetInfo.netWork)															//如果网络未连接
		return 1;
	 
	RTOS_ENTER_CRITICAL();
	//打包属性数据
	light_dir = (unsigned char)Direct_Light_Get();
	state &= 0x3F;
	state |= light_dir << 6;
	data_buff[index++] = state;
	
	memcpy(&data_buff[index],PM_Buff,10);
	index += 10;
	
//	scd_buff[3] = (unsigned char)(upload_sn >> 8);
//	scd_buff[4] = (unsigned char)(upload_sn );
	
	memcpy(&data_buff[index],scd_buff,15);
	
	//发布属性数据
  OneNet_Publish(msg_dir,upload_sn++,Property_Upload, data_len, data_buff);
	vPortFree(data_buff);
	
	RTOS_EXIT_CRITICAL();


	faultTypeReport = FAULT_NONE;													//发送之后清除标记
	
	return 0;

}


_Bool OneNET_FW_Query(void)
{
	unsigned char msg_dir  = 0x03;
	unsigned short data_len = 3;
	unsigned char index = 0;
	unsigned char *data_buff = (unsigned char *)pvPortMalloc(sizeof(unsigned char)*(data_len));
	
	if(!oneNetInfo.netWork)															//如果网络未连接
		return 1;
	 
	RTOS_ENTER_CRITICAL();
	//打包属性数据
	
	data_buff[index++] = (uint8_t)(FW_REC_VER>>8);
	data_buff[index++] = (uint8_t)FW_REC_VER ;
	data_buff[index++] = packsn_query;

	//发布属性数据
  OneNet_Publish(msg_dir,upload_sn++,FW_Query, data_len, data_buff);
	vPortFree(data_buff);
	
	RTOS_EXIT_CRITICAL();


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
unsigned char retry_times = 0,ms_random = 0;
_Bool OneNet_Send_Process(void)
{
	unsigned char send_buff[400];
	unsigned short len = 0;

	struct SendMsg *p,*q;
	if(SendList_Head->next != NULL)
	{
		RTOS_ENTER_CRITICAL();
		p = SendList_Head->next;
		q = p->next;
		p->next = NULL;
		SendList_Head->next = q;
		len = p->len;
		memcpy(send_buff,p->data,len);
		//如果节点类型是新属性上传，则将其插入等待回复列表
		if(p->type == Prop_New)
		{
			struct ReSend *node = (struct ReSend *)pvPortMalloc(sizeof(struct ReSend));
			struct SendMsg *msg = (struct SendMsg*)pvPortMalloc(sizeof(struct SendMsg));
			unsigned char *msgdata = pvPortMalloc(sizeof(unsigned char) * p->len);
			struct ReSend *link = NULL;
			memcpy(msgdata,p->data,p->len);
			msg->data = msgdata;
			msg->len = p->len;
			msg->next = p->next;
			msg->sn = p->sn;
			msg->type = Prop_Old;
			node->msg = msg;
			node->wait_cnt = 0;
			node->next =NULL;
			link = ReSend_List_Head;
			while(link->next != NULL)
				link = link->next;
			link->next = node;
		}
		//如果是指令回复等不需要保存数据的节点，才直接删除

		vPortFree(p->data);
		vPortFree(p);
		RTOS_EXIT_CRITICAL();
		retry_times = 0;
		do{
			ChOcp_flag = 0;
			osDelay(50);
			if(ChOcp_flag == 1){
				srand(__HAL_TIM_GET_COUNTER(&htim2));
				ms_random = rand() % (50);
				osDelay(ms_random);
				retry_times++;
			}
		}while((ChOcp_flag == 1)&&(retry_times < 5));
		if(retry_times < 5)
			NET_DEVICE_SendData(send_buff,len);
		//osDelay(150);
//			}
		memset(send_buff,0,400);
//			sub_sec = Read_MS();
	}
//}
		
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
_Bool OneNet_ReSend(void)
{
	
	struct ReSend *p, *q = NULL;

	p = ReSend_List_Head;
	RTOS_ENTER_CRITICAL();
	while(p->next != NULL){
		p->next->wait_cnt ++;
		//如果到了下一个周期，还没有收到回复，就删除节点
		if(p->next->wait_cnt == Upload_Period){
			q = p->next;
			p->next = q->next;
			q->next = NULL;
			vPortFree(q->msg->data);
			
			vPortFree(q->msg);
			vPortFree(q);
		}
		//否则，每10s将重复数据插入发送链表
		else{
			if(p->next->wait_cnt % 10 == 0){
					struct SendMsg *p1 = SendList_Head; 
					struct SendMsg *q1 =(struct SendMsg *) pvPortMalloc(sizeof(struct SendMsg));
					unsigned char *data = (unsigned char *)pvPortMalloc(sizeof(unsigned char) * p->next->msg->len);
					memcpy(data,p->next->msg->data,p->next->msg->len);
					q1->data = data;
					q1->len = p->next->msg->len;
					q1->next = NULL;
					q1->sn = p->next->msg->sn;
					q1->type = p->next->msg->type;
					while(p1->next != NULL)
								p1 = p1->next;
					p1->next = q1;
			}
			p = p->next;
		}
	}
	RTOS_EXIT_CRITICAL();
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
_Bool OneNet_Publish(unsigned char dir,unsigned short sn ,unsigned short func, unsigned short len, unsigned char *data)
{

	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)		//如果网络未连接 或 不为数据收发模式
		return 1;
	uint16_t check_sum;
	
	NET_DEVICE_ClrData();
	//开辟内存，数据封装
	struct SendMsg *Msg = (struct SendMsg *) pvPortMalloc(sizeof(struct SendMsg));
	unsigned char  *Data = (unsigned char *)pvPortMalloc(sizeof(unsigned char)*len+21);  
	
	Data[0]  = 0x4A;
	Data[1]  = 0x54;
	memcpy(&Data[2],unique_id,8);
	Data[10] = (uint8_t)(sn>>8);
	Data[11] = (uint8_t)sn;
	Data[12] = 0x00;
	Data[13] = ITEM_ID;
	Data[14] = dir;
	Data[15] = (uint8_t)(func>>8);
	Data[16] = (uint8_t)func;
	Data[17] = (uint8_t)(len>>8);
	Data[18] = (uint8_t)len;
	memcpy(&Data[19],data,len);
	
	check_sum = CRC_Calc(Data,len+19);
	
	Data[19+len] = (uint8_t)(check_sum>>8);
	Data[20+len] = (uint8_t)check_sum;
	
	if(func == Property_Upload)
			Msg->type = Prop_New;
	else
			Msg->type = Func_Reply;
	
	Msg->len = len+21;
	Msg->data = Data;
	Msg->sn = sn;
	Msg->next = NULL;

	struct SendMsg *p = SendList_Head; 
	while(p->next != NULL)
		p = p->next;
	p->next = Msg;

	
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

	 
	RTOS_ENTER_CRITICAL();
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
//	vPortFree(topic_string);
//	vPortFree(cjson_send);
//	MqttBuffer_Reset(ctx->mqttbuf);
		RTOS_EXIT_CRITICAL();
	
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

//	cJSON	*cjson_cmd = NULL;
//	cjson_cmd = cJSON_Parse(cmd);

//	if(cjson_cmd == NULL)
//	{
//		UsartPrintf(USART_DEBUG,"Can't find valid cmd\r\n");
//	}
//	else
//	{

//		cJSON *cjson_timestamp = cJSON_GetObjectItem(cjson_cmd,"timestamp");
//		if(cjson_timestamp != NULL)
//		{		
//			double timestamp = strtod(cjson_timestamp->valuestring,NULL);
//			Dev_Set_Time(timestamp/1000);
//			oneNetInfo.syncTime = 1;
//		}
//	}
//	cJSON_Delete(cjson_cmd);
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
void OneNet_App(unsigned short sn,unsigned short func,unsigned short data_len,const char* data)
{
	unsigned short SN = sn;
	unsigned short Func_Code = func;
	unsigned short Len = data_len;
	const char *params = data;
	unsigned char Data_Dir = 0x02;
	switch(Func_Code){
		case Sync_Time:
			{
				long time_stamp = 0;
				unsigned short i = 0;
				unsigned short reply_len = 0;
				unsigned char *reply_data = NULL; 
				for(i = 0;i < data_len;++i)
					time_stamp += (long)params[i] << (8 * (data_len- i -1));
				Dev_Set_Time(time_stamp);
				OneNet_Publish(Data_Dir,SN,Func_Code,reply_len,reply_data);
			}
			break;
			case FWInfo_Push:
			{
				unsigned short  fwnew_ver ,fwnew_size =0;
				unsigned short reply_len = 1;
				unsigned char  reply_data =0 ; 
				
				fwnew_ver = (uint16_t)params[0] << 8 | params[1];
				fwnew_size = (uint16_t)params[2]<< 8 | params[3];
				reply_data = FwInfo_Verify(fwnew_ver,fwnew_size);
				OneNet_Publish(Data_Dir,SN,Func_Code,reply_len,&reply_data);
			}
			break;
			case FW_Query:
			{
				unsigned short  fwnew_ver ,pack_sn ,pack_size =0;

				unsigned short reply_len = 1;
				unsigned char  reply_data =0 ; 
				
				fwnew_ver = (uint16_t)params[0] << 8 | params[1];
				pack_sn   = params[2];
				pack_size = params[3];
				reply_data = FwRec_Verify(fwnew_ver,pack_sn,(unsigned char *)&params[4],pack_size);
				OneNet_Publish(Data_Dir,SN,Func_Code,reply_len,&reply_data);
			}
			break;
			// 平台查询属性
		case Property_Check:
			{
				unsigned char light_dir = 0x00;
				unsigned char state = 0xD0;
				unsigned short reply_len = 5 * 5 + 1;
				unsigned char index = 0;
				unsigned char *reply_data = (unsigned char *)pvPortMalloc(sizeof(unsigned char)*(Len));
	
				 
				RTOS_ENTER_CRITICAL();
				//打包属性数据
				light_dir = (unsigned char)Direct_Light_Get();
				state &= 0x3F;
				state |= light_dir << 6;
				reply_data[index++] = state;
				
				memcpy(&reply_data[index],PM_Buff,10);
				index += 10;
				memcpy(&reply_data[index],scd_buff,15);
				
				OneNet_Publish(Data_Dir,SN,Func_Code,reply_len,reply_data);
				
				vPortFree(reply_data);
				RTOS_EXIT_CRITICAL();
			}
			break;
			//平台接收到到属性数据后回复
		case Property_Upload:
			{
				struct ReSend *p = ReSend_List_Head;
				RTOS_ENTER_CRITICAL();
				while(p->next != NULL){
					//如果在重发链表中找到SN号一致的数据包，则删除该节点
					if(p->next->msg->sn == SN){
						struct ReSend *q = p->next;
						p->next = q->next;
						q->next = NULL;
						vPortFree(q->msg->data);
						vPortFree(q->msg);
						vPortFree(q);
					}
					else
						p=p->next;
				
				}
				RTOS_EXIT_CRITICAL();
			}
			break;
		case UploadPer_Check:
		{		
			unsigned char index = 0;
			unsigned char data_buff[2] = {0,};
			 
			RTOS_ENTER_CRITICAL();
			//打包属性数据
			
			data_buff[0] = (unsigned char)(Upload_Period >> 8); 
			data_buff[1] = (unsigned char) Upload_Period ; 
			index = 2;
			
			unsigned char *payload = (unsigned char *)pvPortMalloc(sizeof(unsigned char)*(index));
			memcpy(payload,data_buff,index);
			
			OneNet_Publish(Data_Dir,SN,Func_Code,index,payload);
			
			vPortFree(payload);
			RTOS_EXIT_CRITICAL();
		}
			break;
		case UploadPer_Set:
		{		
				unsigned short reply_len = 1;
				unsigned char reply_data = 0x00;
				unsigned short upload_per_new;
				/*控制指令处理函数*/
				upload_per_new =	 (uint16_t)params[0] << 8 | params[1];
			
				reply_data = Dev_UpLoad_Per_Change(upload_per_new);
			
				OneNet_Publish(Data_Dir,SN,Func_Code,reply_len,&reply_data);
		}
			break;
		case LightDir_Chg:
		{
				Directions_T dir = all_off;
				unsigned short reply_len = 1;
				unsigned char reply_data = 0x00; 
				dir = (Directions_T)params[0];
				reply_data = Direct_Light_Control(dir);
				if(reply_data == 1) 
					Dev_LightInfo_Write(dir);
				
				OneNet_Publish(Data_Dir,SN,Func_Code,reply_len,&reply_data);
		}
		break;
		case DevLoc_Chg:
		{		
				unsigned short reply_len = 1;
				unsigned char reply_data = 0x00;
				unsigned short devloc_new[2];
				/*控制指令处理函数*/
				devloc_new[0] =  (uint16_t)params[2]<<8 |params[3];
				devloc_new[1] =	 (uint16_t)params[0]<<0 |params[1];
				reply_data = Dev_Location_Change(devloc_new);	
			
				OneNet_Publish(Data_Dir,SN,Func_Code,reply_len,&reply_data);
		}
			break;
		case DevLoc_Check:
		{
				unsigned short reply_len = 4;
				unsigned char reply_data[4] = {0x00,};
				int dev_loc[2] = {0,0}; 
			
				/*控制指令处理函数*/
				Dev_Loc_Read(dev_loc);	
				reply_data[0] = (uint8_t)(dev_loc[0] >> 8);
				reply_data[1] = (uint8_t)(dev_loc[0]);
				reply_data[2] = (uint8_t)(dev_loc[1] >> 8);
				reply_data[3] = (uint8_t)(dev_loc[1]);
				OneNet_Publish(Data_Dir,SN,Func_Code,reply_len,reply_data);
		}
			
			break;
		case AbnEvtLoc_Send:
		{		
				unsigned short reply_len = 1;
				unsigned char reply_data = 0x00;
				unsigned short devloc_new[2];
				/*控制指令处理函数*/
				devloc_new[0] =  (uint16_t)params[2]<<8 | params[3];
				devloc_new[1] =	 (uint16_t)params[0]<<0 | params[1];
				reply_data = AbneEvt_Dispose(devloc_new);	
				OneNet_Publish(Data_Dir,SN,Func_Code,reply_len,&reply_data);
		}
			break;
		case AbnEvt_Rlse:
		{
				unsigned short reply_len = 1;
				unsigned char reply_data = 0x00;
				reply_data = AbneEvt_Release();	
				OneNet_Publish(Data_Dir,SN,Func_Code,reply_len,&reply_data);
		}
			break;
		default:
			break;
	}

//	NET_DEVICE_ClrData();								//清空缓存
}


//接收一帧完整数据，并复制到buf
int OneNETSample_RecvPkt(void *pos,void *buff_begin, uint32_t count)
{
	unsigned char *dataPtr;
	unsigned short bytes;
	
	dataPtr = NET_DEVICE_GetIPD(2);								//等待平台响应
	if(dataPtr != NULL)
	{
		if(netDeviceInfo.ipdBytes > count){
			netDeviceInfo.ipdBytes = count;
//			memcpy(pos, netIOInfo.buf, count);
//			memcpy(buff_begin,&netIOInfo.buf[count],netDeviceInfo.ipdBytes - count);
		}
			memcpy(pos,netIOInfo.buf,netDeviceInfo.ipdBytes);
		NET_DEVICE_ClrData();
	}
	
	bytes = netDeviceInfo.ipdBytes;
	netDeviceInfo.ipdBytes = 0;
	
	return bytes;
}

int OneNET_ReadLength(const char *stream, int size, uint32_t *len)
{
    const uint8_t *in = (const uint8_t*)stream;

    *len = 0;
	//计算数据包长度字段所表示数据体长度
		*len += in[17];
		*len <<= 8;
		*len += in[18];
	//根据计算长度和size做比较，看数据包长度是否足够
	  if(*len < size)
			return 0;
		else
			return -1; // not complete
}


//平台命令下发的处理
int OneNETSample_HandleCmd( const char *cmdid,
													const char *cmdarg,uint32_t cmdarg_len)
{

		unsigned short SN,Func,Data_Len = 0;
		SN = ((uint16_t)cmdarg[0] << 8) | cmdarg[1];
		Func = ((uint16_t)cmdarg[5] << 8) | cmdarg[6];
		Data_Len = cmdarg_len;
		if(memcmp(cmdid,(const char *)unique_id,8) == 0)
			OneNet_App(SN,Func,Data_Len,cmdarg+9);
		
	  //OneNet_Evesdrop(cmd_str);

    return 0;
}

static int OneNET_HandlePublish(struct OneNETContext *ctx, char *funcs,
                              char *pkt, size_t size)
{
		int err = -1;
		const unsigned char item_id = *funcs;
		const char *arg;
		uint32_t arg_len = 0;
		char mac_rec[8] = {0x00,};		
		//系统识别码过滤
		if(item_id == ITEM_ID)
		{
			memcpy(mac_rec,pkt,8);
			//MAC地址过滤
			if(memcmp(mac_rec,(const char *)unique_id,8) == 0)
			{
				 arg = pkt+8;
				 arg_len = size;
				 err = OneNETSample_HandleCmd(mac_rec,arg, arg_len);
			}
		}
	
		
    return err;
}

static int OneNET_Dispatch(struct OneNETContext *ctx, char *func,  char *pkt, size_t size)
{
    const unsigned char flags = *(func+1);

    switch(flags) {
			case 0x01:
				 return OneNET_HandlePublish(ctx, func, pkt, size);
			default:
        break;
    }
    return OneNETERR_ILLEGAL_PKT;
}

int OneNET_RecvPkt(struct OneNETContext *ctx)
{
    int bytes;
    uint32_t remaining_len = 0;
    char *pkt, *cursor;

    bytes = OneNETSample_RecvPkt(ctx->pos, ctx->bgn, ctx->end - ctx->pos+1);
		
		if(bytes == 0)
			return -1;
		ctx->pos += bytes ;
		
		if(ctx->pos > ctx->end)
			ctx->pos = ctx->bgn;
		  
    cursor = ctx->bgn;
    while(1) {
        int errcode;
				//查找帧头00
				while((!((cursor[0] == 0x4A) &&(cursor[1] == 0X54)))&&(cursor < ctx->pos))
					cursor++;
				//如果剩余长度小于最小包长度，则退出
        if(ctx->pos - cursor  < 21) {
            break;
        }
				//计算数据体长度
        bytes = OneNET_ReadLength(cursor , ctx->pos - cursor + 1, &remaining_len);
				
				//如果 剩余包长度小于计算的长度，说明未接收完成，退出
        if(cursor + remaining_len + 21 -1 > ctx->pos) {
            break;
        }
				if(CRC_Calc((unsigned char *)cursor,remaining_len+19) == ((uint16_t)cursor[remaining_len + 19]<<8 | cursor[remaining_len + 20]))
        {
					pkt = cursor + 2;       

					errcode = OneNET_Dispatch(ctx, cursor+13, pkt, remaining_len);
//					if(errcode < 0) {
//							return errcode;
//					}
				}
				cursor +=  21 + remaining_len;
    }

    if(cursor > ctx->bgn) {
        size_t movebytes = cursor - ctx->bgn;
        memmove(ctx->bgn, cursor, movebytes);
        ctx->pos -= movebytes;
        assert(ctx->pos >= ctx->bgn);
    }

    return OneNETERR_NOERROR;
}


uint16_t CRC_Calc(uint8_t *data_buff,uint8_t len){
	
	uint16_t sum=0;
	for(uint8_t count=0;count<len;++count)
		sum+=data_buff[count];
	return sum;
}




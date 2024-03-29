#include "mqtt_app.h"

#include "stm32f1xx.h"

#include "mqtt.h"

#include "net_io.h"
#include "net_device.h"
#include "onenet.h"

#include "usart.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sysparams.h"
#include "FreeRTOS.h"



#define STRLEN 64
char g_cmdid[STRLEN];

struct MqttExtent *Send_List = NULL;
struct MqttExtent *ReSend_List = NULL;





//-------------------------------- Commands ------------------------------------------------------
//封装连接包
int MqttSample_Connect(struct MqttSampleContext *ctx, char *proid\
    , char *auth_info, const char *devid, int keep_alive, int clean_session)
{
    int err;

	UsartPrintf(USART_DEBUG, "product id: %s\r\nsn: %s\r\ndeviceid: %s\r\nkeepalive: %d\r\ncleansession: %d\r\nQoS: %d\r\n", 
		proid, auth_info, devid, keep_alive, clean_session, MQTT_QOS_LEVEL0);
	
	err = Mqtt_PackConnectPkt(ctx->mqttbuf, keep_alive, devid,
                              clean_session, NULL,
                              NULL, 0,
                              MQTT_QOS_LEVEL0, 0, proid,
                              auth_info, strlen(auth_info));

    if(MQTTERR_NOERROR != err) {
        UsartPrintf(USART_DEBUG, "Failed to pack the MQTT CONNECT PACKET, errcode is %d.\n", err);
        return -1;
    }

    return 0;
}

//接收一帧完整数据，并复制到buf
int MqttSample_RecvPkt(void *arg, void *buf, uint32_t count)
{
	unsigned char *dataPtr;
	unsigned short bytes;
	
	dataPtr = NET_DEVICE_GetIPD(2);								//等待平台响应
	if(dataPtr != NULL)
	{
		memcpy(buf, netIOInfo.buf, netDeviceInfo.ipdBytes);
		
		NET_DEVICE_ClrData();
	}
	
	bytes = netDeviceInfo.ipdBytes;
	netDeviceInfo.ipdBytes = 0;
	
	return bytes;
}

//发送一帧数据
int MqttSample_SendPkt(void *arg, const struct iovec *iov, int iovcnt)
{
    char sendbuf[1024];
    short len = 0;
    int bytes;

    int i=0;
    //UsartPrintf(USART_DEBUG, "send one pkt\n");
    for(i=0; i<iovcnt; ++i)
    {
 //       char *pkg = (char*)iov[i].iov_base;
//        for(int j=0; j<iov[i].iov_len; ++j)
//        {
//            UsartPrintf(USART_DEBUG, "%02X ", pkg[j]&0xFF);
//        }
        //UsartPrintf(USART_DEBUG, "\n");
        
        memcpy(sendbuf+len, iov[i].iov_base, iov[i].iov_len);
        len += iov[i].iov_len;
    }
      //这里考虑把数据发送修改为插入链表，然后设备在发送窗口把链表数据统一发送出去
	struct MqttExtent *q = (struct MqttExtent *)pvPortMalloc(sizeof(struct MqttExtent));
	char *payload = (char *)pvPortMalloc(sizeof(char)*len);
	memcpy(payload,sendbuf,len);
	q->len = len;
	q->payload = payload;
	q->next = NULL;
	if(Send_List != NULL)
	{
		struct MqttExtent *p = Send_List; 
		while(p->next != NULL)
			p = p->next;
		p->next = q;
	}
	else
	{
		Send_List = q;
	}
//	NET_DEVICE_SendData((unsigned char *)sendbuf, len);
//    UsartPrintf(USART_DEBUG, "send over\n");

    return bytes;
}

//------------------------------- packet handlers -------------------------------------------
//连接确认---已接入
int MqttSample_HandleConnAck(void *arg, char flags, char ret_code)
{
    UsartPrintf(USART_DEBUG, "Success to connect to the server, flags(%0x), code(%d).\r\n",
           flags, ret_code);
	
	if(flags == 0 && ret_code == 0)
		oneNetInfo.netWork = 1;
	
    return 0;
}

//心跳确认
int MqttSample_HandlePingResp(void *arg)
{
    //UsartPrintf(USART_DEBUG, "Recv the ping response.\r\n");
	oneNetInfo.heartBeat = 1;
    return 0;
}

//收到publish消息
int MqttSample_HandlePublish(void *arg, uint16_t pkt_id, const char *topic,
                                    const char *payload, uint32_t payloadsize,
                                    int dup, enum MqttQosLevel qos)
{

	char cmd_str[300] = {0};

  memcpy(cmd_str, payload, payloadsize);
 // OneNet_App(cmd_str);
	return 0;
}

//publish消息确认
int MqttSample_HandlePubAck(void *arg, uint16_t pkt_id)
{
    UsartPrintf(USART_DEBUG, "Recv the publish ack, packet id is %d.\r\n", pkt_id);
    return 0;
}


//。。。
int MqttSample_HandlePubRec(void *arg, uint16_t pkt_id)
{
    struct MqttSampleContext *ctx = (struct MqttSampleContext*)arg;
    ctx->pkt_to_ack = pkt_id;
    UsartPrintf(USART_DEBUG, "Recv the publish rec, packet id is %d.\r\n", pkt_id);
    return 0;
}

//。。。
int MqttSample_HandlePubRel(void *arg, uint16_t pkt_id)
{
    struct MqttSampleContext *ctx = (struct MqttSampleContext*)arg;
    ctx->pkt_to_ack = pkt_id;
    UsartPrintf(USART_DEBUG, "Recv the publish rel, packet id is %d.\n", pkt_id);
    return 0;
}

int MqttSample_HandlePubComp(void *arg, uint16_t pkt_id)
{
    UsartPrintf(USART_DEBUG, "Recv the publish comp, packet id is %d.\n", pkt_id);
    return 0;
}

//订阅消息确认
int MqttSample_HandleSubAck(void *arg, uint16_t pkt_id, const char *codes, uint32_t count)
{
    uint32_t i;
    UsartPrintf(USART_DEBUG, "Recv the subscribe ack, packet id is %d, return code count is %d:.\r\n", pkt_id, count);
    for(i = 0; i < count; ++i) {
        unsigned int code = ((unsigned char*)codes)[i];
        UsartPrintf(USART_DEBUG,"   code%d=%02x\n", i, code);
    }

    return 0;
}

//取消订阅消息确认
int MqttSample_HandleUnsubAck(void *arg, uint16_t pkt_id)
{
    UsartPrintf(USART_DEBUG, "Recv the unsubscribe ack, packet id is %d.\n", pkt_id);
    return 0;
}

//平台命令下发的处理
int MqttSample_HandleCmd( const char *cmdid,
													const char *cmdarg,uint32_t cmdarg_len)
{

		unsigned short SN,Func,Data_Len = 0;
		SN = ((uint16_t)cmdarg[0] << 8) | cmdarg[1];
		Func = ((uint16_t)cmdarg[5] << 8) | cmdarg[6];
		Data_Len = cmdarg_len;
		if(strcmp(cmdid,(const char *)unique_id) == 0)
			OneNet_App(SN,Func,Data_Len,cmdarg+9);
		
	  //OneNet_Evesdrop(cmd_str);

    return 0;
}

//订阅消息的组包
int MqttSample_Subscribe(struct MqttSampleContext *ctx, char **topic, int num)
{
    int err;

    err = Mqtt_PackSubscribePkt(ctx->mqttbuf, 2, MQTT_QOS_LEVEL1, (const char **)topic, num);
    if(err != MQTTERR_NOERROR) {
       // UsartPrintf(USART_DEBUG, "Critical bug: failed to pack the subscribe packet.\n");
        return -1;
    }

    return 0;
}

//取消订阅的组包
int MqttSample_Unsubscribe(struct MqttSampleContext *ctx, char **topics, int num)
{
    int err;

    err = Mqtt_PackUnsubscribePkt(ctx->mqttbuf, 3, (const char **)topics, num);
    if(err != MQTTERR_NOERROR) {
        UsartPrintf(USART_DEBUG, "Critical bug: failed to pack the unsubscribe packet.\n");
        return -1;
    }

    return 0;
}

//上传数据点的组包
int MqttSample_Savedata(struct MqttSampleContext *ctx, char *t_json)
{
    int err;
	uint16_t pkt_id = 10; 
	
    int payload_len;
    char *t_payload;
    unsigned short json_len;
    
		json_len = strlen(t_json)/sizeof(char);
//    payload_len = 1 + 2 + strlen(t_json)/sizeof(char);
	  payload_len = json_len;
   
    
    t_payload = (char *)pvPortMalloc(payload_len);
    if(t_payload == NULL)
    {
        UsartPrintf(USART_DEBUG, "<%s>: t_payload malloc error\r\n", __FUNCTION__);
        return MQTTERR_INTERNAL;
    }

//    //type
//    t_payload[0] = '\x01';

//    //length
//    t_payload[1] = (json_len & 0xFF00) >> 8;
//    t_payload[2] = json_len & 0xFF;

//	//json
//	memcpy(t_payload+3, t_json, json_len);
		memcpy(t_payload, t_json, json_len);
    if(ctx->mqttbuf->first_ext)
	{
		vPortFree(t_payload);
		UsartPrintf(USART_DEBUG, "<%s>: ctx->mqttbuf->first_ext zero\r\n", __FUNCTION__);
        return MQTTERR_INVALID_PARAMETER;
    }
	
////	UsartPrintf(USART_DEBUG, "Topic: %s\r\nPakect ID: %d\r\nQoS: %d\r\nPayload: %s\r\n", 
////		"$dp", pkt_id, MQTT_QOS_LEVEL1, t_payload);
////	UsartPrintf(USART_DEBUG, "payload_len = %d\r\n", payload_len);
//		char topic_string[40] = "100601/ssb_uuid/%s/property/up";
//		sprintf(topic_string,"100601/ssb_uuid/%s/property/up",mac_strings);
//    err = Mqtt_PackPublishPkt(ctx->mqttbuf, pkt_id, topic_string, t_payload, payload_len, \
//		MQTT_QOS_LEVEL0, 0, 1);
//    
//    vPortFree(t_payload);
		char *topic_string = pvPortMalloc(strlen(CMD_TOPIC_PREFIX)+strlen(mac_strings)+strlen("/property/up"));
		topic_string  = strcpy(topic_string,CMD_TOPIC_PREFIX);
		topic_string = strcat(topic_string,mac_strings);
		topic_string = strcat(topic_string,"/property/up");
    err = Mqtt_PackPublishPkt(ctx->mqttbuf, pkt_id, topic_string, t_payload, payload_len, \
		MQTT_QOS_LEVEL0, 0, 1);
    
    vPortFree(t_payload);
		vPortFree(topic_string);

    if(err != MQTTERR_NOERROR) {
			UsartPrintf(USART_DEBUG, "<%s>: error\r\n", __FUNCTION__);
      return err;
    }

    return 0;
}


//发布消息
int MqttSample_Publish(const char*topic,struct MqttSampleContext *ctx, char *payload)
{
    int err;
    
   // char *topic = "100601/ssb_uuid/12333131354838a3/function/up";
		
    int pkg_id = 1;

    if(ctx->mqttbuf->first_ext) {
        return MQTTERR_INVALID_PARAMETER;
    }

    /*
    std::string pkg_id_s(topics+3);
    int pkg_id = std::stoi(pkg_id_s);
    */
    
    err = Mqtt_PackPublishPkt(ctx->mqttbuf, pkg_id, topic, payload, strlen(payload), MQTT_QOS_LEVEL0, 0, 1);

    if(err != MQTTERR_NOERROR) {
        return err;
    }

    return 0;
}

//回复命令
int MqttSample_RespCmd(struct MqttSampleContext *ctx, char *resp)
{
    int err;
    int Qos=0;

	UsartPrintf(USART_DEBUG, "QoS: %d\r\nCmdId: %s\r\n", Qos, ctx->cmdid);

    if(0==Qos)
    {
        err = Mqtt_PackCmdRetPkt(ctx->mqttbuf, 1, ctx->cmdid,
                                 resp, 11, MQTT_QOS_LEVEL0, 1);
    }
    else if(1==Qos)
    {
        err = Mqtt_PackCmdRetPkt(ctx->mqttbuf, 1, ctx->cmdid,
                                 resp, 11, MQTT_QOS_LEVEL1, 1);
    }

    if(MQTTERR_NOERROR != err) {
        UsartPrintf(USART_DEBUG, "Critical bug: failed to pack the cmd ret packet.\n");
        return -1;
    }

    return 0;
}


int MqttSample_Init(struct MqttSampleContext *ctx)
{
    //struct epoll_event event;
    int err;

    
    ctx->sendedbytes = 0;
    
    ctx->devid = oneNetInfo.devID;
    ctx->cmdid[0] = '\0';

    err = Mqtt_InitContext(ctx->mqttctx, 400);
    if(MQTTERR_NOERROR != err) {
        UsartPrintf(USART_DEBUG, "Failed to init MQTT context errcode is %d", err);
        return -1;
    }

    ctx->mqttctx->read_func = MqttSample_RecvPkt;
//    ctx->mqttctx->read_func_arg =  (void*)(size_t)ctx->mqttfd;
//    ctx->mqttctx->writev_func_arg =  (void*)(size_t)ctx->mqttfd;
    ctx->mqttctx->writev_func = MqttSample_SendPkt;

    ctx->mqttctx->handle_conn_ack = MqttSample_HandleConnAck;
    ctx->mqttctx->handle_conn_ack_arg = ctx;
    ctx->mqttctx->handle_ping_resp = MqttSample_HandlePingResp;
    ctx->mqttctx->handle_ping_resp_arg = ctx;
    ctx->mqttctx->handle_publish = MqttSample_HandlePublish;
    ctx->mqttctx->handle_publish_arg = ctx;
    ctx->mqttctx->handle_pub_ack = MqttSample_HandlePubAck;
    ctx->mqttctx->handle_pub_ack_arg = ctx;
    ctx->mqttctx->handle_pub_rec = MqttSample_HandlePubRec;
    ctx->mqttctx->handle_pub_rec_arg = ctx;
    ctx->mqttctx->handle_pub_rel = MqttSample_HandlePubRel;
    ctx->mqttctx->handle_pub_rel_arg = ctx;
    ctx->mqttctx->handle_pub_comp = MqttSample_HandlePubComp;
    ctx->mqttctx->handle_pub_comp_arg = ctx;
    ctx->mqttctx->handle_sub_ack = MqttSample_HandleSubAck;
    ctx->mqttctx->handle_sub_ack_arg = ctx;
    ctx->mqttctx->handle_unsub_ack = MqttSample_HandleUnsubAck;
    ctx->mqttctx->handle_unsub_ack_arg = ctx;
    ctx->mqttctx->handle_cmd = MqttSample_HandleCmd;
    ctx->mqttctx->handle_cmd_arg = ctx;

    MqttBuffer_Init(ctx->mqttbuf);

    return 0;
}

int MqttSample_ReInit(struct MqttSampleContext *ctx)
{

    MqttBuffer_Init(ctx->mqttbuf);

    return 0;
}

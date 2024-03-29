/*
base on
v2.0 2016/4/19
 */
#include "mqtt.h"
#include "cJSON.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include "onenet.h"

#include "FreeRTOS.h"
#include "usart.h"
#include "sysparams.h"



// range of int: (-2147483648  2147483648), and 1 byte for terminating null byte.
#define MAX_INTBUF_SIZE 12
#define MAX_DBLBUF_SIZE 320

#ifdef WIN32
#define snprintf _snprintf
#endif

static const char Mqtt_TrailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

struct DataPointPktInfo
{
    int16_t tag;
    int16_t subobj_depth;
};

static const int16_t DATA_POINT_PKT_TAG = 0xc19c;

/**
 * 封装发布确认数据包
 * @param buf 存储数据包的缓冲区对象
 * @param pkt_id 被确认的发布数据数据包的ID
 * @return 成功返回MQTTERR_NOERROR
 */
static int Mqtt_PackPubAckPkt(struct MqttBuffer *buf, uint16_t pkt_id);
/**
 * 封装已接收数据包
 * @param buf 存储数据包的缓冲区对象
 * @param pkt_id 被确认的发布数据(Publish)数据包的ID
 * @return 成功返回MQTTERR_NOERROR
 */
static int Mqtt_PackPubRecPkt(struct MqttBuffer *buf, uint16_t pkt_id);
/**
 * 封装发布数据释放数据包
 * @param buf 存储数据包的缓冲区对象
 * @param pkt_id 被确认的已接收数据包(PubRec)的ID
 * @return 成功返回MQTTERR_NOERROR
 */
static int Mqtt_PackPubRelPkt(struct MqttBuffer *buf, uint16_t pkt_id);
/**
 * 封装发布数据完成数据包
 * @param buf 存储数据包的缓冲区对象
 * @param pkt_id 被确认的发布数据释放数据包(PubRel)的ID
 * @return 成功返回MQTTERR_NOERROR
 */
static int Mqtt_PackPubCompPkt(struct MqttBuffer *buf, uint16_t pkt_id);

uint16_t Mqtt_RB16(const char *v)
{
    const uint8_t *uv = (const uint8_t*)v;
    return (((uint16_t)(uv[0])) << 8) | uv[1];
}

uint64_t Mqtt_RB64(const char *v)
{
    const uint8_t *uv = (const uint8_t*)v;
    return ((((uint64_t)(uv[0])) << 56) |
            (((uint64_t)(uv[1])) << 48) |
            (((uint64_t)(uv[2])) << 40) |
            (((uint64_t)(uv[3])) << 32) |
            (((uint64_t)(uv[4])) << 24) |
            (((uint64_t)(uv[5])) << 16) |
            (((uint64_t)(uv[6])) << 8)  |
            uv[7]);

}

void Mqtt_WB16(uint16_t v, char *out)
{
    uint8_t *uo = (uint8_t*)out;
    uo[0] = (uint8_t)(v >> 8);
    uo[1] = (uint8_t)(v);
}

void Mqtt_WB32(uint32_t v, char *out)
{
    uint8_t *uo = (uint8_t*)out;
    uo[0] = (uint8_t)(v >> 24);
    uo[1] = (uint8_t)(v >> 16);
    uo[2] = (uint8_t)(v >> 8);
    uo[3] = (uint8_t)(v);
}

int Mqtt_ReadLength(const char *stream, int size, uint32_t *len)
{
    int i;
    const uint8_t *in = (const uint8_t*)stream;
    uint32_t multiplier = 1;

    *len = 0;
	//计算数据包长度字段所表示数据体长度
		*len += in[16];
		*len <<= 8;
		*len += in[17];
	//根据计算长度和size做比较，看数据包长度是否足够
	  if(*len < size)
			return 0;
		else
			return -1; // not complete
}

int Mqtt_CrcCheck(char *buf, uint32_t len)
{
    int i;
		uint8_t count = 0;
		uint16_t crc_calc=0;
    uint16_t crc_rec = (uint16_t)buf[len]<<8 | buf[len+1];

		for( count=0;count<len;++count)
			crc_calc+=buf[count];
		
		if(crc_calc == crc_rec)
			return 0;
		else
			return -1; 
}


int Mqtt_DumpLength(size_t len, char *buf)
{
    int i;
    for(i = 1; i <= 4; ++i) {
        *((uint8_t*)buf) = len % 128;
        len /= 128;
        if(len > 0) {
            *buf |= 128;
            ++buf;
        }
        else {
            return i;
        }
    }

    return -1;
}

int Mqtt_AppendLength(struct MqttBuffer *buf, uint32_t len)
{
    struct MqttExtent *fix_head = buf->first_ext;
    uint32_t pkt_len;

    assert(fix_head);

    if(Mqtt_ReadLength(fix_head->payload + 1, 4, &pkt_len) < 0) {
        return MQTTERR_INVALID_PARAMETER;
    }

    pkt_len += len;

    fix_head->len = Mqtt_DumpLength(pkt_len, fix_head->payload + 1) + 1;
    if(fix_head->len < 2) {
        return MQTTERR_PKT_TOO_LARGE;
    }

    return MQTTERR_NOERROR;
}

int Mqtt_EraseLength(struct MqttBuffer *buf, uint32_t len)
{
    struct MqttExtent *fix_head = buf->first_ext;
    uint32_t pkt_len;

    assert(fix_head);

    if(Mqtt_ReadLength(fix_head->payload + 1, 4, &pkt_len) < 0) {
        return MQTTERR_INVALID_PARAMETER;
    }

    if(pkt_len < len) {
        // critical bug
        return MQTTERR_INTERNAL;
    }

    pkt_len -= len;
    buf->buffered_bytes -= len;

    fix_head->len = Mqtt_DumpLength(pkt_len, fix_head->payload + 1) + 1;
    assert(fix_head->len >= 2);

    return MQTTERR_NOERROR;
}

void Mqtt_PktWriteString(char **buf, const char *str, uint16_t len)
{
    Mqtt_WB16(len, *buf);
    memcpy(*buf + 2, str, len);
    *buf += 2 + len;
}

int Mqtt_CheckClentIdentifier(const char *id)
{
    int len;
    for(len = 0; '\0' != id[len]; ++len) {
        if(!isalnum(id[len])) {
            return -1;
        }
    }

    return len;
}

static int Mqtt_IsLegalUtf8(const char *first, int len)
{
    unsigned char bv;
    const unsigned char *tail = (const unsigned char *)(first + len);

    switch(len) {
    default:
        return MQTTERR_NOT_UTF8;

    case 4:
        bv = *(--tail);
        if((bv < 0x80) || (bv > 0xBF)) {
            return MQTTERR_NOT_UTF8;
        }
    case 3:
        bv = *(--tail);
        if((bv < 0x80) || (bv > 0xBF)) {
            return MQTTERR_NOT_UTF8;
        }
    case 2:
        bv = *(--tail);
        if((bv < 0x80) || (bv > 0xBF)) {
            return MQTTERR_NOT_UTF8;
        }
        switch(*(const unsigned char *)first) {
        case 0xE0:
            if(bv < 0xA0) {
                return MQTTERR_NOT_UTF8;
            }
            break;

        case 0xED:
            if(bv > 0x9F) {
                return MQTTERR_NOT_UTF8;
            }
            break;

        case 0xF0:
            if(bv < 0x90) {
                return MQTTERR_NOT_UTF8;
            }
            break;

        case 0xF4:
            if(bv > 0x8F) {
                return MQTTERR_NOT_UTF8;
            }
            break;

        default:
            break;
        }
    case 1:
        if(((*first >= 0x80) && (*first < 0xC2)) || (*first > 0xF4)) {
            return MQTTERR_NOT_UTF8;
        }
    }

    return MQTTERR_NOERROR;
}

static int Mqtt_CheckUtf8(const char *str, size_t len)
{
    size_t i;

    for(i = 0; i < len;) {
        int ret;
        char utf8_char_len = Mqtt_TrailingBytesForUTF8[(uint8_t)str[i]] + 1;

        if(i + utf8_char_len > len) {
            return MQTTERR_NOT_UTF8;
        }

        ret = Mqtt_IsLegalUtf8(str, utf8_char_len);
        if(ret != MQTTERR_NOERROR) {
            return ret;
        }

        i += utf8_char_len;
        if('\0'== str[i]) {
            break;
        }
    }

    return (int)i;
}

struct DataPointPktInfo *Mqtt_GetDataPointPktInfo(struct MqttBuffer *buf)
{
    struct MqttExtent *fix_head = buf->first_ext;
    struct MqttExtent *first_payload;
    struct DataPointPktInfo *info;

    if(!fix_head) {
        return NULL;
    }

    if(MQTT_PKT_PUBLISH != (((uint8_t)(fix_head->payload[0])) >> 4)){
        return NULL;
    }

    if(!(fix_head->next) || !(first_payload = fix_head->next->next) ||
       (MQTT_DPTYPE_TRIPLE != first_payload->payload[0])) {
        return NULL;
    }

    if(first_payload->len != 2 + sizeof(struct DataPointPktInfo)) {
        return NULL;
    }

    info = (struct DataPointPktInfo*)(first_payload->payload + 2);
    if(DATA_POINT_PKT_TAG != info->tag) {
        return NULL;
    }

    return info;
}

int Mqtt_HasIllegalCharacter(const char *str, size_t len)
{
    // TODO:
    return 0;
}

int Mqtt_FormatTime(int64_t ts, char *out)
{
    int64_t millisecond = ts % 1000;
    struct tm *t;
    time_t tt = (time_t)(ts) / 1000;
    t = gmtime(&tt);
    if(!t) {
        return 0;
    }

    if(0 == strftime(out, 24, "%Y-%m-%d %H:%M:%S", t)) {
        return 0;
    }

    sprintf(out + 19, ".%03d", (int)millisecond); // 19 bytes for %Y-%m-%dT%H:%M:%S
    return FORMAT_TIME_STRING_SIZE;
}

int Mqtt_HandlePingResp(struct MqttContext *ctx, char flags,
                               char *pkt, size_t size)
{
    if((0 != flags) || (0 != size)) {
        return MQTTERR_ILLEGAL_PKT;
    }

    return ctx->handle_ping_resp(ctx->handle_ping_resp_arg);
}

int Mqtt_HandleConnAck(struct MqttContext *ctx, char flags,
                              char *pkt, size_t size)
{
    char ack_flags, ret_code;

    if((0 != flags) || (2 != size)) {
        return MQTTERR_ILLEGAL_PKT;
    }

    ack_flags = pkt[0];
    ret_code = pkt[1];

    if(((ack_flags & 0x01) && (0 != ret_code)) || (ret_code > 5)) {
        return MQTTERR_ILLEGAL_PKT;
    }

    return ctx->handle_conn_ack(ctx->handle_conn_ack_arg, ack_flags, ret_code);
}

static int Mqtt_HandlePublish(struct MqttContext *ctx, char *funcs,
                              char *pkt, size_t size)
{
		int err = -1;
		const unsigned char item_id = *funcs;
		const char *arg;
		uint32_t arg_len = 0;
		char *mac_rec = pkt; 			
		//系统识别码过滤
		if(item_id == ITEM_ID)
		{
			//MAC地址过滤
			if(strcmp(mac_rec,(const char *)unique_id) == 0)
			{
				 arg = pkt+8;
				 arg_len = size;
				 err = ctx->handle_cmd(mac_rec,arg, arg_len);
			}
		}
	
		
    return err;
}

int Mqtt_HandlePubAck(struct MqttContext *ctx, char flags,
                             char *pkt, size_t size)
{
    uint16_t pkt_id;

    if((0 != flags) || (2 != size)) {
        return MQTTERR_ILLEGAL_PKT;
    }

    pkt_id = Mqtt_RB16(pkt);
    if(0 == pkt_id) {
        return MQTTERR_ILLEGAL_PKT;
    }

    return ctx->handle_pub_ack(ctx->handle_pub_ack_arg, pkt_id);
}

int Mqtt_HandlePubRec(struct MqttContext *ctx, char flags,
                             char *pkt, size_t size)
{
    uint16_t pkt_id;
    int err;

    if((0 != flags) || (2 != size)) {
        return MQTTERR_ILLEGAL_PKT;
    }

    pkt_id = Mqtt_RB16(pkt);
    if(0 == pkt_id) {
        return MQTTERR_ILLEGAL_PKT;
    }

    err = ctx->handle_pub_rec(ctx->handle_pub_rec_arg, pkt_id);
    if(err >= 0) {
        struct MqttBuffer response[1];
        MqttBuffer_Init(response);

        err = Mqtt_PackPubRelPkt(response, pkt_id);
        if(MQTTERR_NOERROR == err) {
            if(Mqtt_SendPkt(ctx, response, 0) != response->buffered_bytes) {
                err = MQTTERR_FAILED_SEND_RESPONSE;
            }
        }

        MqttBuffer_Destroy(response);
    }

    return err;
}

int Mqtt_HandlePubRel(struct MqttContext *ctx, char flags,
                             char *pkt, size_t size)
{
    uint16_t pkt_id;
    int err;

    if((2 != flags) || (2 != size)) {
        return MQTTERR_ILLEGAL_PKT;
    }

    pkt_id = Mqtt_RB16(pkt);
    if(0 == pkt_id) {
        return MQTTERR_ILLEGAL_PKT;
    }

    err = ctx->handle_pub_rel(ctx->handle_pub_rel_arg, pkt_id);
    if(err >= 0) {
        struct MqttBuffer response[1];
        MqttBuffer_Init(response);
        err = Mqtt_PackPubCompPkt(response, pkt_id);
        if(MQTTERR_NOERROR == err) {
            if(Mqtt_SendPkt(ctx, response, 0) != response->buffered_bytes) {
                err = MQTTERR_FAILED_SEND_RESPONSE;
            }
        }
        MqttBuffer_Destroy(response);
    }

    return err;
}

int Mqtt_HandlePubComp(struct MqttContext *ctx, char flags,
                              char *pkt, size_t size)
{
    uint16_t pkt_id;

    if((0 != flags) || (2 != size)) {
        return MQTTERR_ILLEGAL_PKT;
    }

    pkt_id = Mqtt_RB16(pkt);
    if(0 == pkt_id) {
        return MQTTERR_ILLEGAL_PKT;
    }

    return ctx->handle_pub_comp(ctx->handle_pub_comp_arg, pkt_id);
}


int Mqtt_HandleSubAck(struct MqttContext *ctx, char flags,
                             char *pkt, size_t size)
{
    uint16_t pkt_id;
    char *code;

    if((0 != flags) || (size < 2)) {
        return MQTTERR_ILLEGAL_PKT;
    }

    pkt_id = Mqtt_RB16(pkt);
    if(0 == pkt_id) {
        return MQTTERR_ILLEGAL_PKT;
    }

    for(code = pkt + 2; code < pkt + size; ++code ) {
        if(*code & 0x7C) {
            return MQTTERR_ILLEGAL_PKT;
        }
    }

    return ctx->handle_sub_ack(ctx->handle_sub_ack_arg, pkt_id, pkt + 2, size - 2);
}

int Mqtt_HandleUnsubAck(struct MqttContext *ctx, char flags,
                               char *pkt, size_t size)
{
    uint16_t pkt_id;

    if((0 != flags) || (2 != size)) {
        return MQTTERR_ILLEGAL_PKT;
    }

    pkt_id = Mqtt_RB16(pkt);
    if(0 == pkt_id) {
        return MQTTERR_ILLEGAL_PKT;
    }

    return ctx->handle_unsub_ack(ctx->handle_unsub_ack_arg, pkt_id);
}

static int Mqtt_Dispatch(struct MqttContext *ctx, char *func,  char *pkt, size_t size)
{
    const unsigned char flags = *(func+1);

    switch(flags) {
			case 0x01:
				 return Mqtt_HandlePublish(ctx, func, pkt, size);
			default:
        break;
    }
    return MQTTERR_ILLEGAL_PKT;
}

int Mqtt_InitContext(struct MqttContext *ctx, uint32_t buf_size)
{
    memset(ctx, 0, sizeof(*ctx));

    ctx->bgn = (char*)pvPortMalloc(buf_size);
    if(NULL == ctx->bgn) {
        return MQTTERR_OUTOFMEMORY;
    }

    ctx->end = ctx->bgn + buf_size;
    ctx->pos = ctx->bgn;

    return MQTTERR_NOERROR;
}

void Mqtt_DestroyContext(struct MqttContext *ctx)
{
    vPortFree(ctx->bgn);
    memset(ctx, 0, sizeof(*ctx));
}

int Mqtt_RecvPkt(struct MqttContext *ctx)
{
    int bytes;
    uint32_t remaining_len = 0;
    char *pkt, *cursor;

    bytes = ctx->read_func(ctx->read_func_arg, ctx->pos, ctx->end - ctx->pos);

    if(0 == bytes) {
        ctx->pos = ctx->bgn; // clear the buffer
        return MQTTERR_ENDOFFILE;
    }

    if(bytes < 0) {
        return MQTTERR_IO;
    }

    ctx->pos += bytes;

    if(ctx->pos > ctx->end) {
        return MQTTERR_BUF_OVERFLOW;
    }

    cursor = ctx->bgn;
    while(1) {
        int errcode;
				//如果长度小于最小长度，直接退出
        if(ctx->pos - cursor  < 2) {
            break;
        }
				//查找帧头
				while(((*cursor!= 0x4A)||(*(cursor+1)!=0x54))&&(cursor < ctx->pos))
					cursor++;
				
				if(ctx->pos - cursor  < 21)
					break;
				//读取数据体长度
				bytes = Mqtt_ReadLength(cursor, ctx->pos - cursor, &remaining_len);
				
        if(-1 == bytes) {
            break;
        }
        else if(-2 == bytes) {
            return MQTTERR_ILLEGAL_PKT;
        }
				//校验数据包完整性CRC
				bytes = Mqtt_CrcCheck(cursor,19+remaining_len);
				if(-1 == bytes){
					break;
				}

        pkt = cursor + 2;       

        errcode = Mqtt_Dispatch(ctx, cursor+13, pkt, remaining_len);
        if(errcode < 0) {
            return errcode;
        }
		
				cursor += bytes + 1 + remaining_len;
    }

    if(cursor > ctx->bgn) {
        size_t movebytes = cursor - ctx->bgn;
        memmove(ctx->bgn, cursor, movebytes);
        ctx->pos -= movebytes;
        assert(ctx->pos >= ctx->bgn);
    }

    return MQTTERR_NOERROR;
}

int Mqtt_SendPkt(struct MqttContext *ctx, const struct MqttBuffer *buf, uint32_t offset)
{
    const struct MqttExtent *cursor;
    const struct MqttExtent *first_ext;
    uint32_t bytes;
    int ext_count;
    int i;
    struct iovec *iov;

    if(offset >= buf->buffered_bytes) {
        return 0;
    }

    cursor = buf->first_ext;
    bytes = 0;
    while(cursor && bytes < offset) {
        bytes += cursor->len;
        cursor = cursor->next;
    }

    first_ext = cursor;
    ext_count = 0;
    for(; cursor; cursor = cursor->next) {
        ++ext_count;
    }

    if(0 == ext_count) {
        return 0;
    }

    assert(first_ext);

    iov = (struct iovec*)pvPortMalloc(sizeof(struct iovec) * ext_count);
    if(!iov) {
        return MQTTERR_OUTOFMEMORY;
    }

    iov[0].iov_base = first_ext->payload + (offset - bytes);
    iov[0].iov_len = first_ext->len - (offset - bytes);

    i = 1;
    for(cursor = first_ext->next; cursor; cursor = cursor->next) {
        iov[i].iov_base = cursor->payload;
        iov[i].iov_len = cursor->len;
        ++i;
    }

    i = ctx->writev_func(ctx->writev_func_arg, iov, ext_count);
    vPortFree(iov);

    return i;
}



int Mqtt_PackConnectPkt(struct MqttBuffer *buf, uint16_t keep_alive, const char *id,
                        int clean_session, const char *will_topic,
                        const char *will_msg, uint16_t msg_len,
                        enum MqttQosLevel qos, int will_retain, const char *user,
                        const char *password, uint16_t pswd_len)
{
    int ret;
    int16_t id_len, wt_len, user_len;
    size_t total_len;
    char flags = 0;
    struct MqttExtent *fix_head, *variable_head, *payload;
    char *cursor;


    fix_head = MqttBuffer_AllocExtent(buf, 5);
    if(NULL == fix_head) {
        return MQTTERR_OUTOFMEMORY;
    }

    variable_head = MqttBuffer_AllocExtent(buf, 10);
    if(NULL == variable_head) {
        return MQTTERR_OUTOFMEMORY;
    }

    total_len = 10; // length of the variable header
    id_len = Mqtt_CheckClentIdentifier(id);
    if(id_len < 0) {
        return MQTTERR_ILLEGAL_CHARACTER;
    }
    total_len += id_len + 2;

    if(clean_session) {
        flags |= MQTT_CONNECT_CLEAN_SESSION;
    }

    if(will_msg && !will_topic) {
        return MQTTERR_INVALID_PARAMETER;
        }

    wt_len = 0;
    if(will_topic) {
        flags |= MQTT_CONNECT_WILL_FLAG;
        wt_len = strlen(will_topic);
        if(Mqtt_CheckUtf8(will_topic, wt_len) != wt_len) {
            return MQTTERR_NOT_UTF8;
        }
    }

    switch(qos) {
    case MQTT_QOS_LEVEL0:
        flags |= MQTT_CONNECT_WILL_QOS0;
        break;
    case MQTT_QOS_LEVEL1:
        flags |= (MQTT_CONNECT_WILL_FLAG | MQTT_CONNECT_WILL_QOS1);
        break;
    case MQTT_QOS_LEVEL2:
        flags |= (MQTT_CONNECT_WILL_FLAG | MQTT_CONNECT_WILL_QOS2);
        break;
    default:
        return MQTTERR_INVALID_PARAMETER;
    }

    if(will_retain) {
        flags |= (MQTT_CONNECT_WILL_FLAG | MQTT_CONNECT_WILL_RETAIN);
    }

    if(flags & MQTT_CONNECT_WILL_FLAG) {
        total_len += 4 + wt_len + msg_len;
    }

    if(!user && password) {
        return MQTTERR_INVALID_PARAMETER;
    }

    /*must have user + password
     in v2.0
    */
    if(NULL == user ||
        NULL == password){
        return MQTTERR_INVALID_PARAMETER;
    }


    user_len = 0;
    if(user) {
        flags |= MQTT_CONNECT_USER_NAME;
        user_len = strlen(user);
        ret = Mqtt_CheckUtf8(user, user_len);
        if(user_len != ret) {
            return MQTTERR_NOT_UTF8;
        }

        total_len += user_len + 2;
    }

    if(password) {
        flags |= MQTT_CONNECT_PASSORD;
        total_len += pswd_len + 2;
    }



    payload = MqttBuffer_AllocExtent(buf, total_len - 10);
    fix_head->payload[0] = MQTT_PKT_CONNECT << 4;

    ret = Mqtt_DumpLength(total_len, fix_head->payload + 1);
    if(ret < 0) {
        return MQTTERR_PKT_TOO_LARGE;
    }
    fix_head->len = ret + 1; // ajust the length of the extent

    variable_head->payload[0] = 0;
    variable_head->payload[1] = 4;
    variable_head->payload[2] = 'M';
    variable_head->payload[3] = 'Q';
    variable_head->payload[4] = 'T';
    variable_head->payload[5] = 'T';
    variable_head->payload[6] = 4; // protocol level 4
    variable_head->payload[7] = flags;
    Mqtt_WB16(keep_alive, variable_head->payload + 8);

    //write payload client_id
    cursor = payload->payload;
    Mqtt_PktWriteString(&cursor, id, id_len);

    if(flags & MQTT_CONNECT_WILL_FLAG) {
        if(!will_msg) {
            will_msg = "";
            msg_len = 0;
        }

        Mqtt_PktWriteString(&cursor, will_topic, wt_len);
        Mqtt_PktWriteString(&cursor, will_msg, msg_len);
    }

    if(flags & MQTT_CONNECT_USER_NAME) {
        Mqtt_PktWriteString(&cursor, user, user_len);
    }

    if(flags & MQTT_CONNECT_PASSORD) {
        Mqtt_PktWriteString(&cursor, password, pswd_len);
    }

    MqttBuffer_AppendExtent(buf, fix_head);
    MqttBuffer_AppendExtent(buf, variable_head);
    MqttBuffer_AppendExtent(buf, payload);

    return MQTTERR_NOERROR;
}



int Mqtt_PackPublishPkt(struct MqttBuffer *buf, uint16_t pkt_id, uint8_t dir,
                        uint16_t func_code,const char *payload, uint32_t size
                       )
{
    int ret;
    size_t topic_len, total_len;
    struct MqttExtent *fix_head, *variable_head;
    char *cursor;

    if(0 == pkt_id) {
        return MQTTERR_INVALID_PARAMETER;
    }

    for(topic_len = 0; '\0' != topic[topic_len]; ++topic_len) {
        if(('#' == topic[topic_len]) || ('+' == topic[topic_len])) {
            return MQTTERR_INVALID_PARAMETER;
        }
    }

    if(Mqtt_CheckUtf8(topic, topic_len) != topic_len) {
        return MQTTERR_NOT_UTF8;
    }

    fix_head = MqttBuffer_AllocExtent(buf, 5);
    if(NULL == fix_head) {
        return MQTTERR_OUTOFMEMORY;
    }

    fix_head->payload[0] = MQTT_PKT_PUBLISH << 4;

    if(retain) {
        fix_head->payload[0] |= 0x01;
    }

    total_len = topic_len + size + 2;
    switch(qos) {
    case MQTT_QOS_LEVEL0:
        break;
    case MQTT_QOS_LEVEL1:
        fix_head->payload[0] |= 0x02;
        total_len += 2;
        break;
    case MQTT_QOS_LEVEL2:
        fix_head->payload[0] |= 0x04;
        total_len += 2;
        break;
    default:
        return MQTTERR_INVALID_PARAMETER;
    }

    ret = Mqtt_DumpLength(total_len, fix_head->payload + 1);
    if(ret < 0) {
        return MQTTERR_PKT_TOO_LARGE;
    }
    fix_head->len = ret + 1;

    variable_head = MqttBuffer_AllocExtent(buf, total_len - size);
    if(NULL == variable_head) {
        return MQTTERR_OUTOFMEMORY;
    }
    cursor = variable_head->payload;

    Mqtt_PktWriteString(&cursor, topic, topic_len);
    if(MQTT_QOS_LEVEL0 != qos) {
        Mqtt_WB16(pkt_id, cursor);
    }

    MqttBuffer_AppendExtent(buf, fix_head);
    MqttBuffer_AppendExtent(buf, variable_head);
    if(0 != size) {
        MqttBuffer_Append(buf, (char*)payload, size, own);
    }


    return MQTTERR_NOERROR;
}

int Mqtt_SetPktDup(struct MqttBuffer *buf)
{
    struct MqttExtent *fix_head = buf->first_ext;
    uint8_t pkt_type = ((uint8_t)buf->first_ext->payload[0]) >> 4;
    if(!fix_head || (MQTT_PKT_PUBLISH != pkt_type)) {
        return MQTTERR_INVALID_PARAMETER;
    }

    buf->first_ext->payload[0] |= 0x08;
    return MQTTERR_NOERROR;
}

static int Mqtt_PackPubAckPkt(struct MqttBuffer *buf, uint16_t pkt_id)
{
    struct MqttExtent *ext;

    if(0 == pkt_id)  {
        return MQTTERR_INVALID_PARAMETER;
    }

    ext = MqttBuffer_AllocExtent(buf, 4);
    if(!ext) {
        return MQTTERR_OUTOFMEMORY;
    }

    ext->payload[0]= MQTT_PKT_PUBACK << 4;
    ext->payload[1] = 2;
    Mqtt_WB16(pkt_id, ext->payload + 2);
    MqttBuffer_AppendExtent(buf, ext);

    return MQTTERR_NOERROR;
}

static int Mqtt_PackPubRecPkt(struct MqttBuffer *buf, uint16_t pkt_id)
{
    struct MqttExtent *ext;

    if(0 == pkt_id) {
        return MQTTERR_INVALID_PARAMETER;
    }

    ext = MqttBuffer_AllocExtent(buf, 4);
    if(!ext) {
        return MQTTERR_OUTOFMEMORY;
    }

    ext->payload[0]= MQTT_PKT_PUBREC << 4;
    ext->payload[1] = 2;
    Mqtt_WB16(pkt_id, ext->payload + 2);
    MqttBuffer_AppendExtent(buf, ext);

    return MQTTERR_NOERROR;
}

static int Mqtt_PackPubRelPkt(struct MqttBuffer *buf, uint16_t pkt_id)
{
    struct MqttExtent *ext;

    if(0 == pkt_id) {
        return MQTTERR_INVALID_PARAMETER;
    }

    ext = MqttBuffer_AllocExtent(buf, 4);
    ext->payload[0]= MQTT_PKT_PUBREL << 4 | 0x02;
    ext->payload[1] = 2;
    Mqtt_WB16(pkt_id, ext->payload + 2);
    MqttBuffer_AppendExtent(buf, ext);

    return MQTTERR_NOERROR;
}

static int Mqtt_PackPubCompPkt(struct MqttBuffer *buf, uint16_t pkt_id)
{
    struct MqttExtent *ext;

    if(0 == pkt_id) {
        return MQTTERR_INVALID_PARAMETER;
    }

    ext = MqttBuffer_AllocExtent(buf, 4);
    if(!ext) {
        return MQTTERR_OUTOFMEMORY;
    }

    ext->payload[0]= MQTT_PKT_PUBCOMP << 4;
    ext->payload[1] = 2;
    Mqtt_WB16(pkt_id, ext->payload + 2);
    MqttBuffer_AppendExtent(buf, ext);

    return MQTTERR_NOERROR;
}

int Mqtt_PackSubscribePkt(struct MqttBuffer *buf, uint16_t pkt_id,
                          enum MqttQosLevel qos, const char *topics[], int topics_len)
{

    int ret;
    size_t topic_len, remaining_len;
    struct MqttExtent *fixed_head, *ext;
    char *cursor;
    size_t topic_total_len = 0;
    const char *topic;

    if(0 == pkt_id) {
        return MQTTERR_INVALID_PARAMETER;
    }

    int i=0;
    for(i=0; i<topics_len; ++i){
        topic = topics[i];
        if(!topic)
            return MQTTERR_INVALID_PARAMETER;
        topic_len = strlen(topic);
        topic_total_len += topic_len;
        if(Mqtt_CheckUtf8(topic, topic_len) != topic_len) {
            return MQTTERR_NOT_UTF8;
        }
    }

    fixed_head = MqttBuffer_AllocExtent(buf, 5);
    if(NULL == fixed_head) {
        return MQTTERR_OUTOFMEMORY;
    }
    fixed_head->payload[0] = (char)((MQTT_PKT_SUBSCRIBE << 4) | 0x00);

    remaining_len = 2 + 2*(topics_len + 1) + topic_total_len;  // 2 bytes packet id, 2 bytes topic length + topic + 1 byte reserve
    ext = MqttBuffer_AllocExtent(buf, remaining_len);
    if(NULL == ext) {
        return MQTTERR_OUTOFMEMORY;
    }

    ret = Mqtt_DumpLength(remaining_len, fixed_head->payload + 1);
    if(ret < 0) {
        return MQTTERR_PKT_TOO_LARGE;
    }
    fixed_head->len = ret + 1;

    cursor = ext->payload;
    Mqtt_WB16(pkt_id, cursor);
    cursor += 2;

    //write payload
    for(i=0; i<topics_len; ++i){
        topic = topics[i];
        topic_len = strlen(topic);
        Mqtt_PktWriteString(&cursor, topic, topic_len);
        cursor[0] = qos & 0xFF;
        cursor += 1;
    }

    
    MqttBuffer_AppendExtent(buf, fixed_head);
    MqttBuffer_AppendExtent(buf, ext);


    return MQTTERR_NOERROR;
}

int Mqtt_AppendSubscribeTopic(struct MqttBuffer *buf, const char *topic, enum MqttQosLevel qos)
{
    struct MqttExtent *fixed_head = buf->first_ext;
    struct MqttExtent *ext;
    size_t topic_len;
    uint32_t remaining_len;
    char *cursor;
    int ret;
    const char sub_type = (char)(MQTT_PKT_SUBSCRIBE << 4 | 0x02);
    if(!fixed_head || (sub_type != fixed_head->payload[0]) || !topic) {
        return MQTTERR_INVALID_PARAMETER;
    }

    topic_len = strlen(topic);
    ext = MqttBuffer_AllocExtent(buf, topic_len + 3);
    if(!ext) {
        return MQTTERR_OUTOFMEMORY;
    }

    cursor = ext->payload;
    Mqtt_PktWriteString(&cursor, topic, topic_len);
    cursor[0] = qos;

    if(Mqtt_ReadLength(fixed_head->payload + 1, 4, &remaining_len) < 0) {
        return MQTTERR_INVALID_PARAMETER;
    }

    remaining_len += topic_len + 3;
    ret = Mqtt_DumpLength(remaining_len, fixed_head->payload + 1);
    if(ret < 0) {
        return MQTTERR_PKT_TOO_LARGE;
    }

    fixed_head->len = ret + 1;
    MqttBuffer_AppendExtent(buf, ext);
    return MQTTERR_NOERROR;
}

int Mqtt_PackUnsubscribePkt(struct MqttBuffer *buf, uint16_t pkt_id, const char *topics[], int topics_len)
{
    struct MqttExtent *fixed_head, *ext;
    size_t topic_len;
    uint32_t remaining_len;
    char *cursor;
    int ret;
    int topic_total_len = 0;
    int i;
    const char* topic;

    if(0 == pkt_id) {
        return MQTTERR_INVALID_PARAMETER;
    }

    for(i=0; i<topics_len; ++i){
        topic = topics[i];
        if(!topic)
            return MQTTERR_INVALID_PARAMETER;
        topic_len = strlen(topic);
        topic_total_len += topic_len;
        if(Mqtt_CheckUtf8(topic, topic_len) != topic_len) {
            return MQTTERR_NOT_UTF8;
        }
    }

    remaining_len = 2 + 2*topics_len + topic_total_len; // 2 bytes for packet id + 2 bytest topic_len + topic

    fixed_head = MqttBuffer_AllocExtent(buf, 5);
    if(!fixed_head) {
        return MQTTERR_OUTOFMEMORY;
    }

    fixed_head->payload[0] = (char)(MQTT_PKT_UNSUBSCRIBE << 4 | 0x00);
    ret = Mqtt_DumpLength(remaining_len, fixed_head->payload + 1);
    if(ret < 0) {
        return MQTTERR_PKT_TOO_LARGE;
    }
    fixed_head->len = ret + 1;

    ext = MqttBuffer_AllocExtent(buf, remaining_len);
    if(!ext) {
        return MQTTERR_OUTOFMEMORY;
    }

    cursor = ext->payload;
    Mqtt_WB16(pkt_id, cursor);
    cursor += 2;

    //write paylod
    for(i=0; i<topics_len; ++i){
        topic = topics[i];
        topic_len = strlen(topic);
        Mqtt_PktWriteString(&cursor, topic, topic_len);
    }

    MqttBuffer_AppendExtent(buf, fixed_head);
    MqttBuffer_AppendExtent(buf, ext);

    return MQTTERR_NOERROR;
}

int Mqtt_AppendUnsubscribeTopic(struct MqttBuffer *buf, const char *topic)
{
    struct MqttExtent *fixed_head = buf->first_ext;
    struct MqttExtent *ext;
    size_t topic_len;
    uint32_t remaining_len;
    char *cursor;
    int ret;
    const char unsub_type =(char)(MQTT_PKT_UNSUBSCRIBE << 4 | 0x02);
    if(!fixed_head || (unsub_type != fixed_head->payload[0]) || !topic) {
        return MQTTERR_INVALID_PARAMETER;
    }

    topic_len = strlen(topic);
    ext = MqttBuffer_AllocExtent(buf, topic_len + 2);
    if(!ext) {
        return MQTTERR_OUTOFMEMORY;
    }

    cursor = ext->payload;
    Mqtt_PktWriteString(&cursor, topic, topic_len);

    if(Mqtt_ReadLength(fixed_head->payload + 1, 4, &remaining_len) < 0) {
        return MQTTERR_INVALID_PARAMETER;
    }

    remaining_len += topic_len + 2;
    ret = Mqtt_DumpLength(remaining_len, fixed_head->payload + 1);
    if(ret < 0) {
        return MQTTERR_PKT_TOO_LARGE;
    }
    fixed_head->len = ret + 1;

    MqttBuffer_AppendExtent(buf, ext);
    return MQTTERR_NOERROR;
}

int Mqtt_PackPingReqPkt(struct MqttBuffer *buf)
{
    struct MqttExtent *ext = MqttBuffer_AllocExtent(buf, 2);
    if(!ext) {
        return MQTTERR_OUTOFMEMORY;
    }

    ext->payload[0] = (char)(MQTT_PKT_PINGREQ << 4);
    ext->payload[1] = 0;
    MqttBuffer_AppendExtent(buf, ext);

    return MQTTERR_NOERROR;
}

int Mqtt_PackDisconnectPkt(struct MqttBuffer *buf)
{
    struct MqttExtent *ext = MqttBuffer_AllocExtent(buf, 2);
    if(!ext) {
        return MQTTERR_OUTOFMEMORY;
    }

    ext->payload[0] = (char)(MQTT_PKT_DISCONNECT << 4);
    ext->payload[1] = 0;
    MqttBuffer_AppendExtent(buf, ext);

    return MQTTERR_NOERROR;
}

int Mqtt_PackCmdRetPkt(struct MqttBuffer *buf, uint16_t pkt_id, const char *cmdid,
                       const char *ret, uint32_t ret_len,
                       enum MqttQosLevel qos, int own)
{
    size_t cmdid_size = strlen(cmdid) + 1;
    struct MqttExtent *ext = MqttBuffer_AllocExtent(buf, cmdid_size + CMD_TOPIC_PREFIX_LEN);
    if(!ext) {
        return MQTTERR_OUTOFMEMORY;
    }

    memcpy(ext->payload, CMD_TOPIC_PREFIX, CMD_TOPIC_PREFIX_LEN);
    strcpy(ext->payload + CMD_TOPIC_PREFIX_LEN, cmdid);

    return (MQTT_QOS_LEVEL1 == qos)?
        Mqtt_PackPublishPkt(buf, pkt_id, ext->payload, ret, ret_len,
                            MQTT_QOS_LEVEL1, 0, own):
        Mqtt_PackPublishPkt(buf, pkt_id, ext->payload, ret, ret_len,
                            MQTT_QOS_LEVEL0, 0, own);
}

int Mqtt_PackDataPointStart(struct MqttBuffer *buf, uint16_t pkt_id,
                            enum MqttQosLevel qos, int retain, int topic)
{
    int err;
    //struct MqttExtent *ext;

    if(buf->first_ext) {
        return MQTTERR_INVALID_PARAMETER;
    }


    if(topic) {
        err = Mqtt_PackPublishPkt(buf, pkt_id, "$dp", NULL, 0, qos, retain, 0);
    }
    else {
        err = Mqtt_PackPublishPkt(buf, pkt_id, "$crsp/", NULL, 0, qos, retain, 0);
    }

    if(err != MQTTERR_NOERROR) {
        return err;
    }


    return MQTTERR_NOERROR;
}

static int Mqtt_AppendDP(struct MqttBuffer *buf, const char *dsid, int64_t ts,
                         const char *value, size_t value_len, int str)
{
    int err;
    size_t dsid_len, total_len;
    struct MqttExtent *ext;
    struct DataPointPktInfo *info;
    char *cursor;

    info = Mqtt_GetDataPointPktInfo(buf);
    if(!info) {
        return MQTTERR_INVALID_PARAMETER;
    }

    if(!dsid || (0 == (dsid_len = strlen(dsid)))) {
        return MQTTERR_INVALID_PARAMETER;
    }

    if(Mqtt_CheckUtf8(dsid, dsid_len) != dsid_len) {
        return MQTTERR_NOT_UTF8;
    }

    if(Mqtt_HasIllegalCharacter(dsid, dsid_len)) {
        return MQTTERR_ILLEGAL_CHARACTER;
    }

    if(!value) {
        value_len = 4;
    }

    total_len = dsid_len + 9 + (ts > 0 ? FORMAT_TIME_STRING_SIZE : 0) +
        value_len + (str ? 2 : 0);

    ext = MqttBuffer_AllocExtent(buf, (uint32_t)total_len);
    if(!ext) {
        return MQTTERR_OUTOFMEMORY;
    }
    cursor = ext->payload;

    *(cursor++) = '\"';
    memcpy(cursor, dsid, dsid_len);
    cursor += dsid_len;
    *(cursor++) = '\"';
    *(cursor++) = ':';
    *(cursor++) = '{';
    *(cursor++) = '\"';

    if(ts > 0) {
        if(0 == Mqtt_FormatTime(ts, cursor)) {
            return MQTTERR_INTERNAL;
        }
        cursor += FORMAT_TIME_STRING_SIZE;
    }

    *(cursor++) = '\"';
    *(cursor++) = ':';

    if(str) {
        *(cursor++) = '\"';
    }

    if(value) {
        memcpy(cursor, value, value_len);
        cursor += value_len;
    }
    else {
        *(cursor++) = 'n';
        *(cursor++) = 'u';
        *(cursor++) = 'l';
        *(cursor++) = 'l';
    }

    if(str) {
        *(cursor++) = '\"';
    }
    *(cursor++) = '}';
    *(cursor++) = ',';

    if(MQTTERR_NOERROR != (err = Mqtt_AppendLength(buf, ext->len))) {
        return err;
    }

    MqttBuffer_AppendExtent(buf, ext);
    return MQTTERR_NOERROR;
}

int Mqtt_AppendDPNull(struct MqttBuffer *buf, const char *dsid)
{
    return Mqtt_AppendDP(buf, dsid, 0, NULL, 0, 0);
}

int Mqtt_AppendDPInt(struct MqttBuffer *buf, const char *dsid,
                     int64_t ts, int value)
{
    char intbuf[MAX_INTBUF_SIZE];
    size_t bytes = (size_t)snprintf(intbuf, MAX_INTBUF_SIZE, "%d", value);
    return Mqtt_AppendDP(buf, dsid, ts, intbuf, bytes, 0);
}

int Mqtt_AppendDPDouble(struct MqttBuffer *buf, const char *dsid,
                        int64_t ts, double value)
{
    char dblbuf[MAX_DBLBUF_SIZE];
    size_t bytes = (size_t)snprintf(dblbuf, MAX_DBLBUF_SIZE, "%lf", value);
    return Mqtt_AppendDP(buf, dsid, ts, dblbuf, bytes, 0);
}

int Mqtt_AppendDPString(struct MqttBuffer *buf, const char *dsid,
                        int64_t ts, const char *value)
{
    size_t bytes;
    if(!value) {
        value = "";
    }

    bytes = strlen(value);
    if(Mqtt_CheckUtf8(value, bytes) != bytes) {
        return MQTTERR_NOT_UTF8;
    }

    if(Mqtt_HasIllegalCharacter(value, bytes)) {
        return MQTTERR_ILLEGAL_CHARACTER;
    }

    return Mqtt_AppendDP(buf, dsid, ts, value, bytes, 1);
}

int Mqtt_AppendDPStartObject(struct MqttBuffer *buf, const char *dsid, int64_t ts)
{
    int err;
    char strtime[FORMAT_TIME_STRING_SIZE + 1];

    if(MQTTERR_NOERROR != (err = Mqtt_AppendDPStartSubobject(buf, dsid))) {
        return err;
    }

    if(ts > 0) {
        if(0 == Mqtt_FormatTime(ts, strtime)) {
            return MQTTERR_INTERNAL;
        }
        strtime[FORMAT_TIME_STRING_SIZE] ='\0';
    }
    else {
        strtime[0] = '\0';
    }

    return Mqtt_AppendDPStartSubobject(buf, strtime);
}

int Mqtt_AppendDPFinishObject(struct MqttBuffer *buf)
{
    int err;

    if(MQTTERR_NOERROR != (err = Mqtt_AppendDPFinishSubobject(buf))) {
        return err;
    }

    return Mqtt_AppendDPFinishSubobject(buf);
}

static int Mqtt_AppendDPSubvalue(struct MqttBuffer *buf, const char *name,
                                 const char *value, size_t value_len, int str)
{
    int err;
    size_t name_len;
    size_t total_len;
    struct MqttExtent *ext;
    struct DataPointPktInfo *info;
    char *cursor;

    info = Mqtt_GetDataPointPktInfo(buf);
    if(!info) {
        return MQTTERR_INVALID_PARAMETER;
    }

    if(info->subobj_depth <= 0) {
        return MQTTERR_NOT_IN_SUBOBJECT;
    }

    // 1 byte for : and 1 byte for ,
    // if value is string 2 bytes for "" of value
    // format like this: "name":"value",
    total_len = value_len + 2 + (str ? 2 : 0);
    name_len = strlen(name);
    if(Mqtt_CheckUtf8(name, name_len) != name_len) {
        return MQTTERR_NOT_UTF8;
    }

    if(Mqtt_HasIllegalCharacter(name, name_len)) {
        return MQTTERR_ILLEGAL_CHARACTER;
    }
    total_len += name_len + 2;

    ext = MqttBuffer_AllocExtent(buf, (uint32_t)total_len);
    if(!ext) {
        return MQTTERR_OUTOFMEMORY;
    }

    cursor = ext->payload;
    *(cursor++) = '\"';
    memcpy(cursor, name, name_len);
    cursor += name_len;
    *(cursor++) = '\"';
    *(cursor++) = ':';

    if(str) {
        *(cursor++) = '\"';
    }

    memcpy(cursor, value, value_len);
    cursor += value_len;

    if(str) {
        *(cursor++) = '\"';
    }

    *(cursor++) = ',';
    if(MQTTERR_NOERROR != (err = Mqtt_AppendLength(buf, ext->len))) {
        return err;
    }

    MqttBuffer_AppendExtent(buf, ext);
    return MQTTERR_NOERROR;
}

int Mqtt_AppendDPSubvalueInt(struct MqttBuffer *buf, const char *name, int value)
{
    char intbuf[MAX_INTBUF_SIZE];
    size_t bytes = (size_t)snprintf(intbuf, MAX_INTBUF_SIZE, "%d", value);
    return Mqtt_AppendDPSubvalue(buf, name, intbuf, bytes, 0);
}

int Mqtt_AppendDPSubvalueDouble(struct MqttBuffer *buf, const char *name, double value)
{
    char dblbuf[MAX_DBLBUF_SIZE];
    size_t bytes = (size_t)snprintf(dblbuf, MAX_DBLBUF_SIZE, "%lf", value);
    return Mqtt_AppendDPSubvalue(buf, name, dblbuf, bytes, 0);
}

int Mqtt_AppendDPSubvalueString(struct MqttBuffer *buf, const char *name, const char *value)
{
    size_t value_len;

    if(!value) {
        value = "";
    }

    value_len = strlen(value);
    if(Mqtt_CheckUtf8(value, value_len) != value_len) {
        return MQTTERR_NOT_UTF8;
    }

    if(Mqtt_HasIllegalCharacter(value, value_len)) {
        return MQTTERR_ILLEGAL_CHARACTER;
    }

    return Mqtt_AppendDPSubvalue(buf, name, value, value_len, 1);
}

int Mqtt_AppendDPStartSubobject(struct MqttBuffer *buf, const char *name)
{
    int err;
    size_t name_len;
    struct MqttExtent *ext;
    struct DataPointPktInfo *info;
    char *cursor;

    if(!name) {
        name = "";
    }

    name_len = strlen(name);
    if(Mqtt_CheckUtf8(name, name_len) != name_len) {
        return MQTTERR_NOT_UTF8;
    }

    if(Mqtt_HasIllegalCharacter(name, name_len)) {
        return MQTTERR_ILLEGAL_CHARACTER;
    }

    info = Mqtt_GetDataPointPktInfo(buf);
    if(!info) {
        return MQTTERR_INVALID_PARAMETER;
    }

    ++info->subobj_depth;

    // 2 bytes for "" of name, 1 byte for : and 1 byte for {
    ext = MqttBuffer_AllocExtent(buf, name_len + 4);
    if(!ext) {
        return MQTTERR_OUTOFMEMORY;
    }
    cursor = ext->payload;
    *(cursor++) = '\"';
    memcpy(cursor, name, name_len);
    cursor += name_len;
    *(cursor++) = '\"';
    *(cursor++) = ':';
    *(cursor++) = '{';

    if(MQTTERR_NOERROR != (err = Mqtt_AppendLength(buf, ext->len))) {
        return err;
    }

    MqttBuffer_AppendExtent(buf, ext);
    return MQTTERR_NOERROR;
}

int Mqtt_AppendDPFinishSubobject(struct MqttBuffer *buf)
{
    int err;
    struct MqttExtent *ext;
    struct DataPointPktInfo *info;

    info = Mqtt_GetDataPointPktInfo(buf);
    if(!info) {
        return MQTTERR_INVALID_PARAMETER;
    }

    if(--info->subobj_depth < 0) {
        return MQTTERR_NOT_IN_SUBOBJECT;
    }

    if('{' == buf->last_ext->payload[buf->last_ext->len - 1]) {
        ext = MqttBuffer_AllocExtent(buf, 2);
        if(!ext) {
            return MQTTERR_OUTOFMEMORY;
        }

        ext->payload[0] = '}';
        ext->payload[1] = ',';
    }
    else {
        buf->last_ext->payload[buf->last_ext->len - 1] = '}';
        ext = MqttBuffer_AllocExtent(buf, 1);
        if(!ext) {
            return MQTTERR_OUTOFMEMORY;
        }
        ext->payload[0] = ',';
    }

    if(MQTTERR_NOERROR != (err = Mqtt_AppendLength(buf, ext->len))) {
        return err;
    }

    MqttBuffer_AppendExtent(buf, ext);
    return MQTTERR_NOERROR;
}

int Mqtt_PackDataPointFinish(struct MqttBuffer *buf)
{
    struct DataPointPktInfo *info;
    struct MqttExtent *first_payload;
    int err;

    info = Mqtt_GetDataPointPktInfo(buf);
    if(!info) {
        return MQTTERR_INVALID_PARAMETER;
    }

    if(info->subobj_depth > 0) {
        return MQTTERR_INCOMPLETE_SUBOBJECT;
    }

    first_payload = buf->first_ext->next->next;
    first_payload->len = 2;

    if(MQTTERR_NOERROR != (err = Mqtt_EraseLength(buf, sizeof(struct DataPointPktInfo)))) {
        return err;
    }

    if(buf->last_ext != first_payload) {
        buf->last_ext->payload[buf->last_ext->len - 1] = '}';
    }
    else {
        struct MqttExtent *end_ext = MqttBuffer_AllocExtent(buf, 1);
        if(!end_ext) {
            return MQTTERR_OUTOFMEMORY;
        }

        end_ext->payload[0] = '}';
        MqttBuffer_AppendExtent(buf, end_ext);
        err = Mqtt_AppendLength(buf, end_ext->len);
        if(MQTTERR_NOERROR != err) {
            return err;
        }
    }

    return MQTTERR_NOERROR;
}


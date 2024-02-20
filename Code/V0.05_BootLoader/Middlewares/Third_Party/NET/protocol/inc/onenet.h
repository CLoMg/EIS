#ifndef _ONENET_H_
#define _ONENET_H_


typedef struct
{

    char devID[16];
    char apiKey[35];
	
	char proID[43];//产品ID(Product ID,PID;早期亦称为:项目ID)
	char auif[76];//鉴权信息
	
	char ip[16];
	char port[8];
	
	unsigned char netWork : 1;		//0-局网模式(AP)		1-互联网模式(STA)
	unsigned char syncTime: 1; //时间同步标识 0：未同步 1：同步
	unsigned char sendData : 3;		//数据反馈
	unsigned char errCount : 3;		//错误计数
	unsigned char heartBeat : 1;

} ONETNET_INFO;

extern ONETNET_INFO oneNetInfo;

typedef enum
{

	TYPE_BOOL = 0,
	
	TYPE_CHAR,
	TYPE_UCHAR,
	
	TYPE_SHORT,
	TYPE_USHORT,
	
	TYPE_INT,
	TYPE_UINT,
	
	TYPE_LONG,
	TYPE_ULONG,
	
	TYPE_FLOAT,
	TYPE_DOUBLE,
	
	TYPE_GPS,

} DATA_TYPE;

typedef enum
{
	Sync_Time = 0x0004,
	Property_Check = 0x1033,
	Property_Upload = 0x1034,
	UploadPer_Check = 0x1035,
	UploadPer_Set   = 0x1036,
	LightDir_Chg = 0x1048,
	LightDir_Check = 0x1049,
	DevLoc_Chg = 0x1050,
	DevLoc_Check = 0x1051,
	AbnEvtLoc_Send = 0x1054,
	AbnEvt_Rlse = 0x1055,
} FUNC_TYPE;

typedef struct
{
	char *name;
	void *data;
	DATA_TYPE dataType;
	_Bool flag;

} DATA_STREAM;


/** OneNET 运行时上下文 */
struct OneNETContext {
    char *bgn;
    char *end;
    char *pos;
};
/** OneNET错误码 */
enum OneNETError {
    OneNETERR_NOERROR                  = 0,  /**< 成功，无错误*/
    OneNETERR_OUTOFMEMORY              = -1, /**< 内存不足 */
    OneNETERR_ENDOFFILE                = -2, /**< 读数据失败，已到文件结尾*/
		OneNETERR_IO                       = -3, /**< I/O错误 */
    OneNETERR_ILLEGAL_PKT              = -4, /**< 非法的数据包 */
    OneNETERR_ILLEGAL_CHARACTER        = -5, /**< 非法的字符 */
    OneNETERR_NOT_UTF8                 = -6, /**< 字符编码不是UTF-8 */
    OneNETERR_INVALID_PARAMETER        = -7, /**< 参数错误 */
    OneNETERR_PKT_TOO_LARGE            = -8, /**< 数据包过大 */
    OneNETERR_BUF_OVERFLOW             = -9, /**< 缓冲区溢出 */
    OneNETERR_EMPTY_CALLBACK           = -10,/**< 回调函数为空 */
    OneNETERR_INTERNAL                 = -11,/**< 系统内部错误 */
    OneNETERR_NOT_IN_SUBOBJECT         = -12,/**< 调用OneNET_AppendDPFinishObject，但没有匹配的Mqtt_AppendDPStartObject */
    OneNETERR_INCOMPLETE_SUBOBJECT     = -13,/**< 调用Mqtt_PackDataPointFinish时，包含的子数据结构不完整 */
    OneNETERR_FAILED_SEND_RESPONSE     = -14 /**< 处理publish系列消息后，发送响应包失败 */
};


struct OneNETSampleContext
{
    struct OneNETContext mqttctx[1];
   // struct OneNETBuffer mqttbuf[1];

    //uint16_t pkt_to_ack;
};

struct SensorInfo{
	unsigned char id;
	unsigned int data;
};

typedef struct Property
{
	unsigned char status;
	struct SensorInfo PM25;
	struct SensorInfo PM10;
	struct SensorInfo CO2;
	struct SensorInfo TEMP;
	struct SensorInfo HUMI;
}Property_T;

typedef enum
{
	Prop_New = 0x01,
	Prop_Old = 0x02,
	Func_Reply = 0x03,
} MSG_TYPE;

struct SendMsg{
	MSG_TYPE type;
	unsigned short len;
	unsigned short sn;
	unsigned char *data;
	struct SendMsg *next;
};

struct ReSend {
		unsigned char wait_cnt;
		struct SendMsg *msg;
    struct ReSend *next;
};
extern struct OneNETSampleContext ctx[1];

#define SEND_BUF_SIZE  256
#define CHECK_CONNECTED			0	//已连接
#define CHECK_CLOSED			1	//已断开
#define CHECK_GOT_IP			2	//已获取到IP
#define CHECK_NO_DEVICE			3	//无设备
#define CHECK_INITIAL			4	//初始化状态
#define CHECK_NO_CARD			5	//没有sim卡
#define CHECK_NO_ERR			255 //

#define DEVICE_CMD_MODE			0 	//AT命令模式
#define DEVICE_DATA_MODE		1 	//平台命令下发模式
#define DEVICE_HEART_MODE		2 	//心跳连接模式





void OneNet_DevLink(const char* devid, const char* auth_key);

void OneNet_DevDisLink(void);

_Bool OneNet_SendData(unsigned char len);

_Bool OneNet_Publish(unsigned char dir,unsigned short sn ,unsigned short func, unsigned short len, unsigned char *data);

_Bool OneNet_Subscribe(void);

_Bool OneNet_UnSubscribe(void);

_Bool OneNet_HeartBeat(void);

_Bool OneNet_OnLine(void);

void OneNet_App(unsigned short sn,unsigned short func,unsigned short data_len,const char* data);

_Bool OneNet_Send_Process(void);

void OneNet_Evesdrop(char *cmd);

int OneNET_Init(struct OneNETSampleContext *ctx);

int OneNET_RecvPkt(struct OneNETContext *ctx);

unsigned short CRC_Calc(unsigned char *data_buff,unsigned char len);

_Bool OneNet_ReSend(void);

#endif

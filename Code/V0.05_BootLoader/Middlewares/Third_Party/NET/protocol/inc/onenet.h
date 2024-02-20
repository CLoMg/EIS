#ifndef _ONENET_H_
#define _ONENET_H_


typedef struct
{

    char devID[16];
    char apiKey[35];
	
	char proID[43];//��ƷID(Product ID,PID;�������Ϊ:��ĿID)
	char auif[76];//��Ȩ��Ϣ
	
	char ip[16];
	char port[8];
	
	unsigned char netWork : 1;		//0-����ģʽ(AP)		1-������ģʽ(STA)
	unsigned char syncTime: 1; //ʱ��ͬ����ʶ 0��δͬ�� 1��ͬ��
	unsigned char sendData : 3;		//���ݷ���
	unsigned char errCount : 3;		//�������
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


/** OneNET ����ʱ������ */
struct OneNETContext {
    char *bgn;
    char *end;
    char *pos;
};
/** OneNET������ */
enum OneNETError {
    OneNETERR_NOERROR                  = 0,  /**< �ɹ����޴���*/
    OneNETERR_OUTOFMEMORY              = -1, /**< �ڴ治�� */
    OneNETERR_ENDOFFILE                = -2, /**< ������ʧ�ܣ��ѵ��ļ���β*/
		OneNETERR_IO                       = -3, /**< I/O���� */
    OneNETERR_ILLEGAL_PKT              = -4, /**< �Ƿ������ݰ� */
    OneNETERR_ILLEGAL_CHARACTER        = -5, /**< �Ƿ����ַ� */
    OneNETERR_NOT_UTF8                 = -6, /**< �ַ����벻��UTF-8 */
    OneNETERR_INVALID_PARAMETER        = -7, /**< �������� */
    OneNETERR_PKT_TOO_LARGE            = -8, /**< ���ݰ����� */
    OneNETERR_BUF_OVERFLOW             = -9, /**< ��������� */
    OneNETERR_EMPTY_CALLBACK           = -10,/**< �ص�����Ϊ�� */
    OneNETERR_INTERNAL                 = -11,/**< ϵͳ�ڲ����� */
    OneNETERR_NOT_IN_SUBOBJECT         = -12,/**< ����OneNET_AppendDPFinishObject����û��ƥ���Mqtt_AppendDPStartObject */
    OneNETERR_INCOMPLETE_SUBOBJECT     = -13,/**< ����Mqtt_PackDataPointFinishʱ�������������ݽṹ������ */
    OneNETERR_FAILED_SEND_RESPONSE     = -14 /**< ����publishϵ����Ϣ�󣬷�����Ӧ��ʧ�� */
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
#define CHECK_CONNECTED			0	//������
#define CHECK_CLOSED			1	//�ѶϿ�
#define CHECK_GOT_IP			2	//�ѻ�ȡ��IP
#define CHECK_NO_DEVICE			3	//���豸
#define CHECK_INITIAL			4	//��ʼ��״̬
#define CHECK_NO_CARD			5	//û��sim��
#define CHECK_NO_ERR			255 //

#define DEVICE_CMD_MODE			0 	//AT����ģʽ
#define DEVICE_DATA_MODE		1 	//ƽ̨�����·�ģʽ
#define DEVICE_HEART_MODE		2 	//��������ģʽ





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

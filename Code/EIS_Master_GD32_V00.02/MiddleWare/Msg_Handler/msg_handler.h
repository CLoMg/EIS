#ifndef MSG_HANDLER_H
#define MSG_HANDLER_H

#include "freertos.h"
#include "semphr.h"
#define VIP_MSG_MAX 0x1000
#define ITEM_NUM 0x0F
extern SemaphoreHandle_t xStateMutex;
extern unsigned char Sensor_Buff[20];
	
typedef enum xEvent{
	LORARxEvent = 0x00, LORATxEvent, ParaEvent,EventType_Eof
}xEvent_T;	
typedef struct
{
	xEvent_T iEventType;
	unsigned short iMeaning;
	unsigned short sn;
	unsigned char *iValue;
	unsigned char len;
}xData;

typedef enum type_id{
	Undef = 0x00, Lora = 0x01, BT, States, Slave, Direction, Type_id_Eof
}type_id_t;

/*设备自身状态和传感器数据结构体*/
typedef struct Dev_Status_Data{
	unsigned char status;
	unsigned char data_len;
	unsigned char *data_addr;
	struct Dev_Status_Data* next;
}Dev_Status_Data_T;
/*设备内部消息传递的数据结构*/
typedef struct Internal_MSG{
	type_id_t id;
	unsigned int sn;
	unsigned int func_code;
	unsigned char len;
	unsigned char *data_addr;
	struct Internal_MSG *next ;
}Internal_MSG_T;

typedef enum func_code{
	Factory_Rst = 0x0001, Reboot ,Soft_Ver, Net_Time_Get, FW_Update=0x1030, FW_Acquire, FW_Replace, Data_Query, Data_Upload = 0x1034, 
	Upload_Int, LoraChip_Para_Push = 0x103E, LoraChip_Para_Repalce, LoraChip_Para_Query, DataComp_Get, DataComp_Issue, DataAcq_Int_Query, DataAcq_Int_Issue,
	Light_Dir_Issue = 0x1048, Light_Dir_Query, DevPosi_Issue  = 0x1050, DevPosi_Query, LoraModule_Para_Set, LoraModule_Para_Query,Abnormal_Posi_Issue,Abnormal_Event_CLR,Light_State_Refresh
}func_code_t;
typedef struct message{
	unsigned char id;
	func_code_t func;
	char *name;
	char *addr;
	char length;
	char *data;
	struct message *next;
} *message_t;

extern  Internal_MSG_T *States_Msg_Head ;
extern Internal_MSG_T *Int_Msg_Head ;
extern Internal_MSG_T *Lora_InMsg_Head;
extern Internal_MSG_T *InMsg_Lora_Head;
extern Internal_MSG_T *IAP_Msg_Head;
extern Dev_Status_Data_T Dev_Status_Struct;

void MSG_FSM_Init(void);
void Data_Upload_Func(void);
void Timeout_Clr_Func(void);
void Retry_Func(void);
void Log_Func(void);
void MSG_Init_Func(void);

void MsgLink_Init(void);
void MsgLink_Insert(message_t new);
message_t Msg_Link_Insert_to_Tail(message_t head,message_t node);
message_t Msg_Link_Insert_to_Head(message_t link_head,message_t node);
unsigned char Msg_Dispose(void *msg_out);
void LORA_InMSG_Handler(Internal_MSG_T *p);
void MSG_Handler_Func(void);
void MSG_FSM_RUN_Func(void);
void LORACTL_FMS_RUN(void);
#endif

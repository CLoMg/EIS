#include "iap.h"
#include "fsm.h"
#include "string.h"
#include "stdlib.h"
#include "msg_handler.h"

#define FW_VERSION 0x0001
FSM_T *IAP_FSM = NULL;
static EventsID_T event;

enum Status
{
	UnInit_State = 0x00, Idle_State, FW_Acq_State, Wait_Reply_State,
};

typedef enum  
{
	No_Event = 0x00, Init_Event , FW_Upload_Event, FW_Acq_Event,
	RX_Sucess_Event, RX_Timeout_Event, FW_Acq_Finish_Event,
}IAP_Events_T;



static StateTransform_T trans_table[] = 
{
	{UnInit_State,Init_Event,Idle_State,IAP_Init_Func},
	{Idle_State,FW_Upload_Event,FW_Acq_State,_IAP_Idle_Func},
	{FW_Acq_State,FW_Acq_Event,Wait_Reply_State,FW_Acq_Func},
	{FW_Acq_State,FW_Acq_Finish_Event,Idle_State,App_Jump_Func},
	{Wait_Reply_State,RX_Timeout_Event,FW_Acq_State,FW_ReAcq_Func},
	{Wait_Reply_State,RX_Sucess_Event,FW_Acq_State,FW_RX_Handler_Func},
};

IAP_Info_Struct_T *IAP_Info_Struct = NULL;
void _IAP_Idle_Func(void){
	
}

void IAP_Init_Func(void){
	IAP_Info_Struct = IAP_Info_Struct_Init();
	IAP_Msg_LinkList_Init();
	IAP_FSM_Init();
}
void IAP_Msg_LinkList_Init(void){
	Internal_MSG_T *p = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T));
	p->next = NULL;
	IAP_Msg_Head = p;
}

IAP_Info_Struct_T* IAP_Info_Struct_Init(void){
	IAP_Info_Struct_T *iap_info_instance = (IAP_Info_Struct_T *)malloc(sizeof(IAP_Info_Struct_T));
	iap_info_instance->fw_version = FW_VERSION;
	iap_info_instance->fw_pack_total = 0;
	iap_info_instance->fw_pack_cur = 0;
	iap_info_instance->fw_pack_size_cur = 0;
	iap_info_instance->fw_pack_size_single = 0;
	return  iap_info_instance;
}
void IAP_FSM_Init(void)
{
	IAP_FSM = (FSM_T*)malloc(sizeof(FSM_T));
	FSM_Regist(IAP_FSM,trans_table);
	FSM_StateTransfer(IAP_FSM,UnInit_State);
	IAP_FSM->transform_num = 6;
	event = Init_Event;
}


void FW_Acq_Func(void){
	Internal_MSG_T *IAP_TX_MSG = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T));
	
	IAP_TX_MSG->data_addr = (unsigned char*)malloc(5*sizeof(unsigned char));
	IAP_TX_MSG->func_code = FW_Acquire;
	IAP_TX_MSG->id = Lora;
	IAP_TX_MSG->len = 0x05;
	IAP_TX_MSG->data_addr[0] = 0x00;
	IAP_TX_MSG->data_addr[1] = IAP_Info_Struct->fw_version;
	IAP_TX_MSG->data_addr[2] = 0x00;
	IAP_TX_MSG->data_addr[3] = IAP_Info_Struct->fw_pack_cur;
	IAP_TX_MSG->data_addr[4] = IAP_Info_Struct->fw_pack_size_single;
	
	Internal_MSG_T * q =  Int_Msg_Head;
	while(q->next != NULL)
				q = q->next;
	q->next = IAP_TX_MSG;
	
}
void App_Jump_Func(void){}
void FW_ReAcq_Func(void){}
void FW_RX_Handler_Func(void){}
	
void IAP_MSG_Handler(void){
		if(IAP_Msg_Head->next != NULL){
		Internal_MSG_T *p = IAP_Msg_Head->next;
		IAP_Msg_Head->next = p->next;
		switch(p->func_code){
			case  FW_Update:{
				Internal_MSG_T *reply = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T));
				reply->data_addr = (unsigned char *)malloc(3*sizeof(unsigned char));
				unsigned char new_fw_version = p->data_addr[1];
				unsigned int  fw_size = ((unsigned int)p->data_addr[2]<<24) | ((unsigned int)p->data_addr[3]<<16) |
																((unsigned int)p->data_addr[4]<<8)  | ((unsigned int)p->data_addr[5]);
				
				reply->data_addr[0] = p->data_addr[0];
				reply->data_addr[1] = p->data_addr[1];
				reply->data_addr[3] = IAP_Info_Struct->fw_pack_size_single;
				reply->func_code  = p->func_code;
				reply->id = Lora;
				reply->sn = p->sn;
				reply->len = 0x03;
				reply->next = NULL;
				Internal_MSG_T * q =  Int_Msg_Head;
				while(q->next != NULL)
					q = q->next;
				q->next = reply;
				
				free(p->data_addr);
				p->data_addr = NULL;
				free(p);
				p = NULL;
			}
			break;
			case FW_Acquire:{
				IAP_Info_Struct->fw_pack_cur = (unsigned short)p->data_addr[2]<<8 | p->data_addr[3];
				IAP_Info_Struct->fw_pack_size_cur = p->len - 4;
				IAP_Info_Struct->fw_addr = p->data_addr;
				
				event = RX_Sucess_Event;
				free(p);
				p = NULL;
			}
			break;
		}	
	}
}
void IAP_FSM_RUN(void){
	FSM_EventHandle(IAP_FSM,event);
}

void IAP_Process(void){
}
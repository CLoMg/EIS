#include "msg_handler.h"
#include "stdlib.h"
#include "string.h"
#include "direct.h"
#include "fsm.h"
message_t Msg_Head = NULL;
Internal_MSG_T *Int_Msg_Head = NULL;
Internal_MSG_T *States_Msg_Head = NULL;
Internal_MSG_T *Lora_InMsg_Head = NULL;
Internal_MSG_T *InMsg_Lora_Head = NULL;
Internal_MSG_T *IAP_Msg_Head = NULL;
Dev_Status_Data_T Dev_Status_Struct;

FSM_T *MSG_FSM = NULL;
static EventsID_T event;

enum Status
{
	UnInit = 0x00, Idle, Wait_Reply, DisConnect,
};

typedef enum  
{
	No_Event = 0x00, Init_Event , Data_Upload_Event, Reply_Rec_Event,
	Timeout_Event, Wait_Failed_Event, ReCnt_Event,DisCnt_Event,
}Events_T;

void Idle_Func(void){
	
};
static StateTransform_T trans_table[] = 
{
	{UnInit,Init_Event,Idle,MSG_Init_Func},
	{Idle,No_Event,Idle,Idle_Func},
	{Idle,Data_Upload_Event,Wait_Reply,Data_Upload_Func},
	{Wait_Reply,Reply_Rec_Event,Idle,Timeout_Clr_Func},
	{Wait_Reply,No_Event,Wait_Reply,Retry_Func},
	{Wait_Reply,Wait_Failed_Event,Idle,Log_Func},
};

/*!
    \brief     	Initialize the Massage Handle FSM
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MSG_FSM_Init(void)
{
	MSG_FSM = (FSM_T*)malloc(sizeof(FSM_T));
	FSM_Regist(MSG_FSM,trans_table);
	FSM_StateTransfer(MSG_FSM,UnInit);
	MSG_FSM->transform_num = 8;
	event = Init_Event;
//	FSM_EventHandle(LED_FSM,event);
}

static unsigned short upload_sn = 0;
Internal_MSG_T pre_data; //开辟空间，存储上一次上传数据
void Data_Upload_Func(void){
			Internal_MSG_T *status_data = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T)) ,*q = Int_Msg_Head;
			status_data->id = Lora;
			status_data->len = Dev_Status_Struct.data_len;
			status_data->sn = upload_sn++;
			status_data->func_code = Data_Upload;
			status_data->data_addr = Dev_Status_Struct.data_addr;
			status_data->next = NULL;
			pre_data = *status_data;
			while(q->next != NULL)
				q = q->next;
			status_data->next = q->next;
			q->next = status_data;
			
			event  = No_Event;
}

static unsigned char retry_count = 0; 
void Retry_Func(void){
	retry_count++;
	/*重发三次未收到回复，促发等待超时事件，进入断连状态*/
	if(retry_count >30)
		event = Wait_Failed_Event;
	else{
		/*超过1s未收到回复，则重发一次*/
		if(retry_count % 10 == 0){
			Internal_MSG_T *status_data = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T)) ,*q = Int_Msg_Head;
			*status_data = pre_data;
			while(q->next != NULL)
				q = q->next;
			status_data->next = q->next;
			q->next = status_data;
		}
	}
}

void Timeout_Clr_Func(void){
	retry_count = 0;
}


void Log_Func(void){
	retry_count = 0;
}
/*!
    \brief     	Initialize related Module of Message Handler
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MSG_Init_Func(void){
	MsgLink_Init();
	event = No_Event;
}

/*!
    \brief      Insert new message to the message link...
                according to the priority of function code 
	              if function code > VIP_MSG_MAX  Insert the messge
		            to tail, else to head. 
    \param[in]  new message
    \param[out] none
    \retval     none
*/
void MsgLink_Init(void){
	Int_Msg_Head = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T));
	Int_Msg_Head->next = NULL;
}
/*!
    \brief      Insert new message to the message link...
                according to the priority of function code 
	              if function code > VIP_MSG_MAX  Insert the messge
		            to tail, else to head. 
    \param[in]  new message
    \param[out] none
    \retval     none
*/
void MsgLink_Insert(message_t new){
	message_t p = malloc(sizeof(struct message));
	p = new;
	if((p->func) < VIP_MSG_MAX)
		Msg_Head = Msg_Link_Insert_to_Head(Msg_Head,p);
	else
		Msg_Link_Insert_to_Tail(Msg_Head,p);
}
/*!
    \brief      Insert new message to the message link tail
		            to tail, else to head. 
    \param[in]  Message link head
    \param[in]  new message
    \param[out] none
    \retval     none
*/
message_t Msg_Link_Insert_to_Tail(message_t head,message_t node){
	message_t p = head;
	while(p->next != NULL)
		p = p->next;
	p->next = node;
	return head;
}
	
/*!
    \brief      Insert new message to the message link head
    \param[in]  Message link head
    \param[in]  new message
    \param[out] none
    \retval     none
*/
message_t Msg_Link_Insert_to_Head(message_t link_head,message_t node){
	node->next = link_head->next;
	link_head->next = node;
	return link_head;
}

//unsigned char Msg_Dispose(void *msg_out){
//	 unsigned char re_code= 0x00;
//	 msg_out = NULL;
//	 message_t Node = Msg_Head->next;
//	 Msg_Head->next = Node->next;
//	 if(Node != NULL){
//		switch(Node->func){
//			case Data_update:
//				break;
//			case Light_Dir_Ctrl:
//				{
//					
//					Directions_T dir=*(Node->data);
//					if(dir < dir_eof ){
//						msg_out = &dir ;
//						re_code = Direction;
//					}
//					
//				}
//				break;
//			default:
//				break;
//		}
//	 }
//	 return  re_code;
//}
static unsigned short cnt = 0;
void MSG_Handler_Func(void){
	cnt++;
	if(cnt==0xFFFF)
		cnt=0;
	if(cnt % 50 ==0){
		event = Data_Upload_Event;
	}

	if(Int_Msg_Head->next != NULL){
		Internal_MSG_T *p = Int_Msg_Head->next;
		Int_Msg_Head->next = p->next;
		switch(p->id){
			case Undef:
			{
				
				switch(p->func_code){
					case Factory_Rst ... Net_Time_Get:
						break;
					case FW_Replace:
						break;
					case Data_Query:
						break;
					case Data_Upload:
					{
						if(p->sn == pre_data.sn)
							event = Reply_Rec_Event;
					}
						break;
					case LoraChip_Para_Push ... LoraChip_Para_Repalce:
					{	
						Internal_MSG_T *q = InMsg_Lora_Head;
						while(q->next !=NULL)
							q = q->next;
						p->next = q->next;
						q->next = p;
					}
						break;
					case LoraModule_Para_Set ... LoraModule_Para_Query:
					{	
						Internal_MSG_T *q = InMsg_Lora_Head;
						while(q->next !=NULL)
							q = q->next;
						p->next = q->next;
						q->next = p;
					}
						break;
					case DataComp_Get ... DataAcq_Int_Issue:
						break;
					case Light_Dir_Issue ... Light_Dir_Query:
					{
						Internal_MSG_T *q = States_Msg_Head;
						while(q->next !=NULL)
							q = q->next;
						p->next = q->next;
						q->next = p;
					}
						break;
					case DevPosi_Issue ... DevPosi_Query:{
						Internal_MSG_T *q = States_Msg_Head;
						while(q->next !=NULL)
							q = q->next;
						p->next = q->next;
						q->next = p;
					}
						break;
				  case Abnormal_Posi_Issue ... Abnormal_Event_CLR:{
						Internal_MSG_T *q = States_Msg_Head;
						while(q->next !=NULL)
							q = q->next;
						p->next = q->next;
						q->next = p;
					}
						break;
					default:
						break;				
				}
			
			}
			break;
			case Lora:
				{
						Internal_MSG_T *q = InMsg_Lora_Head;
						while(q->next !=NULL)
							q = q->next;
						p->next = q->next;
						q->next = p;
					}
				break;
			case BT:
				break;
			case States:
				break;
			case Slave:
				break;
			case Direction:
				break;
			default :
				break ;
		}
	}
}

void MSG_FSM_RUN_Func(void){
	FSM_EventHandle(MSG_FSM,event);
}

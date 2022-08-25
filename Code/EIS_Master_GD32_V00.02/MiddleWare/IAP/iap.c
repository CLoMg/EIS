#include "iap.h"
#include "fsm.h"
#include "string.h"
#include "stdlib.h"
#include "msg_handler.h"
#include "gd32f10x_fmc.h"

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
	RX_Sucess_Event,RX_Error_Event,RX_Timeout_Event, FW_Acq_Finish_Event,
	Acq_Failed_Event,FW_Version_Invalid_Event,
}IAP_Events_T;



static StateTransform_T trans_table[] = 
{
	{UnInit_State,Init_Event,Idle_State,IAP_Init_Func},
	{Idle_State,FW_Upload_Event,FW_Acq_State,IAP_Calc_Func},
	{FW_Acq_State,FW_Acq_Event,Wait_Reply_State,FW_Acq_Func},
	{FW_Acq_State,Acq_Failed_Event,Idle_State,IAP_Failed_Log_Func},
	{FW_Acq_State,FW_Version_Invalid_Event,Idle_State,IAP_Failed_Log_Func},
	{FW_Acq_State,FW_Acq_Finish_Event,Idle_State,App_Jump_Func},
	{Wait_Reply_State,RX_Timeout_Event,FW_Acq_State,FW_ReAcq_Func},
	{Wait_Reply_State,No_Event,Wait_Reply_State,Timeout_Count_Func},
	{Wait_Reply_State,RX_Sucess_Event,FW_Acq_State,FW_RX_Handler_Func},
};

IAP_Info_Struct_T *IAP_Info_Struct = NULL;
void IAP_Calc_Func(void){
	uint8_t fw_version_cur = (uint8_t)(*(__IO uint32_t *)(FW_VERSION_CUR));
	
	xData *tx_msg = (xData *)pvPortMalloc(sizeof(xData));
	uint8_t *data = (uint8_t *)pvPortMalloc(sizeof(uint8_t)*1);
	tx_msg->iEventType = LORATxEvent;
	tx_msg->iMeaning = FW_Update;
	tx_msg->len = 1;
	tx_msg->iValue = data;
	tx_msg->sn = 0x00;
	 /*��Ҫ�ͷ��ڴ��𣿣�����*/
	/*�汾�Աȣ��·��汾�ţ����ڵ�ǰ�汾������������*/
	if(IAP_Info_Struct->fw_version != fw_version_cur){
			uint8_t fw_version_unfinished = (uint8_t)(*(__IO uint32_t *)(FW_VERSION_UNFINISHED));
		/*�Աȵ�ǰ�̼��Ƿ�ϵ�������*/
		if(IAP_Info_Struct->fw_version == fw_version_unfinished){
			/*��ǰ������ʼ��*/
			IAP_Info_Struct->fw_pack_cur = (*(__IO uint32_t *)(FW_RECIEVED_AMOUNT));
		}
		else
			/*��ǰ������ʼ��*/
			IAP_Info_Struct->fw_pack_cur = 0;
		/*re_code = 0x01 ��Ч�̼��汾 */
		*tx_msg->iValue = 0x01;
		event = FW_Acq_Event;
	}
	/*�·��̼��汾���뵱ǰ�汾��ͬ��������*/
	else{
		/*re_code = 0x02 �¹̼��汾�뵱ǰ�̼��汾��ͬ*/
		*tx_msg->iValue = 0x02;
		event = FW_Version_Invalid_Event;
	}
	xQueueSend(MSGTR_Queue,tx_msg,500/portTICK_RATE_MS);
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
	xData *tx_msg = (xData *)pvPortMalloc(sizeof(xData));
	uint8_t *data = (uint8_t *)pvPortMalloc(sizeof(uint8_t)*4);
	tx_msg->iEventType = LORATxEvent;
	tx_msg->iMeaning = FW_Acquire;
	tx_msg->len = 4;
	data[0] = ITEM_NUM;
	data[1] = IAP_Info_Struct->fw_version;
	data[2] = (uint8_t)(IAP_Info_Struct->fw_pack_cur>>8);
	data[3] = (uint8_t)IAP_Info_Struct->fw_pack_cur;
	tx_msg->iValue = data;
	tx_msg->sn = 0x00;
	
	xQueueSend(MSGTR_Queue,tx_msg,500/portTICK_RATE_MS);
	 /*��Ҫ�ͷ��ڴ��𣿣�����*/
	event  = No_Event;
}
void Timeout_Count_Func(void){
	static uint8_t wait_count = 0;
	wait_count++;
	if(wait_count > 50){
		event = RX_Timeout_Event;
		wait_count = 0;
	}
	
}
/*ֱ��ͨ��ϵͳ��λ������bootloader*/
void App_Jump_Func(void){
	__disable_irq();
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}
void FW_ReAcq_Func(void){
	static uint8_t retry_count = 0;
	
	retry_count++;
	if(retry_count > 5){
		retry_count = 0;
		event = Acq_Failed_Event;
	}
	else
		event = FW_Acq_Event;
}
/*�̼���ȡʧ��ʱ����¼��Ϣ��flash�������´ζϵ�����*/
void IAP_Failed_Log_Func(void){
	/*����ʧ�ܹ̼��汾��*/
		
	/*�´ο�ʼ�����*/
	
	/*�´δ�д���׵�ַ*/
	
}
void FW_RX_Handler_Func(void){
	
	IAP_Info_Struct->fw_pack_cur++;
	IAP_Info_Struct->fw_addr += IAP_Info_Struct->fw_pack_size_cur;
	/*����ȡ�̼������ > �ܵİ���� ���̼���������ϣ�׼��������BootLoader������й̼�У�������*/
	if(IAP_Info_Struct->fw_pack_cur > IAP_Info_Struct->fw_pack_total){
		
		event = FW_Acq_Finish_Event; //��������¼�
		uint32_t data = 0x01; 
		fmc_word_write(NEW_FW_VERSION_FLAG,1,&data);
		fmc_word_write(NEW_FW_SIZE,1,(uint32_t *)IAP_Info_Struct->fw_addr);
		fmc_word_write(NEW_FW_CHECKSUM, 4, (uint32_t *)IAP_Info_Struct->fw_check_sum);
	
	}
	else
		event  = FW_Acq_Event;
}
	
void IAP_MSG_Handler(void *msg){
		xData *p = (xData *)msg;
		switch(p->iMeaning){
			case FW_Update:{
				IAP_Info_Struct->fw_version = p->iValue[0];
				uint32_t fw_size =  ((uint32_t)p->iValue[1]<<24)|((uint32_t)p->iValue[2]<<16)|((uint16_t)p->iValue[3]<<8)|(p->iValue[4]);
				IAP_Info_Struct->fw_pack_total = (fw_size % 200 == 0)? (fw_size / 200 ):( fw_size / 200 + 1);
				memcpy(IAP_Info_Struct->fw_check_sum,&p->iValue[6],16);
				event = FW_Upload_Event;
			}
				break;
			case FW_Acquire:
				if(((uint16_t)p->iValue[0]<<8 | p->iValue[1] ) == IAP_Info_Struct->fw_pack_cur){
					/*д��flash*/
					uint8_t wlen=0;
					wlen = ((p->len-2)%4 == 0)? (p->len-2)/4 : (p->len-2)/4 + 1;
					fmc_word_write((uint32_t )IAP_Info_Struct->fw_addr, wlen, (uint32_t *)&(p->iValue[2]));
					IAP_Info_Struct->fw_pack_size_cur = wlen;
					event = RX_Sucess_Event;
				}
				else
					event = RX_Error_Event;
					;
				break;
			default:
				break;
			
		}
}
void IAP_FSM_RUN(void){
	FSM_EventHandle(IAP_FSM,event);
}

void IAP_Process(void){
}

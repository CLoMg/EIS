#ifndef IAP_H
#define IAP_H

typedef struct IAP_Info_Struct{
	unsigned char fw_version;
	unsigned short fw_pack_total;
	unsigned short fw_pack_cur;
	unsigned short fw_pack_size_cur;
	unsigned char  fw_pack_size_single;
	unsigned char  *fw_addr;
}IAP_Info_Struct_T;

void IAP_Calc_Func(void);
void IAP_Init_Func(void);
void IAP_Msg_LinkList_Init(void);
IAP_Info_Struct_T* IAP_Info_Struct_Init(void);
void IAP_FSM_Init(void);
void FW_Acq_Func(void);
void App_Jump_Func(void);
void FW_ReAcq_Func(void);
void IAP_Failed_Log_Func(void);
void Timeout_Count_Func(void);
void FW_RX_Handler_Func(void);
void IAP_MSG_Handler(void *msg);
void IAP_FSM_RUN(void);
void IAP_Process(void);
#endif

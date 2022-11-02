#ifndef __LORA_H
#define __LORA_H
#include "sys.h"

//����LORA ��Դʹ������ PD8
#define LORA_PowerEN PDout(8)
#define LORA_MSG_MIN_LEN 21

typedef struct LORA_Rec_Msg_Link{
	uint8_t data_len;
	uint8_t *data_addr;
	struct LORA_Rec_Msg_Link *next;
}LORA_Rec_Msg_Link_T;
extern LORA_Rec_Msg_Link_T *LORA_Rec_Msg_Head ;
//LORA���ų�ʼ��
void LORA_Hardware_Reset(void);
void LORA_GPIO_Init(void);
void LORA_Init(void);
Error_Code_t LORA_test(void);

//����ģ��
_Bool LoRA_Restart_Mode(void);

// ��ȡLORAģ���ʼ�����ʵȼ����ŵ�����
void LORA_Setup_Parameter_Access(void);

//����ATָ��ģʽ ����ֵ0-ʧ�� 1-�ɹ�
_Bool LoRA_Enter_Command_Mode(void);

//����LoRAģ�����ʵȼ� new_speed: 1~10  ����ֵ 0��ʧ�� 1���ɹ�
_Bool LoRA_Speed_Grade_Set(uint8_t new_speed,uint8_t mode_change,uint8_t restart);

//��ѯģ�����ʵȼ� ����ֵ-1����ѯʧ�ܣ�1~10Ϊ���ʵȼ�
int8_t LoRA_Query_Speed_Grade(void);

//����LoRAģ���ŵ�new_ch:1~127
_Bool LoRA_Channel_Set(uint8_t new_ch,uint8_t mode_change,uint8_t restart);

//��ѯģ���ŵ� ����ֵ-1����ѯʧ�ܣ�0~127Ϊ��ǰ�ŵ�
int8_t LoRA_Query_Channel(void);

//����LoRAģ�����ʵȼ����ŵ� new_speed(���ʵȼ�): 1~10 new_channel(�ŵ�)��0~127
_Bool LoRA_SPD_CH_Set(uint8_t new_speed,uint8_t new_channel);

void LORA_send_data(char *sendbuff,uint16_t length);
#endif

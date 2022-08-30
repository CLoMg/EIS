#ifndef __LORA_H
#define __LORA_H

#include <stdint.h>
//����LORA ��Դʹ������ PD8
#define LORA_PowerEN PDout(8)

typedef struct Lora_Module_Para{
	uint8_t ch;
	uint8_t spd;
}Lora_Module_Para_T;

typedef struct Lora_Para_Record{
	Lora_Module_Para_T src_para;
	Lora_Module_Para_T new_para;
}Lora_Para_Record_T;

/*�豸��ƽ̨������豸���н��������ݽṹ*/
typedef struct {
	uint8_t Head[2];
	uint8_t Device_Id[8];
	uint8_t SN[2];
	uint8_t Encry_Mode;
	uint8_t Func_Code[4];  
	uint8_t Data_Length[2];
	uint8_t *Data_Addr;
	uint8_t CRC16[2];
}External_MSG_T;

typedef struct Lora_TxMSG{
	uint16_t  len;
	uint8_t *data;
	struct Lora_TxMSG *next;
}Lora_TxMSG_T;

//LORA���ų�ʼ��
void LORA_Hardware_Reset(void);
void LORA_GPIO_Init(void);
void LORA_Init(void);
_Bool LORA_Param_Set(void);

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

void LORA_TR_Func(void);
void LOAR_ExMSG_Handler();
void LOAR_InMSG_Handler();
uint16_t CRC_Calc(uint8_t *data_buff,uint8_t len);
void Device_ID_Get(void);
#endif

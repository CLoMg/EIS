#ifndef __LORA_H
#define __LORA_H

#include <stdint.h>
//定义LORA 电源使能引脚 PD8
#define LORA_PowerEN PDout(8)

typedef struct Lora_Module_Para{
	uint8_t ch;
	uint8_t spd;
}Lora_Module_Para_T;

typedef struct Lora_Para_Record{
	Lora_Module_Para_T src_para;
	Lora_Module_Para_T new_para;
}Lora_Para_Record_T;

/*设备与平台或调试设备进行交互的数据结构*/
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

//LORA引脚初始化
void LORA_Hardware_Reset(void);
void LORA_GPIO_Init(void);
void LORA_Init(void);
_Bool LORA_Param_Set(void);

//重启模块
_Bool LoRA_Restart_Mode(void);

// 获取LORA模块初始化速率等级和信道参数
void LORA_Setup_Parameter_Access(void);

//进入AT指令模式 返回值0-失败 1-成功
_Bool LoRA_Enter_Command_Mode(void);

//设置LoRA模块速率等级 new_speed: 1~10  返回值 0：失败 1：成功
_Bool LoRA_Speed_Grade_Set(uint8_t new_speed,uint8_t mode_change,uint8_t restart);

//查询模块速率等级 返回值-1：查询失败，1~10为速率等级
int8_t LoRA_Query_Speed_Grade(void);

//设置LoRA模块信道new_ch:1~127
_Bool LoRA_Channel_Set(uint8_t new_ch,uint8_t mode_change,uint8_t restart);

//查询模块信道 返回值-1：查询失败，0~127为当前信道
int8_t LoRA_Query_Channel(void);

//设置LoRA模块速率等级和信道 new_speed(速率等级): 1~10 new_channel(信道)：0~127
_Bool LoRA_SPD_CH_Set(uint8_t new_speed,uint8_t new_channel);

void LORA_send_data(char *sendbuff,uint16_t length);

void LORA_TR_Func(void);
void LOAR_ExMSG_Handler();
void LOAR_InMSG_Handler();
uint16_t CRC_Calc(uint8_t *data_buff,uint8_t len);
void Device_ID_Get(void);
#endif

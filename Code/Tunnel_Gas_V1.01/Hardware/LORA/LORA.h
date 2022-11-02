#ifndef __LORA_H
#define __LORA_H
#include "sys.h"

//定义LORA 电源使能引脚 PD8
#define LORA_PowerEN PDout(8)
#define LORA_MSG_MIN_LEN 21

typedef struct LORA_Rec_Msg_Link{
	uint8_t data_len;
	uint8_t *data_addr;
	struct LORA_Rec_Msg_Link *next;
}LORA_Rec_Msg_Link_T;
extern LORA_Rec_Msg_Link_T *LORA_Rec_Msg_Head ;
//LORA引脚初始化
void LORA_Hardware_Reset(void);
void LORA_GPIO_Init(void);
void LORA_Init(void);
Error_Code_t LORA_test(void);

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
#endif

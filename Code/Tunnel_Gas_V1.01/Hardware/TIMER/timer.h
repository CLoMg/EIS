#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
	
extern _Bool Data_Acq_Flag;
extern _Bool System_Reset_Flag;
extern _Bool Data_UpLoad_Flag;
extern _Bool Node_Upgrade_Flag;
extern uint8_t Terminal_num;
extern uint16_t Sync_Time;
extern uint16_t upgrade_cnt;
extern _Bool upgrade_timeout_flag;

void Task_Timer_Redister(void);
void Timer_Init(void);
void TIM3_Init(u16 arr,u16 psc);
uint8_t Update_time_Change(uint16_t time);

void TIM2_Init(u16 arr,u16 psc);


#endif

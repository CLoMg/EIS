#ifndef __TASK_H
#define __TASK_H
#include "sys.h"
typedef struct _TASK_COMPONENTS
{
	uint8_t Run;     //Task执行状态标识： 0-不执行 1-执行
	uint32_t Timer;   //计时器
	uint32_t ItvTime; //任务执行间隔时间（AKA计时器初始值）
	void (*TaskHook)(void); //待执行的任务函数
}TASK_COMPONENTS;

enum TASK_LIST
{
	TASK_4G_RECON = 0,
	TASK_IWDG_FEED,
	TASK_4G_MSG,
	TASK_LORA,
	TASK_ETH,
	TASK_DATA_ACQ,
	TASK_DATA_UPLOAD,
	TASK_MOTOR,
	TASK_SYS_RESET,
	TASK_NODE_UPGRADE,
	TASK_MAX,
};

extern TASK_COMPONENTS Task_Comps[] ;

void TaskRemarks(void);
void TaskProcess(void);

void Motor_Task(void);
void IWDG_Feed_Task(void);
void _4G_MSG_Task(void);
void _4G_Reconnect_Task(void);
void ETH_Task(void);
void LORA_Task(void);
void Data_Acq_Task(void);
void System_Reset_Task(void);
void Data_Upload_Task(void);
void Node_Upgrade_Task(void);


#endif

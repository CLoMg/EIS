#include "task.h"
#include "motor.h"
#include "wdg.h"
#include "usart.h"
#include "ec20.h"
#include "string.h"
#include "command.h"
#include "lora.h"
#include "stdlib.h"
#include "usr_k7.h"
#include "upgrade.h"
#include "delay.h"


TASK_COMPONENTS Task_Comps[] =
{
	{0,30,30,_4G_Reconnect_Task},
	{0,10,10,IWDG_Feed_Task},
	{0,1,1,_4G_MSG_Task},
	{0,1,1,LORA_Task},
	{0,1,1,ETH_Task},
	{0,1,1,Data_Acq_Task},
	{0,5,5,Data_Upload_Task},
	{0,60*60,60*60,Motor_Task},
	{0,24*60*60,24*60*60,System_Reset_Task},
	{0,60*60,60*60,Node_Upgrade_Task},
};
/**************************************************************************************
* FunctionName   : TaskRemarks()
* Description    : 任务标志处理
* EntryParameter : None
* ReturnValue    : None
* attention      : ***在定时器中断中调用此函数***
**************************************************************************************/
void TaskRemarks(void)
{
	uint8_t i ;
	
	for(i=0;i<TASK_MAX;i++)
	{
		if(Task_Comps[i].Timer)
		{
			Task_Comps[i].Timer--;
			if(Task_Comps[i].Timer == 0)
			{	
				Task_Comps[i].Timer = Task_Comps[i].ItvTime;
				Task_Comps[i].Run = 1;
			}
		}
	}
}
/**************************************************************************************
* FunctionName   : TaskProcess()
* Description    : 任务处理|判断什么时候该执行那一个任务
* EntryParameter : None
* ReturnValue    : None
* * attention      : ***在mian的while(1)中调用***
**************************************************************************************/
void TaskProcess(void)
{
	uint8_t i;
	
	for (i=0; i<TASK_MAX; i++)           	// 逐个任务时间处理
	{
		if (Task_Comps[i].Run)           	// 时间不为0
		{
			// 运行任务
			Task_Comps[i].TaskHook();
			Task_Comps[i].Run = 0;          	// 标志清0
		}
	}   
}
void Motor_Task(void){
		Motor_N_Circle(2,1);
}

void IWDG_Feed_Task(void){
	IWDG_Feed();
}

void _4G_MSG_Task(void){
	if(command_recieved_flag_4G)
			Command_Parse(USART_RX_BUF[1]);
}

void _4G_Reconnect_Task(void){
	if(ec20_reconnect_flag)
	{
		memset(USART_RX_BUF[1],0,USART_REC_LEN);
		ec20_reconnect_flag=0;
		ec20_reconnnect();	
	}
}

void ETH_Task(void){
		if(command_recieved_flag_ETH)
			Command_Parse(USART_RX_BUF[2]);	
}
void LORA_Task(void){
	if(LORA_Rec_Msg_Head->next!=NULL){
			while(LORA_Rec_Msg_Head->next!=NULL){
			LORA_Rec_Msg_Link_T *p = LORA_Rec_Msg_Head->next;
			LORA_Rec_Msg_Head->next = p->next;
			
			if(p->data_len >= LORA_MSG_MIN_LEN){
				if((p->data_addr[0]==0x4A)&&(p->data_addr[1]==0X54)){
				#if gateway
					uint8_t i=0;
					for(i=1;i<9;++i){
						if(memcmp(&p->data_addr[2],terminal_id[i],8)==0){
							usr_data_send((char*)p->data_addr,(uint16_t)p->data_addr[17]<<8|p->data_addr[18]+21);
							ec20_send_data((char*)p->data_addr,(uint16_t)p->data_addr[17]<<8|p->data_addr[18]+21);
						break;
					}		
				}
			#else 
				Command_Parse(p->data_addr);
			#endif
				} 
			}
			
			free(p->data_addr);
			free(p);
	  }
	}
}

void Data_Acq_Task(void){
	Sensor_Data_Update();
}
void System_Reset_Task(void){
	NVIC_SystemReset();
}
void Data_Upload_Task(void){
	#if gateway
	Data_Upload(1);
	uint8_t empty_id[8]= {0x00,};
	char node_serial = 0;
		for(node_serial=2;node_serial < 10;node_serial++){
				if(memcmp(terminal_id[node_serial],empty_id,8)!=0){
					Data_Upload(node_serial);
					delay_ms(5);
				}
			}
	#endif
	Data_Upload(1);
}

void Node_Upgrade_Task(void){
	firmware_send_process();
 //Node_Upgrade_Flag=0;
}

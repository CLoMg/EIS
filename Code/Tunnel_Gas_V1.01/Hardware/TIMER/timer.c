#include "timer.h"
#include "stmflash.h"
#include "usart.h"
#include "motor.h"
#include "task.h"

uint8_t Terminal_num=0;
_Bool Data_Acq_Flag = 0;//传感器数据采集flag
_Bool HeartBeat_Flag = 0;//心跳包flag
_Bool Data_UpLoad_Flag=0;//数据上传flag
_Bool System_Reset_Flag= 0;//系统重启flag
_Bool Node_Upgrade_Flag =0;//节点固件更新flag

uint16_t Sync_Time=0,count_now=0;

uint16_t data_upload_period,data_upload_preriod_default=5;

uint16_t node_upgrade_cnt=0,node_upgrade_period=60*60;
//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!

void Task_Timer_Redister(void){
	if((data_upload_period=(uint16_t)STMFLASH_ReadWord(Data_Update_Frequence)) ==0xFFFF){
			data_upload_period = data_upload_preriod_default;
	};
	
	Task_Comps[TASK_DATA_UPLOAD].Timer = data_upload_period;
	Task_Comps[TASK_DATA_UPLOAD].ItvTime = data_upload_period;
}
void Timer_Init(void){
	Task_Timer_Redister();
	TIM3_Init(999,8399);//Tout=((arr+1)*(psc+1))/Ft us. (Ft 84MHZ)
	TIM2_Init(9999,8399);
}
//Tout = （（999+1）*（8399+1））/84MHz = 100 ms
void TIM3_Init(u16 arr,u16 psc)
{

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}


//改变定时中断时间 
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
uint8_t Update_time_Change(uint16_t time)
{  
	uint8_t change_success=0; 
	uint32_t flash_para_bak[27]={};
	
	
	data_upload_period = time;
	
	//对比此次设置参数和之前参数
	if(data_upload_period!=(uint16_t)STMFLASH_ReadWord(Data_Update_Frequence)){
		//写入flash
		STMFLASH_Read(User_Parameter_BASE,flash_para_bak,27);
		flash_para_bak[(Data_Update_Frequence-User_Parameter_BASE)/4]=(u32)data_upload_period;
		STMFLASH_Write(User_Parameter_BASE,flash_para_bak,27);
	}
	//读取flash
	if(data_upload_period==(uint16_t)STMFLASH_ReadWord(Data_Update_Frequence)){
		//匹配成功？返回 0/1
		change_success=1;
	}
	
	Task_Comps[TASK_DATA_UPLOAD].Timer = data_upload_period;
	Task_Comps[TASK_DATA_UPLOAD].ItvTime = data_upload_period;
	return change_success;
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断,100ms发生一次
	{
		count_now++;
		//溢出10次，计数器清零。TaskRemarks() 函数更新运行状态
		if(count_now>9){
			TaskRemarks();
			count_now=0;
		}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}



uint16_t upgrade_cnt=0;
_Bool upgrade_timeout_flag=0;
//通用定时器2中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!

//2160 000 000 =48 000 *45 000
void TIM2_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM2,DISABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}


//定时器2中断服务函数
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{
		upgrade_cnt++;
		if(upgrade_cnt>=30){
			upgrade_cnt=0;
			upgrade_timeout_flag=1;
		}
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
}


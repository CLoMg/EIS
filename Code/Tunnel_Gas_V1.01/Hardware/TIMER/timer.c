#include "timer.h"
#include "stmflash.h"
#include "usart.h"
#include "motor.h"
#include "task.h"

uint8_t Terminal_num=0;
_Bool Data_Acq_Flag = 0;//���������ݲɼ�flag
_Bool HeartBeat_Flag = 0;//������flag
_Bool Data_UpLoad_Flag=0;//�����ϴ�flag
_Bool System_Reset_Flag= 0;//ϵͳ����flag
_Bool Node_Upgrade_Flag =0;//�ڵ�̼�����flag

uint16_t Sync_Time=0,count_now=0;

uint16_t data_upload_period,data_upload_preriod_default=5;

uint16_t node_upgrade_cnt=0,node_upgrade_period=60*60;
//ͨ�ö�ʱ��3�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!

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
//Tout = ����999+1��*��8399+1����/84MHz = 100 ms
void TIM3_Init(u16 arr,u16 psc)
{

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}


//�ı䶨ʱ�ж�ʱ�� 
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
uint8_t Update_time_Change(uint16_t time)
{  
	uint8_t change_success=0; 
	uint32_t flash_para_bak[27]={};
	
	
	data_upload_period = time;
	
	//�Աȴ˴����ò�����֮ǰ����
	if(data_upload_period!=(uint16_t)STMFLASH_ReadWord(Data_Update_Frequence)){
		//д��flash
		STMFLASH_Read(User_Parameter_BASE,flash_para_bak,27);
		flash_para_bak[(Data_Update_Frequence-User_Parameter_BASE)/4]=(u32)data_upload_period;
		STMFLASH_Write(User_Parameter_BASE,flash_para_bak,27);
	}
	//��ȡflash
	if(data_upload_period==(uint16_t)STMFLASH_ReadWord(Data_Update_Frequence)){
		//ƥ��ɹ������� 0/1
		change_success=1;
	}
	
	Task_Comps[TASK_DATA_UPLOAD].Timer = data_upload_period;
	Task_Comps[TASK_DATA_UPLOAD].ItvTime = data_upload_period;
	return change_success;
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�,100ms����һ��
	{
		count_now++;
		//���10�Σ����������㡣TaskRemarks() ������������״̬
		if(count_now>9){
			TaskRemarks();
			count_now=0;
		}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}



uint16_t upgrade_cnt=0;
_Bool upgrade_timeout_flag=0;
//ͨ�ö�ʱ��2�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!

//2160 000 000 =48 000 *45 000
void TIM2_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///ʹ��TIM3ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM2,DISABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}


//��ʱ��2�жϷ�����
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //����ж�
	{
		upgrade_cnt++;
		if(upgrade_cnt>=30){
			upgrade_cnt=0;
			upgrade_timeout_flag=1;
		}
	}
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
}


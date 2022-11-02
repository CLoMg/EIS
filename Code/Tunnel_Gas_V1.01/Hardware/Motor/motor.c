#include "motor.h"
#include "delay.h"
#include "wdg.h"

/****�������������ʽ****/
/**A-AB-B-BC-C-CD-D-DA-A**/
  /**����� 5.625/64**/
/*************************/
_Bool Motor_Active_Flag=0;

void Motor_Init(void){
	Motor_PowerEN_Config();
	Motor_Driver_Config();
}
/****��ʼ����������ӿ�****/
void Motor_Driver_Config(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOE,GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13);
}

/****��ʼ���������ʹ�ܽӿ� PF11 ****/
void Motor_PowerEN_Config(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;
	
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	GPIO_SetBits(GPIOF,GPIO_Pin_11);
}


/****�������������ʽһ��ѭ�����൱�ڲ���8step������ ****/
/****������� ms--ÿ��֮��ļ��ʱ�䣬ȡ 1 �ȽϺ���  ****/
void Motor_One_Step(uint8_t ms){
	//A
	Motor_Phase_A=1;
	Motor_Phase_B=0;
	Motor_Phase_C=0;
	Motor_Phase_D=0;
	delay_ms(ms);
	//AB
	Motor_Phase_A=1;
	Motor_Phase_B=1;
	Motor_Phase_C=0;
	Motor_Phase_D=0;
	delay_ms(ms);

	//B
	Motor_Phase_A=0;
	Motor_Phase_B=1;
	Motor_Phase_C=0;
	Motor_Phase_D=0;
	delay_ms(ms);

	//BC
	Motor_Phase_A=0;
	Motor_Phase_B=1;
	Motor_Phase_C=1;
	Motor_Phase_D=0;
	delay_ms(ms);

	//C
	Motor_Phase_A=0;
	Motor_Phase_B=0;
	Motor_Phase_C=1;
	Motor_Phase_D=0;
	delay_ms(ms);

  //CD
	Motor_Phase_A=0;
	Motor_Phase_B=0;
	Motor_Phase_C=1;
	Motor_Phase_D=1;
	delay_ms(ms);

	
  //D
	Motor_Phase_A=0;
	Motor_Phase_B=0;
	Motor_Phase_C=0;
	Motor_Phase_D=1;
	delay_ms(ms);

	
  //DA
	Motor_Phase_A=1;
	Motor_Phase_B=0;
	Motor_Phase_C=0;
	Motor_Phase_D=1;
	delay_ms(ms);
	
}

/************���ת��һȦ********************/
/********* msΪÿ�ļ�ļ��ʱ��**************
���Լ��� ת��һȦ��Ҫ360/5.625*64 = 4096��
4096 * 1ms =4096 ms ��ת��һȦ��Ҫ4096ms
�����ת��Ϊ60 * 1000 ms / 4096 ms = 14.64RPM
*********************************************/

void Motor_One_Circle(uint8_t ms){
	uint16_t step_num=512;//  512=360/5.625*64/8
	while(step_num){
		#if iwatch_dog_work
			IWDG_Feed();
		#endif
		Motor_One_Step(ms);
		step_num--;
	}
	Motor_Phase_A=0;
	Motor_Phase_B=0;
	Motor_Phase_C=0;
	Motor_Phase_D=0;
}

/****���ת��NȦ****/
/****circlr_num  Ϊ����ת��Ȧ�� msΪÿ�ļ�ļ��ʱ��****/
void Motor_N_Circle(uint16_t circle_num,uint8_t ms){
	while(circle_num){
		Motor_One_Circle(ms);
		circle_num--;
	}

}

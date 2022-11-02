#include "motor.h"
#include "delay.h"
#include "wdg.h"

/****四相八拍驱动方式****/
/**A-AB-B-BC-C-CD-D-DA-A**/
  /**步距角 5.625/64**/
/*************************/
_Bool Motor_Active_Flag=0;

void Motor_Init(void){
	Motor_PowerEN_Config();
	Motor_Driver_Config();
}
/****初始化电机驱动接口****/
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

/****初始化电机供电使能接口 PF11 ****/
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


/****四相八拍驱动方式一个循环，相当于步进8step步进角 ****/
/****输出参数 ms--每拍之间的间隔时间，取 1 比较合适  ****/
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

/************电机转动一圈********************/
/********* ms为每拍间的间隔时间**************
可以计算 转动一圈需要360/5.625*64 = 4096拍
4096 * 1ms =4096 ms 即转动一圈需要4096ms
计算得转速为60 * 1000 ms / 4096 ms = 14.64RPM
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

/****电机转动N圈****/
/****circlr_num  为所需转动圈速 ms为每拍间的间隔时间****/
void Motor_N_Circle(uint16_t circle_num,uint8_t ms){
	while(circle_num){
		Motor_One_Circle(ms);
		circle_num--;
	}

}

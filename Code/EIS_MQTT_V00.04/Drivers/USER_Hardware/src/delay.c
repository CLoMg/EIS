/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	delay.c
	*
	*	作者： 		CL
	*
	*	日期： 		2017-01-23
	*
	*	版本： 		V1.0
	*
	*	说明： 		利用Timer6做阻塞式延时
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f1xx.h"

//delay头文件
#include "delay.h"



//延时系数
unsigned char UsCount = 0;
unsigned short MsCount = 0;



/*
************************************************************
*	函数名称：	Delay_Init
*
*	函数功能：	systick初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Delay_Init(void)
{

//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

//	TIM6->CR1 |= 1UL << 3;									//单脉冲模式
//	
//	TIM6->EGR |= 1;											//更新生成
//	
//	TIM6->DIER = 0;											//禁止所有中断
//	
//	TIM6->CR1 &= (unsigned short)~TIM_CR1_CEN;				//停止计时

}

/*
************************************************************
*	函数名称：	DelayUs
*
*	函数功能：	微秒级延时
*
*	入口参数：	us：延时的时长
*
*	返回参数：	无
*
*	说明：		此时钟(21MHz)最大延时798915us
************************************************************
*/
void DelayUs(unsigned short nus)
{

//	TIM2->ARR = us;
//	
//	TIM2->PSC = 71;									//timer6为72MHz，设置为71+1分频，1MHz，1us计数一次
//	
//	TIM2->CR1 |= (unsigned short)TIM_CR1_CEN;		//开始计时
//	
//	while(!(TIM2->SR & 1));							//等待产生中断事件
//	
//	TIM2->SR &= ~(1UL << 0);						//清除标志
	unsigned int ticks;
	unsigned int told,tnow,reload,tcnt=0;
//	if((0x0001&(SysTick->CTRL)) ==0)    //定时器未工作
//		vPortSetupTimerInterrupt();  //初始化定时器

	reload = SysTick->LOAD;                     //获取重装载寄存器值
	ticks = nus * (SystemCoreClock / 1000000);  //计数时间值
	told=SysTick->VAL;                          //获取当前数值寄存器值（开始时数值）

	while(1)
	{
		tnow=SysTick->VAL;          //获取当前数值寄存器值
		if(tnow!=told)              //当前值不等于开始值说明已在计数
		{         

					 if(tnow<told)             //当前值小于开始数值，说明未计到0
								tcnt+=told-tnow;     //计数值=开始值-当前值

					 else                  //当前值大于开始数值，说明已计到0并重新计数
									tcnt+=reload-tnow+told;   //计数值=重装载值-当前值+开始值  （已
																						//从开始值计到0） 

					 told=tnow;                //更新开始值
					 if(tcnt>=ticks)break;     //时间超过/等于要延迟的时间,则退出.
		} 
	}     
}

/*
************************************************************
*	函数名称：	DelayXms
*
*	函数功能：	毫秒级延时
*
*	入口参数：	ms：延时的时长
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void DelayXms(unsigned short ms)
{

	if(ms < 32768)
	{
//		TIM6->ARR = (ms << 1);						//双倍计数值
//		
//		TIM6->PSC = 35999;							//timer6为72MHz，设置为35999+1分频，2KHz，500us计数一次
//		
//		TIM6->CR1 |= (unsigned short)TIM_CR1_CEN;	//开始计时
//		
//		while(!(TIM6->SR & 1));						//等待产生中断事件
//		
//		TIM6->SR &= ~(1UL << 0);					//清除标志
		HAL_Delay(ms-1);
	}

}

/*
************************************************************
*	函数名称：	DelayMs
*
*	函数功能：	微秒级长延时
*
*	入口参数：	ms：延时的时长
*
*	返回参数：	无
*
*	说明：		多次调用DelayXms，做到长延时
************************************************************
*/
void DelayMs(unsigned short ms)
{

	unsigned char repeat = 0;
	unsigned short remain = 0;
	
	repeat = ms / 500;
	remain = ms % 500;
	
	while(repeat)
	{
		DelayXms(500);
		repeat--;
	}
	
	if(remain)
		DelayXms(remain);

}

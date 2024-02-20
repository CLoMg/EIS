/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	led.c
	*
	*	作者： 		CL
	*
	*	日期： 		2022-10-07
	*
	*	版本： 		V1.0
	*
	*	说明： 		LED初始化，亮灭LED
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f4xx.h"

//LED头文件
#include "led.h"
#include "gpio.h"




LED_STATUS ledStatus;


/*
************************************************************
*	函数名称：	Led_Init
*
*	函数功能：	LED初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		LED4-PC7	LED5-PC8	LED6-PA12	LED7-PC10
				高电平关灯		低电平开灯
************************************************************
*/
void Led_Init(void)
{
	
		LED_GPIO_Init();
	
    
    Led4_Set(LED_ON);
    Led5_Set(LED_ON);

}

/*
************************************************************
*	函数名称：	Led4_Set
*
*	函数功能：	LED4控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led4_Set(LED_ENUM status)
{
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,status == LED_ON ? GPIO_PIN_RESET : GPIO_PIN_SET);

	ledStatus.Led4Sta = status;
}

/*
************************************************************
*	函数名称：	Led5_Set
*
*	函数功能：	LED5控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led5_Set(LED_ENUM status)
{

	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,status == LED_ON ? GPIO_PIN_RESET : GPIO_PIN_SET);

	ledStatus.Led4Sta = status;

}

/*
************************************************************
*	函数名称：	Led6_Set
*
*	函数功能：	LED6控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led6_Set(LED_ENUM status)
{

//	GPIO_WriteBit(GPIOA, GPIO_Pin_12, status != LED_ON ? Bit_SET : Bit_RESET);
//	ledStatus.Led6Sta = status;

}

/*
************************************************************
*	函数名称：	Led7_Set
*
*	函数功能：	LED7控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led7_Set(LED_ENUM status)
{

//	GPIO_WriteBit(GPIOC, GPIO_Pin_10, status != LED_ON ? Bit_SET : Bit_RESET);
//	ledStatus.Led7Sta = status;

}

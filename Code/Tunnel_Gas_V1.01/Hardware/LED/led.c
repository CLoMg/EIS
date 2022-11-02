#include "led.h"
#include "delay.h"

//初始化LED控制引脚，旧版本 LED1-PB8 LED2--PB9
//                   新版本 LED1-PD11 LED2--PD12
void led_init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB总线时钟

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //GPIOB6与GPIOB7
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //开漏模式
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //无上下拉
//	
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOB,GPIO_Pin_8);
//	

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOB总线时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12; //GPIOB6与GPIOB7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //开漏模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //无上下拉
	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_12);
}


void led_beep(uint8_t LED,uint16_t gap){
	if(LED==1){
			LED1=0;
			delay_ms(gap);
			LED1=1;
			delay_ms(gap);
	}
	if(LED==2){
			LED2=0;
			delay_ms(gap);
			LED2=1;
			delay_ms(gap);
	}

}


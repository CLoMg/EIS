#include "led.h"
#include "delay.h"

//��ʼ��LED�������ţ��ɰ汾 LED1-PB8 LED2--PB9
//                   �°汾 LED1-PD11 LED2--PD12
void led_init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOB����ʱ��

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //GPIOB6��GPIOB7
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //��©ģʽ
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //��������
//	
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOB,GPIO_Pin_8);
//	

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOB����ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12; //GPIOB6��GPIOB7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //��©ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //��������
	
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


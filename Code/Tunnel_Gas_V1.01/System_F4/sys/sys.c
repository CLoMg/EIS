#include "sys.h"  
#include "core_cm4.h"

uint8_t device_type_num=5;
uint8_t terminal_id[9][8]={0xff,};

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}

void Get_Device_ID(uint32_t *id){
	uint32_t *temp=id;
	
	for(int i=0;i<3;++i){
		*(temp+i)=*(volatile uint32_t*)(ID_ADDR1+i*4);
	}
}
void Sensor_5V_PowerEN_Init(void){
		//初始化传感5v 供电使能引脚 旧版 PA3 新版PD9
		GPIO_InitTypeDef GPIO_InitStructure;
		
	//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA总线时钟

	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //GPIOA3
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽模式
	//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	//	
	//	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOD总线时钟

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //GPIOD9
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽模式
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
		
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	}

















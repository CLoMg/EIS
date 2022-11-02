#include "sys.h"  
#include "core_cm4.h"

uint8_t device_type_num=5;
uint8_t terminal_id[9][8]={0xff,};

//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//�ر������ж�(���ǲ�����fault��NMI�ж�)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//���������ж�
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//����ջ����ַ
//addr:ջ����ַ
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
		//��ʼ������5v ����ʹ������ �ɰ� PA3 �°�PD9
		GPIO_InitTypeDef GPIO_InitStructure;
		
	//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOA����ʱ��

	//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //GPIOA3
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //����ģʽ
	//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	//	
	//	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOD����ʱ��

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //GPIOD9
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //����ģʽ
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
		
		GPIO_Init(GPIOD, &GPIO_InitStructure);
	}

















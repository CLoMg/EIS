#include "usart.h"	
#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "ec20.h"
#include "command.h"
#include "lora.h"

_Bool command_recieved_flag_4G=0;
_Bool command_recieved_flag_ETH=0;
_Bool command_recieved_flag_LORA=0;

extern void processBytes(char * bytes, unsigned short len);

//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif

//�������´���,֧��//printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
// __use_no_semihosting was requested, but _ttywrch was referenced
void _ttywrch(int ch){
ch=ch;
}
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
#if serial_port1_debug
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
	USART3->DR = (u8) ch; 	
	return ch;
#endif
}
#endif
 
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[6][USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

u16 USART_RX_CNT[6]={0,0,0,0,0,0};			//���յ��ֽ���	 

uint8_t RS485_EN=0;
//��ʼ��IO ����1
/********��һ����Լ򻯣�Ϊ�˷�����ԣ�ǰ����������������ɺ��*******/
Error_Code_t uart_init(USART_TypeDef* USARTx,u32 bound){
	  //�����豸���Ͷ���RS485ʹ��
		if (device_type_num<2)
			 RS485_EN=0x00  ;//bit[0~5] ��ӦUART1~6��RS485ʹ��  0--Disable  1--Enable
		else if (device_type_num==2)
			 RS485_EN=0x01;
		else
			 RS485_EN=0x21;
		
		Error_Code_t error_code;
		error_code=IS_USART_ALL_PERIPH(USARTx);
		CHECK_ERROR(error_code);
   //GPIO�˿�����
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		
		 //USARTͳһ��ʼ������
		USART_InitStructure.USART_BaudRate = bound;//����������
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	
	if(USARTx==USART1){
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
	 
		//����1��Ӧ���Ÿ���ӳ��
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
		
		//USART1�˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
		GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10
		
					//���UART1ʹ��RS485,����RS485_EN����
		if(RS485_EN&0x01){
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //GPIOA8
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //��������
			GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA8
			U1_RS485_TX_EN=0; //RS485����Ϊ����ģʽ
		}

		USART_Init(USART1, &USART_InitStructure); //��ʼ������1
		USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
		USART_ClearFlag(USART1, USART_FLAG_TC);//���USART_FLAG_TC��־λ
		
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
		//Usart1 NVIC ����
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
		


	}
	else if(USARTx==USART2){
		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
		
		/* ������STM32F407̽���߿������IO����*/
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIODʱ��
		//����1��Ӧ���Ÿ���ӳ��
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); //GPIOD5����ΪUSART2
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); //GPIOD6����ΪUSART2
		
		//USART1�˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; //GPIOD5��GPIOD6
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
		GPIO_Init(GPIOD,&GPIO_InitStructure); //��ʼ��PD5��PD6
				/* ������STM32F407̽���߿������IO����*/
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIODʱ��
//		//����1��Ӧ���Ÿ���ӳ��
//		GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOD5����ΪUSART2
//		GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOD6����ΪUSART2
//		
//		//USART1�˿�����
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOD5��GPIOD6
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
//		GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PD5��PD6
		
							//���UART1ʹ��RS485,����RS485_EN����
		if(RS485_EN&(0x01<<1)){
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); //ʹ��GPIODʱ��
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //GPIOA2
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //��������
			GPIO_Init(GPIOG,&GPIO_InitStructure); //��ʼ��PA2
			U2_RS485_TX_EN=0; //RS485����Ϊ����ģʽ
		}	

		USART_Init(USART2, &USART_InitStructure); //��ʼ������2
		USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���2
		USART_ClearFlag(USART2, USART_FLAG_TC);//���USART_FLAG_TC��־λ
		
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�
		USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
		//Usart2 NVIC ����
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����1�ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
		

	}
	else if(USARTx==USART3){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOBʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��
	 
		//����3��Ӧ���Ÿ���ӳ��
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOD10����ΪUSART3
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOD11����ΪUSART3
		
		//USART3�˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB10��GPIOB11
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
		GPIO_Init(GPIOB,&GPIO_InitStructure); //��ʼ��PB10��PB11

		USART_Init(USART3, &USART_InitStructure); //��ʼ������3
		USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���3
		USART_ClearFlag(USART3, USART_FLAG_TC);//���USART_FLAG_TC��־λ
		
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�
		USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
		//Usart3 NVIC ����
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//����3�ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	}
	else if(USARTx==UART4){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//ʹ��USART4ʱ��
				/* �����ھ�ͨPCB���IO����*/
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
		//����4��Ӧ���Ÿ���ӳ��
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4); //GPIOA0����ΪUART4
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4); //GPIOA1����ΪUART4
		
		//UART4�˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //GPIOA0��GPIOA1
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
		GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA0��PA1

		/* ������STM32F407̽���߿������IO����*/
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIOAʱ��
//		//����4��Ӧ���Ÿ���ӳ��
//		GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOA0����ΪUART4
//		GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOA1����ΪUART4
//		
//		//UART4�˿�����
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOA0��GPIOA1
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
//		GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��PA0��PA1
		
					//���UART1ʹ��RS485,����RS485_EN����
		if(RS485_EN&(0x01<<3)){
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //GPIOA2
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //��������
			GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA2
			U4_RS485_TX_EN=0; //RS485����Ϊ����ģʽ
		}	

		USART_Init(UART4, &USART_InitStructure); //��ʼ������4
		USART_Cmd(UART4, ENABLE);  //ʹ�ܴ���4
		USART_ClearFlag(UART4, USART_FLAG_TC);//���USART_FLAG_TC��־λ
			
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//��������ж�
		USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
		//Usart4 NVIC ����
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//����4�ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	}
	else if(USARTx==UART5){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIOCʱ��
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIODʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);//ʹ��USART5ʱ��
	 
		//����5��Ӧ���Ÿ���ӳ��
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5); //GPIOC12����ΪUSART5
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5);  //GPIOD2����ΪUSART5
		
		//USART5�˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//GPIOC12
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
		GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��PC12
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//GPIOD2
		GPIO_Init(GPIOD,&GPIO_InitStructure); //��ʼ��PD2
		
					//���UART1ʹ��RS485,����RS485_EN����
		if(RS485_EN&(0x01<<4)){
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //GPIOC11
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //��������
			GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��PA8
			U5_RS485_TX_EN=0; //RS485����Ϊ����ģʽ
		}	
		
		USART_Init(UART5, &USART_InitStructure); //��ʼ������5
		USART_Cmd(UART5, ENABLE);  //ʹ�ܴ���5
		USART_ClearFlag(UART5, USART_FLAG_TC);//���USART_FLAG_TC��־λ
		
		USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//��������ж�
		USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
		//Usart5 NVIC ����
		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//����5�ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	}
		else if(USARTx==USART6){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIOCʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//ʹ��USART6ʱ��
	 
		//����6��Ӧ���Ÿ���ӳ��
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6); //GPIOC6����ΪUSART6
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); //GPIOC7����ΪUSART6
		
		//USART1�˿�����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOC6��GPIOC7
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
		GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��PC6��PC7
			
						//���UART1ʹ��RS485,����RS485_EN����
		if(RS485_EN&(0x01<<5)){
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //GPIOC8
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //��������
			GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��PC8
			U6_RS485_TX_EN=0; //RS485����Ϊ����ģʽ
		}

		USART_Init(USART6, &USART_InitStructure); //��ʼ������6
		USART_Cmd(USART6, ENABLE);  //ʹ�ܴ���6
		USART_ClearFlag(USART6, USART_FLAG_TC);//���USART_FLAG_TC��־λ
		

		USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//��������ж�
		USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);
		//Usart6 NVIC ����
		NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//����1�ж�ͨ��
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	}
		
		else 
			error_code=0;
		//�����ʼ�����ڲ�����ȷ�������һ�����ַ��������һ���ֽ��޷�ͨ�����ڴ�ӡ������
		if(error_code){
			USART_SendData(USARTx,0x00);
			while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		  USART_ClearFlag(USARTx, USART_FLAG_TC);             //������������ɱ�־λ	
		
		}
    return error_code;	
}

void USART1_IRQHandler(void)                	//����1�жϷ������
{
//	u8 res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
	{
		USART_RX_BUF[0][USART_RX_CNT[0]++]=USART_ReceiveData(USART1);
		if(USART_RX_CNT[0]>=USART_REC_LEN)
			USART_RX_CNT[0]=0;
	}
	
	else if(USART_GetITStatus(USART1,USART_IT_IDLE)!=RESET)
	{
		USART1->SR;
		USART1->DR;
		if(LORA_Rec_Msg_Head !=NULL){
			#if gateway
				uint8_t *data_addr = (uint8_t *)malloc(USART_RX_CNT[0]);
				memcpy(data_addr,USART_RX_BUF[0],USART_RX_CNT[0]);
				
				LORA_Rec_Msg_Link_T *node = (LORA_Rec_Msg_Link_T *)malloc(sizeof(LORA_Rec_Msg_Link_T));

//				uint32_t heap_sp = __get_MSP();
//				for(char i=0;i<4;i++){
//					uint8_t data =(uint8_t)(heap_sp>>((3-i)*8));
//				USART_SendData(USART2,data);
//				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);	}
//			
//				uint32_t data_32 = (uint32_t)node;
//				for(char i=0;i<4;i++){
//					uint8_t data =(uint8_t)(data_32>>((3-i)*8));
//				USART_SendData(USART2,data);
//				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);	}
				
				node->data_len = USART_RX_CNT[0];
				node->data_addr = data_addr;
				node->next = NULL;
				
				LORA_Rec_Msg_Link_T *tail = LORA_Rec_Msg_Head;
				while(tail->next != NULL)
					tail = tail->next;
				tail->next = node;
				memset(USART_RX_BUF[0],0x00,USART_REC_LEN);
			#else
			   uint16_t index = 0;
				 
			while(index < USART_RX_CNT[0]){
				if(memcmp(&USART_RX_BUF[0][index+2],terminal_id[0],8)==0){
					uint16_t len = (uint16_t)USART_RX_BUF[0][index+17]|USART_RX_BUF[0][index+18]; 
					len += 21;
					uint8_t *data_addr = (uint8_t *)malloc(sizeof(uint8_t)*len);
					memcpy(data_addr,&USART_RX_BUF[0][index],len);
					
					LORA_Rec_Msg_Link_T *node = (LORA_Rec_Msg_Link_T *)malloc(sizeof(LORA_Rec_Msg_Link_T));
					node->data_len = len;
					node->data_addr = data_addr;
					node->next = NULL;
					
					LORA_Rec_Msg_Link_T *tail = LORA_Rec_Msg_Head;
					while(tail->next != NULL)
						tail = tail->next;
					tail->next = node;
					memset(USART_RX_BUF[0],0x00,USART_REC_LEN);
				}
				else{
					index += (uint16_t)USART_RX_BUF[0][17]<<8 | USART_RX_BUF[0][18];
					index += 21;
				}
			
			}
			#endif
			}
			command_recieved_flag_LORA=1;
			USART_RX_CNT[0]=0;
		}

#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
} 

void USART2_IRQHandler(void)                	//����2�жϷ������
{
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
			if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
		{
			USART_RX_BUF[1][USART_RX_CNT[1]++]=USART_ReceiveData(USART2);
			if(USART_RX_CNT[1]>=USART_REC_LEN)
				USART_RX_CNT[1]=0;
		}
		
		else if(USART_GetITStatus(USART2,USART_IT_IDLE)!=RESET)
		{
			USART2->SR;
			USART2->DR;
			command_recieved_flag_4G=1;
////			if(USART_RX_CNT[1]>=21){
////				if((USART_RX_BUF[1][0]==0x4a)&&(USART_RX_BUF[1][1]==0x54)&&(USART_RX_BUF[1][15]==0x10)&&(USART_RX_BUF[1][16]==0x30))
////				{	
////					Command_Parse(USART_RX_BUF[1]);
////				}
////			
////			}
			USART_RX_CNT[1]=0;

			if(strstr((const char*)&USART_RX_BUF[1],"NO CARRIER")!=NULL)
				ec20_reconnect_flag=1;	
		}
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
} 

void USART3_IRQHandler(void)                	//����3�жϷ������
{
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
			if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)
		{
			USART_RX_BUF[2][USART_RX_CNT[2]++]=USART_ReceiveData(USART3);
			if(USART_RX_CNT[2]>=USART_REC_LEN)
				USART_RX_CNT[2]=0;
		}
		
		else if(USART_GetITStatus(USART3,USART_IT_IDLE)!=RESET)
		{
			USART3->SR;
			USART3->DR;
			command_recieved_flag_ETH=1;
//			if(USART_RX_CNT[2]>=21){
//				if((USART_RX_BUF[2][0]==0x4a)&&(USART_RX_BUF[2][1]==0x54)&&(USART_RX_BUF[2][15]==0x10)&&(USART_RX_BUF[2][16]==0x30))
//				{	
//					Command_Parse(USART_RX_BUF[2]);
//				}
//			
//			}

			USART_RX_CNT[2]=0;
		}
	
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
} 
void UART4_IRQHandler(void)                	//����4�жϷ������
{
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET)
		{
			USART_RX_BUF[3][USART_RX_CNT[3]++]=USART_ReceiveData(UART4);
			if(USART_RX_CNT[3]>=USART_REC_LEN)
				USART_RX_CNT[3]=0;
		}
		
		else if(USART_GetITStatus(UART4,USART_IT_IDLE)!=RESET)
		{
			UART4->SR;
			UART4->DR;
		}
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
} 

void UART5_IRQHandler(void)                	//����5�жϷ������
{
	//u8 res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART5,USART_IT_RXNE)!=RESET)
	{
		USART_RX_BUF[4][USART_RX_CNT[4]++]=USART_ReceiveData(UART5);
		if(USART_RX_CNT[4]>=USART_REC_LEN)
			USART_RX_CNT[4]=0;
	}
	
	else if(USART_GetITStatus(UART5,USART_IT_IDLE)!=RESET)
	{
		UART5->SR;
		UART5->DR;	
	}
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
} 

void USART6_IRQHandler(void)                	//����6�жϷ������
{
	//uint8_t res;
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();    
#endif
		if(USART_GetITStatus(USART6,USART_IT_RXNE)!=RESET)
	{
		USART_RX_BUF[5][USART_RX_CNT[5]++]=USART_ReceiveData(USART6);
		if(USART_RX_CNT[5]>=USART_REC_LEN)
			USART_RX_CNT[5]=0;
if ((device_type_num==2)||(device_type_num==0)){
		if(	USART_RX_BUF[5][0]!=0x42)
				USART_RX_CNT[5]=0;
		}
	}
	
	else if(USART_GetITStatus(USART6,USART_IT_IDLE)!=RESET)
	{
		USART6->SR;
		USART6->DR;
	}
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
#endif
} 

void uart_init_all(u32 baud){
		
//	uart_init(UART4,baud);
//	uart_init(UART5,baud);
	if(device_type_num)
		uart_init(USART6,baud);
}

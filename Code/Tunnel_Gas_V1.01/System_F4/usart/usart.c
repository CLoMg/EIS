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

//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif

//加入以下代码,支持//printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
// __use_no_semihosting was requested, but _ttywrch was referenced
void _ttywrch(int ch){
ch=ch;
}
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
#if serial_port1_debug
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
	USART3->DR = (u8) ch; 	
	return ch;
#endif
}
#endif
 
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[6][USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

u16 USART_RX_CNT[6]={0,0,0,0,0,0};			//接收的字节数	 

uint8_t RS485_EN=0;
//初始化IO 串口1
/********这一块可以简化，为了方便调试，前期先这样，调试完成后简化*******/
Error_Code_t uart_init(USART_TypeDef* USARTx,u32 bound){
	  //根据设备类型定义RS485使能
		if (device_type_num<2)
			 RS485_EN=0x00  ;//bit[0~5] 对应UART1~6的RS485使能  0--Disable  1--Enable
		else if (device_type_num==2)
			 RS485_EN=0x01;
		else
			 RS485_EN=0x21;
		
		Error_Code_t error_code;
		error_code=IS_USART_ALL_PERIPH(USARTx);
		CHECK_ERROR(error_code);
   //GPIO端口设置
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		
		 //USART统一初始化设置
		USART_InitStructure.USART_BaudRate = bound;//波特率设置
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	
	if(USARTx==USART1){
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
	 
		//串口1对应引脚复用映射
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
		
		//USART1端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
		GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10
		
					//如果UART1使用RS485,开启RS485_EN引脚
		if(RS485_EN&0x01){
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //GPIOA8
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //不上下拉
			GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA8
			U1_RS485_TX_EN=0; //RS485设置为接受模式
		}

		USART_Init(USART1, &USART_InitStructure); //初始化串口1
		USART_Cmd(USART1, ENABLE);  //使能串口1 
		USART_ClearFlag(USART1, USART_FLAG_TC);//清除USART_FLAG_TC标志位
		
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
		//Usart1 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		


	}
	else if(USARTx==USART2){
		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
		
		/* 适用于STM32F407探索者开发板的IO配置*/
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD时钟
		//串口1对应引脚复用映射
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); //GPIOD5复用为USART2
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); //GPIOD6复用为USART2
		
		//USART1端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; //GPIOD5与GPIOD6
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
		GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化PD5，PD6
				/* 适用于STM32F407探索者开发板的IO配置*/
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOD时钟
//		//串口1对应引脚复用映射
//		GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOD5复用为USART2
//		GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOD6复用为USART2
//		
//		//USART1端口配置
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOD5与GPIOD6
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
//		GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PD5，PD6
		
							//如果UART1使用RS485,开启RS485_EN引脚
		if(RS485_EN&(0x01<<1)){
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); //使能GPIOD时钟
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //GPIOA2
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //不上下拉
			GPIO_Init(GPIOG,&GPIO_InitStructure); //初始化PA2
			U2_RS485_TX_EN=0; //RS485设置为接受模式
		}	

		USART_Init(USART2, &USART_InitStructure); //初始化串口2
		USART_Cmd(USART2, ENABLE);  //使能串口2
		USART_ClearFlag(USART2, USART_FLAG_TC);//清除USART_FLAG_TC标志位
		
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断
		USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
		//Usart2 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口1中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		

	}
	else if(USARTx==USART3){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOB时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
	 
		//串口3对应引脚复用映射
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOD10复用为USART3
		GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOD11复用为USART3
		
		//USART3端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB10与GPIOB11
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
		GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化PB10，PB11

		USART_Init(USART3, &USART_InitStructure); //初始化串口3
		USART_Cmd(USART3, ENABLE);  //使能串口3
		USART_ClearFlag(USART3, USART_FLAG_TC);//清除USART_FLAG_TC标志位
		
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启相关中断
		USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
		//Usart3 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口3中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	}
	else if(USARTx==UART4){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//使能USART4时钟
				/* 适用于九通PCB板的IO配置*/
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
		//串口4对应引脚复用映射
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4); //GPIOA0复用为UART4
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4); //GPIOA1复用为UART4
		
		//UART4端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //GPIOA0与GPIOA1
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
		GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA0，PA1

		/* 适用于STM32F407探索者开发板的IO配置*/
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOA时钟
//		//串口4对应引脚复用映射
//		GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOA0复用为UART4
//		GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOA1复用为UART4
//		
//		//UART4端口配置
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOA0与GPIOA1
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
//		GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PA0，PA1
		
					//如果UART1使用RS485,开启RS485_EN引脚
		if(RS485_EN&(0x01<<3)){
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //GPIOA2
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //不上下拉
			GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA2
			U4_RS485_TX_EN=0; //RS485设置为接受模式
		}	

		USART_Init(UART4, &USART_InitStructure); //初始化串口4
		USART_Cmd(UART4, ENABLE);  //使能串口4
		USART_ClearFlag(UART4, USART_FLAG_TC);//清除USART_FLAG_TC标志位
			
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启相关中断
		USART_ITConfig(UART4, USART_IT_IDLE, ENABLE);
		//Usart4 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//串口4中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	}
	else if(USARTx==UART5){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOC时钟
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);//使能USART5时钟
	 
		//串口5对应引脚复用映射
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5); //GPIOC12复用为USART5
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5);  //GPIOD2复用为USART5
		
		//USART5端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//GPIOC12
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
		GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PC12
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//GPIOD2
		GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化PD2
		
					//如果UART1使用RS485,开启RS485_EN引脚
		if(RS485_EN&(0x01<<4)){
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //GPIOC11
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //不上下拉
			GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PA8
			U5_RS485_TX_EN=0; //RS485设置为接受模式
		}	
		
		USART_Init(UART5, &USART_InitStructure); //初始化串口5
		USART_Cmd(UART5, ENABLE);  //使能串口5
		USART_ClearFlag(UART5, USART_FLAG_TC);//清除USART_FLAG_TC标志位
		
		USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启相关中断
		USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
		//Usart5 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//串口5中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	}
		else if(USARTx==USART6){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOC时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//使能USART6时钟
	 
		//串口6对应引脚复用映射
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6); //GPIOC6复用为USART6
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); //GPIOC7复用为USART6
		
		//USART1端口配置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOC6与GPIOC7
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
		GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PC6，PC7
			
						//如果UART1使用RS485,开启RS485_EN引脚
		if(RS485_EN&(0x01<<5)){
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //GPIOC8
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //不上下拉
			GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PC8
			U6_RS485_TX_EN=0; //RS485设置为接受模式
		}

		USART_Init(USART6, &USART_InitStructure); //初始化串口6
		USART_Cmd(USART6, ENABLE);  //使能串口6
		USART_ClearFlag(USART6, USART_FLAG_TC);//清除USART_FLAG_TC标志位
		

		USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启相关中断
		USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);
		//Usart6 NVIC 配置
		NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//串口1中断通道
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	}
		
		else 
			error_code=0;
		//如果初始化串口参数正确，先输出一个空字符，避免第一个字节无法通过串口打印的问题
		if(error_code){
			USART_SendData(USARTx,0x00);
			while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//等待发送结束
		  USART_ClearFlag(USARTx, USART_FLAG_TC);             //软件清除发送完成标志位	
		
		}
    return error_code;	
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
//	u8 res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
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

#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 

void USART2_IRQHandler(void)                	//串口2中断服务程序
{
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
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
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 

void USART3_IRQHandler(void)                	//串口3中断服务程序
{
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
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
	
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 
void UART4_IRQHandler(void)                	//串口4中断服务程序
{
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
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
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 

void UART5_IRQHandler(void)                	//串口5中断服务程序
{
	//u8 res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
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
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 

void USART6_IRQHandler(void)                	//串口6中断服务程序
{
	//uint8_t res;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
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
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 

void uart_init_all(u32 baud){
		
//	uart_init(UART4,baud);
//	uart_init(UART5,baud);
	if(device_type_num)
		uart_init(USART6,baud);
}

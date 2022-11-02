#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 



extern _Bool command_recieved_flag_4G;
extern _Bool command_recieved_flag_ETH;
extern _Bool command_recieved_flag_LORA;

extern uint8_t RS485_EN;
#define USART_REC_LEN  		200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收


#define U1_RS485_TX_EN PAout(8)
#define U2_RS485_TX_EN PGout(8)
#define U4_RS485_TX_EN PAout(2)
#define U5_RS485_TX_EN PCout(11)
#define U6_RS485_TX_EN PCout(8)

extern u16  USART_RX_CNT[6];//各串口接收数据数量
extern u8  USART_RX_BUF[6][USART_REC_LEN]; //接收缓冲区,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

Error_Code_t uart_init(USART_TypeDef* USARTx,u32 bound);
void uart_init_all(u32 baud);

#endif



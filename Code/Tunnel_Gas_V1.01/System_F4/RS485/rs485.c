#include "rs485.h"
#include <string.h>
#include "delay.h"
#include <stdarg.h>

void RS485_GPIO_Init(uint8_t usart_num,uint32_t baud){
	USART_TypeDef* USARTx;
	switch(usart_num){
		case 1:USARTx=USART1;break;
		case 2:USARTx=USART2;break;
		case 3:USARTx=USART3;break;
		case 4:USARTx=UART4;break;
		case 5:USARTx=UART5;break;
		case 6:USARTx=USART6;break;
		default:return;
	}
	uart_init(USARTx,baud);
}
uint16_t RS485_CRC16_Calculate(uint8_t *buffer,uint8_t length){
  uint16_t  wCRCin = 0xFFFF;
  uint16_t wCPoly = 0xA001;
  uint8_t wChar = 0;
	
  while (length--) 	
  {
        wChar = *(buffer++);
		
        wCRCin ^= wChar;
				//wCRCin = (wCRCin>>1)&(0x01<<15);
        for(int i = 0;i < 8;i++)
        {
          if(wCRCin & 0x0001)
            wCRCin = (wCRCin >> 1) ^ wCPoly;
          else
            wCRCin = wCRCin >> 1;
        }
  }
  return (wCRCin) ;
}

Error_Code_t RS485_Send_Data(uint8_t usart_num,uint8_t *Send_buffer,uint8_t length){
	Error_Code_t Error=1;
	USART_TypeDef* USARTx;
	uint16_t i;
	switch (usart_num){
		case 1: USARTx = USART1;U1_RS485_TX_EN=1;break;
		case 2: USARTx = USART2;U2_RS485_TX_EN=1;break;
		case 3: USARTx = USART3;break;
		case 4: USARTx = UART4;U4_RS485_TX_EN=1;break;
		case 5: USARTx = UART5;U5_RS485_TX_EN=1;break;
		case 6: USARTx = USART6;U6_RS485_TX_EN=1;break;
    default: Error = 0;
	}
		
	
	for(i=0;i<length;++i){
		USART_SendData(USARTx,Send_buffer[i]);
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//µÈ´ý·¢ËÍ½áÊø
	}
	switch (usart_num){
			//CH4=1,CO2=4,SO2=5,NH3=6,
		case 1: U1_RS485_TX_EN=0;break;
		case 2: U2_RS485_TX_EN=0;break;
		case 4: U4_RS485_TX_EN=0;break;
		case 5: U5_RS485_TX_EN=0;break;
		case 6: U6_RS485_TX_EN=0;break;
    default: Error = 0;
	}
	return Error;
}

uint8_t RS485_Receive_Data(uint8_t usart_num,uint8_t  *data){

	uint8_t data_length=USART_RX_CNT[usart_num-1];
	if(data_length>0){
		 memcpy(data,&USART_RX_BUF[usart_num-1],data_length);
		 }
	USART_RX_CNT[usart_num-1]=0;
	return data_length;
}

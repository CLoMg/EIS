#include "rs485.h"
#include <string.h>

#include "delay.h"
#include <stdarg.h>


//uint16_t RS485_CRC16_Calculate(uint8_t *buffer,uint8_t length);
Error_Code_t RS485_Get_Value(enum gas_type gas,uint8_t device_addr,uint16_t memory_addr,uint16_t *data){
	  Error_Code_t Error=0;
		uint8_t write_buff[8]={0x00,0x03,0x00,0x00,0x00,0x01};
		uint16_t CRC16;
		
		write_buff[0]=device_addr;
		write_buff[2]=(uint8_t)(memory_addr>>8);
		write_buff[3]=(uint8_t)memory_addr;
		
    CRC16=RS485_CRC16_Calculate(write_buff,6);
		write_buff[7]=(uint8_t)(CRC16>>8);
		
		write_buff[6]=(uint8_t)CRC16;
		
		//RS485_Send_Data(write_buff,8);
		Error=RS485_Send_Data(gas,write_buff,8);
		
		delay_ms(200);//应该等待多久才开始读取？
		
		Error=RS485_Receive_Data(gas,data);	 
		return Error;
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

Error_Code_t RS485_Send_Data(enum gas_type gas,uint8_t *Send_buffer,uint8_t length){
	Error_Code_t Error=1;
	USART_TypeDef* USARTx;
	uint16_t i;
	switch (gas){
		case CH4: USARTx = USART1;U1_RS485_TX_EN=1;break;
		case NH3: USARTx = UART4;U4_RS485_TX_EN=1;break;
		case CO: USARTx = UART5;U5_RS485_TX_EN=1;break;
		case CO2: USARTx = USART6;U6_RS485_TX_EN=1;break;
    default: Error = 0;
	}
		
	
	for(i=0;i<length;++i){
		USART_SendData(USARTx,Send_buffer[i]);
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);//等待发送结束
	}
	switch (gas){
			//CH4=1,CO2=4,SO2=5,NH3=6,
		case CH4: U1_RS485_TX_EN=1;break;
		case NH3: U4_RS485_TX_EN=1;break;
		case CO: U5_RS485_TX_EN=1;break;
		case CO2: U6_RS485_TX_EN=1;break;
    default: Error = 0;
	}
	return Error;
}

Error_Code_t RS485_Receive_Data(enum gas_type gas,uint16_t  *data){
	Error_Code_t Error=0;
	uint8_t Read_buffer[10];
	uint16_t gas_concentation;
	uint16_t CRC16_Read,CRC16_Calc;
	 if(USART_RX_CNT[gas-1]>=7){
		 memcpy(Read_buffer,&USART_RX_BUF[gas-1],7);
		 CRC16_Read = (uint16_t)Read_buffer[6]<<8 | Read_buffer[5];
		 CRC16_Calc=RS485_CRC16_Calculate(Read_buffer,5);
		 if(CRC16_Calc==CRC16_Read){
				gas_concentation = ((uint16_t)Read_buffer[3]<<8)+Read_buffer[4];
	//测试时输出至串口1，后续添加输出接口
			 *data = gas_concentation;
			 //printf("value of RS485 : %x\n",gas_concentation);
			 Error=1;
		 }
//		 RS485_TX_EN=1;
		 USART_RX_CNT[gas-1]=0;
	}
	 return Error;
}

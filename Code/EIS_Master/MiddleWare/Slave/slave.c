#include "slave.h"

#include "spi.h"
#include "gd32f10x_eval.h"
#include "timer.h"
#include "msg_handler.h"
#include "string.h"

#define ReadData  0x52 
#define WriterData 0x57

static uint8_t Communicate_Failure_Times = 0x00;


void Slave_Init(void)
{
	/*Parameter config*/
	SPI_Init();

	/*Instance structure initialize*/

	/*communication interface initialize*/
	
	/*timer initialize*/
	
	/*FSM Initialize*/

}

void Slave_Data_Read(void){
	



}

void Slave_Register_Read_Single(uint8_t addr,uint8_t *read_buff){
	


}
void Slave_Register_Read_Continues(uint8_t start_addr,uint8_t len,uint8_t *read_buff){
	
	uint8_t i=0;
	NSS_L;
//	read_buff[i++] = SPI_ReadWriteByte(WriterData);
//	read_buff[i++] = SPI_ReadWriteByte(start_addr);
//	read_buff[i++] = SPI_ReadWriteByte(0x0D);
//	read_buff[i++] = SPI_ReadWriteByte(0x0A);
//	delay_ms(2000);
	
	read_buff[i++] = SPI_ReadWriteByte(ReadData);
	read_buff[i++] = SPI_ReadWriteByte(start_addr);
	for(i=0;i<len;++i)
		read_buff[i+2] = SPI_ReadWriteByte(0xFF);
//  read_buff[i+5] = SPI_ReadWriteByte(0x0D);
//	i++;
//	read_buff[i+5] = SPI_ReadWriteByte(0x0A);
	NSS_H;
}

void Slave_Register_Write_Single(uint8_t addr,uint8_t *read_buff){
	


}
void Slave_Register_Write_Continues(uint8_t start_addr,uint8_t len,uint8_t *write_buff){
	uint8_t i=0;
	uint8_t  dummy_buff[20] = {0,};
	NSS_L;
	dummy_buff[i++] = SPI_ReadWriteByte(WriterData);
	dummy_buff[i++] =SPI_ReadWriteByte(start_addr);
	for(i=0;i<len;++i)
		dummy_buff[i+2] = SPI_ReadWriteByte(write_buff[i]);
	NSS_H;
}
void Slave_Process(){

		uint8_t rxdata[20]= {0,};
		uint8_t len=8;
	
		Slave_Register_Read_Continues(0x00,len,rxdata);
		
		if(LRC_Value_Calc(&rxdata[2],len-1) == rxdata[len+2-1]){
			Dev_Status_Struct.status |= 0x01;
			Dev_Status_Struct.data_len = len;
			if(Dev_Status_Struct.data_addr != NULL)
				free(Dev_Status_Struct.data_addr);
			Dev_Status_Struct.data_addr = (uint8_t *)malloc(sizeof(uint8_t)*len);
			Dev_Status_Struct.data_addr[0] = Dev_Status_Struct.status;
			memcpy(&Dev_Status_Struct.data_addr[1],&rxdata[2],len-1);
		}
		else{
			Dev_Status_Struct.status &= 0xFE;
			Dev_Status_Struct.data_len = 1;
			if(Dev_Status_Struct.data_addr != NULL)
				free(Dev_Status_Struct.data_addr);
			Dev_Status_Struct.data_addr = (uint8_t *)malloc(sizeof(uint8_t)*len);
			Dev_Status_Struct.data_addr[0] = Dev_Status_Struct.status;		
			if(Communicate_Failure_Times > 10){
				Communicate_Failure_Times = 0 ;
				Slave_Reset();
			}
		}
}

unsigned char LRC_Value_Calc(unsigned char *data_buff,unsigned char len)
{
	uint16_t lrc  = 0;
	uint16_t i=0;
	for(;i<len;++i){
		lrc += data_buff[i];
	}
	lrc = ~lrc + 1;
	return (uint8_t)(lrc & 0xff);
}

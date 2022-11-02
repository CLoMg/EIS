#include "uestc_gas.h"
#include "modbus.h"
#include "string.h"

//������GPIO��ʼ������Ҫ�ǳ�ʼ��RS485 A\B\TR��������
void UESTC_Gas_GPIO_Init(uint8_t usart_num,uint32_t baud){
	ModBus_GPIO_Init(usart_num,baud);
}

//��������ʼ��
void UESTC_Gas_Init(uint8_t usart_num,uint32_t baud){
	UESTC_Gas_GPIO_Init(usart_num,baud);
}

//����������Ũ�ȶ�ȡ usart_num���ô��ڱ�� slave_addr �ӻ���ַ data_buff���ݴ洢λ����ʼ��ַ
Error_Code_t UESTC_Gas_Conc_Read(uint8_t usart_num,uint8_t slave_addr,uint8_t *data_buff){
	Error_Code_t error_code=0;
	uint8_t read_buff[2]={0};
	 if(ModBus_RTU_Read_Holding_Register(usart_num,slave_addr,0x08,1,read_buff)){
		 memcpy(data_buff,read_buff,2);
		 error_code=1;
	 }
	 else{
		 data_buff[0]=0xff;
		 data_buff[1]=0xff;
	 }
	 return error_code;
}

//�������豸ID��ȡ
void UESTC_Gas_ID_Read(uint8_t usart_num,uint8_t slave_addr,uint8_t *data_buff){
	uint8_t read_buff[2]={0};
	 if(ModBus_RTU_Read_Holding_Register(usart_num,slave_addr,0x02,1,read_buff))
		 memcpy(data_buff,read_buff,2);
	 else{
		 data_buff[0]=0xff;
		 data_buff[1]=0xff;
	 }
}



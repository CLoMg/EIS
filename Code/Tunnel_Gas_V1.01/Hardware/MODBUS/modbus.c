#include "modbus.h"
#include "rs485.h"
#include "string.h"
#include "delay.h"


//ModBus GPIO��ʼ������Ҫ�ǳ�ʼ��RS485��A\B\TR��������
//�����������
//���أ���
void ModBus_GPIO_Init(uint8_t usart_num,uint32_t baud){
	RS485_GPIO_Init(usart_num,baud);
}

//ModBus-RTUЭ��д�뵥���Ĵ��� 
//���������usart_num ���ô��ڱ��  slave_addr�ӻ���ַ register_addr��д��ļĴ�����ַ data��д���16λ����
//���أ���
void ModBus_RTU_Write_Single_Holding_Register(uint8_t usart_num,uint8_t slave_addr,uint16_t register_addr,uint16_t data){
	uint8_t Write_buff[8]={0};
	uint16_t CRC_Calc_Out=0;
  enum ModBus_Func_Code_Type Func_Code=Preset_Single_Register;
	Write_buff[0]=slave_addr;
	Write_buff[1]=Func_Code;
	Write_buff[2]=(uint8_t)(register_addr>>8);
	Write_buff[3]=(uint8_t)register_addr;
	Write_buff[4]=(uint8_t)(data>>8);
	Write_buff[5]=(uint8_t)data;
	CRC_Calc_Out=ModBus_CRC16_Calculate(Write_buff,6);
	Write_buff[6]=(uint8_t)(CRC_Calc_Out>>8);
	Write_buff[7]=(uint8_t)CRC_Calc_Out;

	ModBus_Data_Send(usart_num,Write_buff,8);
	
}

//ModBus-RTU Э���ȡ�Ĵ�������
//���������usart_num ���ô��ڱ��  slave_addr�ӻ���ַ register_addr��ʼ�ļĴ�����ַ read_number ��ȡ�Ĵ���������ÿ���Ĵ����洢2Bytes���ݣ�, read_point��ȡ���ݴ�ŵĿռ��׵�ַ
//���أ������� 1-��ȡ�ɹ� 0-��ȡʧ��
Error_Code_t ModBus_RTU_Read_Holding_Register(uint8_t usart_num,uint8_t slave_addr,uint16_t start_register,uint16_t read_number,uint8_t *read_point){
	Error_Code_t error_code=0;
	uint8_t read_buff[60]={0};
	uint8_t dummy_buff[1],write_length=0,read_length=0;
  enum ModBus_Func_Code_Type Func_Code=Read_Holding_Registers;
	ModBus_RTU_Command_Packet(usart_num,slave_addr,Func_Code,start_register,read_number, dummy_buff,write_length);
	delay_ms(100);
	
	read_length=ModBus_Data_Receive(usart_num,read_buff);
	if(read_length>0){
		error_code=ModBus_Data_Decode(read_buff,read_length);
		if(error_code){
			if((read_buff[0]==slave_addr)&&(read_buff[1]==Func_Code)&&(read_buff[2]==read_number*2))
				memcpy(read_point,&read_buff[3],read_number*2);
	    else
				error_code =0;
		}
		
	}
	return error_code;
}

//ModBus-RTUЭ��ָ��������
//������ָ������ݼ���У��Ͳ��������
//���أ���
void ModBus_RTU_Command_Packet(uint8_t usart_num,uint8_t Slave_Addr,enum ModBus_Func_Code_Type Func_Code,uint16_t Start_register,uint16_t Register_Number,uint8_t *Send_Buff,uint8_t Write_Num){
	uint8_t send_packet[9+Write_Num];
	uint8_t i=6;
	ModBusStruct ModBus_Structure;
	ModBus_Structure.slaveaddr = Slave_Addr;
	ModBus_Structure.func_code = Func_Code;
	ModBus_Structure.register_addr[0] = (uint8_t)(Start_register>>8);
	ModBus_Structure.register_addr[1] = (uint8_t)Start_register;
	ModBus_Structure.register_num[0] = (uint8_t)(Register_Number>>8);
	ModBus_Structure.register_num[1] = (uint8_t)Register_Number;
	
  memcpy(send_packet,&ModBus_Structure,6);
	if(Write_Num>0){
		send_packet[i++] = Write_Num;
		memcpy(&send_packet[i],Send_Buff,Write_Num);
		i+=Write_Num;
	}
	uint16_t CRC_Out = ModBus_CRC16_Calculate(send_packet,i);
	ModBus_Structure.crc_code[0] = (uint8_t)(CRC_Out>>8);
	ModBus_Structure.crc_code[1] = (uint8_t)CRC_Out;
	memcpy(&send_packet[i],ModBus_Structure.crc_code,2);
	
	ModBus_Data_Send(usart_num,send_packet,i+2);

}

//ModBus-RTU �������ݽ���  
//���������data_buff���ݻ�������ʼ��ַ,length ���ݳ���
//���أ��������ݸ�ʽ�Ƿ���ȷ  1-��ȷ 0-����
Error_Code_t ModBus_Data_Decode(uint8_t *data_buff,uint8_t length){
	Error_Code_t error_code=0;
	uint16_t CRC_Calc_Out = ModBus_CRC16_Calculate(data_buff,length-2);
	if(((uint8_t)(CRC_Calc_Out>>8)==data_buff[length-1])&&((uint8_t)CRC_Calc_Out==data_buff[length-2]))
		error_code=1;
	
	return error_code;
}

//ModBus-RTU ��������
//���������usart_num ���մ��ڱ��,read_buff������Ļ����׵�ַ
//���أ����ݳ���
uint8_t ModBus_Data_Receive(uint8_t usart_num,uint8_t *read_buff){
	uint8_t  data_length=0;
	uint8_t data_buff[60]={0};
	data_length = RS485_Receive_Data(usart_num,data_buff);
	if(data_length>0)
		memcpy(read_buff,data_buff,data_length);
	return data_length;
}

//ModBus-RTU ��������
//���������Send_Buffer ���������׵�ַ length���������ֽڳ���
void ModBus_Data_Send(uint8_t usart_num,uint8_t *Send_Buffer,uint8_t length){
	RS485_Send_Data(usart_num,Send_Buffer,length);
}

//ModBus-RTU CRC16У�����
//���������buffer��У�������׵�ַ length���ݳ���
//���أ�16λУ����
uint16_t ModBus_CRC16_Calculate(uint8_t *buffer,uint8_t length){
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

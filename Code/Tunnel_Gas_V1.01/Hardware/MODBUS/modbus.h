#ifndef __MODBUS_H
#define __MODBUS_H
#include "sys.h"

enum ModBus_Func_Code_Type{
	Read_Coil_Status=0x01,Read_Input_Status,Read_Holding_Registers,Read_Input_Registers,
	Force_Single_Coil,Preset_Single_Register,Read_Exception_Status,Fetch_Comm_Event_ctr=0x0B,
	Fetch_Comm_Event_Log,Force_Multiple_Coils=0x0F,Preset_Multiple_Regs,Report_Slave_ID,
	Read_General_Reference=0x14,Write_General_Reference,Mask_Write_4X_Register,Read_Write_4X_Registers,
	Read_FIFO_Queue,
};


typedef struct {
	uint8_t slaveaddr;
  enum ModBus_Func_Code_Type func_code;
	uint8_t register_addr[2];
	uint8_t register_num[2];
	uint8_t write_num;
	uint8_t crc_code[2];
}ModBusStruct;

void ModBus_GPIO_Init(uint8_t usart_num,uint32_t baud);

void ModBus_RTU_Write_Single_Holding_Register(uint8_t usart_num,uint8_t slave_addr,uint16_t register_addr,uint16_t data);

Error_Code_t ModBus_RTU_Read_Holding_Register(uint8_t usart_num,uint8_t slave_addr,uint16_t start_register,uint16_t read_number,uint8_t *read_point);

void ModBus_RTU_Command_Packet(uint8_t usart_num,uint8_t Slave_Addr,enum ModBus_Func_Code_Type Func_Code,uint16_t Start_register,uint16_t Register_Number,uint8_t *Send_Buff,uint8_t Write_Num);

Error_Code_t ModBus_Data_Decode(uint8_t *data_buff,uint8_t length);

uint8_t ModBus_Data_Receive(uint8_t usart_num,uint8_t *read_buff);

void ModBus_Data_Send(uint8_t usart_num,uint8_t *Send_Buffer,uint8_t length);
uint16_t ModBus_CRC16_Calculate(uint8_t *buffer,uint8_t length);

#endif

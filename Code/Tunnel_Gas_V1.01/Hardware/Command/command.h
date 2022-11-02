#ifndef __COMMAND_H
#define __COMMAND_H
#include "sys.h"

typedef  void (*iapfun)(void);				//定义一个函数类型的参数.
#define Sofeware_Version 0x000E000000000003
extern _Bool Query_flag;
#define Data_head 0x4A54 //定义数据头
//#define Serial_num 0X0000
#define Encry_mode 0x00  //定义加密方式
#define func_code_type 0x0E //定义功能指令码类别  0x0E--隧道综合感知  

enum func_code_flag    //指令功能码标志
{
	platform_issue = 0x01, device_return, device_upload, 
};

enum Command_type{
	device_init = 0x0001, set_clock, read_version, update_program=0x1030, 
	update_prog_send, update_result, query_data, active_upload, set_upload_sequence,set_terminal_mac_addr,
	query_terminal_mac_addr,set_g_lora_speed=0x1038,platform_request_g_lora_speed,set_g_lora_channel,
	platform_request_g_lora_channel,set_g_lora_speed_channel,platform_request_g_lora_speed_ch,server_addr_modify=0xFFFE,gateway_upgrade_cmd=0x2003,
};

enum instru_exe_state
{
  checksum_error=0x0000, invalid_func_code, data_len_err, 
	data_form_err, pri_subequip_commu_err, sec_subequip_commu_err, thr_subequip_commu_err,
	four_subequip_commu_err, input_secretkey_err,
	
};
/*&sensor_type 每一位对应一种传感器 1-安装 0-未安装 
_____________________________________________
|			         |Usartx | 00 | 01 | 10  | 11 |
---------------|-------|----|----|-----|----|
|s_t[1]Bits1~0 |   U1  |	  | CH4| CO  |		|
---------------|-------|----|----|-----|----|
|s_t[1]Bits3~2 |   U4  |		| SO2| NH3 |		|
|--------------|-------|----|----|-----|----|
|s_t[1]Bits5~4 |   U5  |		| H2S| NO2 |		|
|--------------|-------|----|----|-YI--|----|
|s_t[1]Bits7~6 |	 U6	 |		| CO2|PM2.5|		|
-------------------------------------------*/
typedef struct {
	uint8_t data_head[2];
	uint8_t device_id[8];
	uint8_t serial_num[2];
	uint8_t encry_mode;
	uint8_t func_code[4];  
	uint8_t data_length[2];
	//uint8_t sensor_type[2];  
	uint8_t data_buff[50];
	uint8_t crc16[2];
}Data_Packet_t;

//extern Data_Packet_t  *Data_package;

void Command_Parse(uint8_t *data);
void iap_load_app(u32 appxaddr);
uint16_t Check_Sum_Cal(uint8_t *data,uint8_t length);
void Data_Package_init(void);
void Data_Package(uint8_t length,Data_Packet_t *Data_package);
void Data_Send(Data_Packet_t *Data_package);
void Data_Unpack(uint8_t *data,Data_Packet_t *Data_package);
void Sensor_Data_Update(void);	
void Data_Upload(uint8_t terminal_num);
void Data_Acquire(uint8_t terminal_num);
void Sensor_Aquire(Data_Packet_t *p);
void Device_return(uint8_t instruc_func_flag,uint16_t instruction);
void Return_Data_State(Data_Packet_t *p,uint8_t instruc_func_flag,uint16_t instruc_exe_state);
void Return_Instru_Error(Data_Packet_t *p, enum instru_exe_state error_type);
void Read_Software_Version(Data_Packet_t *p);

#endif

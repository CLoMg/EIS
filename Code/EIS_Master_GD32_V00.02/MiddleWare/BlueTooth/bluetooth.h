
#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#define MAX_BLE_BUFF_SIZE 255
typedef enum{
	TX = 0, RX=1
}Ble_Uart_Status_T;
typedef enum{
	BR_2400 = 0, BR_4800 = 1, BR_9600 = 2,
	BR_19200, BR_38400, BR_57600, BR_115200,
}BaudRate_Typedef;

typedef enum{
	DataBits_5b = 5, DataBits_6b, DataBits_7b =7,Data_Bits_8b
}Data_Bits_Typedef;

typedef enum{
	None = 0, Odd, Even, Mark, Space
}Parity_Bits_Typedef;

typedef enum{
	Stop_Bits_1b = 0, Stop_Bits_1_5b, Stop_Bits_2b
}Stop_Bits_Typedef;

typedef struct Uart_Config_S{
	BaudRate_Typedef baudrate;
	Data_Bits_Typedef data_bits;
	Parity_Bits_Typedef parity_bits;
	Stop_Bits_Typedef stop_bits;
}Uart_Cfg_T;

typedef enum{
	Idle = 0x00, Broadcast = 0x01, Connected = 0x02,
	Scan_With_Respon = 0x03, Scan_Without_Respon = 0x04
}Module_Status_Typedef;

typedef enum{
	cmd_exe_success = 0x01,reply_timeout = 0x02, Pre_op_not_finish = 0x06, 
	invalid_para, module_disconnect,module_connecting, 
	module_broadcasting, operate_failed,
}Cmd_Error_Return_Typedef;


typedef struct AdvInt_s{
	unsigned short Min;
	unsigned short Max;
}AdvInt_T;

typedef struct Connint_s{
	unsigned short Min;
	unsigned short Max;
	unsigned short Latency;
	unsigned short Timeout;
}ConnInt_T;

typedef struct BLE_Param_s{
	Uart_Cfg_T Uart_Config;
	Module_Status_Typedef status;
	AdvInt_T advint;
	ConnInt_T connint;
	Cmd_Error_Return_Typedef error_code;
	char *blename;
	char *addr;
}BLE_Param_T;

void BlueTooth_Init(void);
void BlueTooth_IO_Config(void);
void BlueTooth_default_Para_Config(BaudRate_Typedef baudrate,char *blename,char name_len,char *addr,char addr_len, ConnInt_T connint,AdvInt_T advint);
Cmd_Error_Return_Typedef BlueTooth_Set_BR(BaudRate_Typedef baudrate);
Cmd_Error_Return_Typedef BlueTooth_Get_Name(char *blename_point);
Cmd_Error_Return_Typedef BlueTooth_Set_Name(char *blename_buff,char name_len);
Cmd_Error_Return_Typedef BlueTooth_Get_Addr(char *bleaddr_point);
Cmd_Error_Return_Typedef BlueTooth_Set_Addr(char *bleaddr_buff,char name_len);
Cmd_Error_Return_Typedef BlueTooth_Get_Status(char *status_point);
Cmd_Error_Return_Typedef BlueTooth_Set_aAdvInt(AdvInt_T advint);
Cmd_Error_Return_Typedef BlueTooth_Set_ConnInt(ConnInt_T connint);
Cmd_Error_Return_Typedef BlueTooth_DisConnect(void);
Cmd_Error_Return_Typedef BlueTooth_ReStart(void);
Cmd_Error_Return_Typedef BlueTooth_Exit_CMD_Mode(void);
Cmd_Error_Return_Typedef BlueTooth_Enter_CMD_Mode(void);
Cmd_Error_Return_Typedef BlueTooth_Shutdown(void);
Cmd_Error_Return_Typedef BlueTooth_Restore(void);
Cmd_Error_Return_Typedef BlueTooth_Get_Info(char *info_point);
void BlueTooth_Get_Help(void);
void BlueTooth_Get_Para(void);
Cmd_Error_Return_Typedef Ble_Cmd_Write_Read(char *cmd_str,char cmd_len ,char *return_data,unsigned short timeout);
void BlueTooth_Data_Send(char *data_send,unsigned short data_len);
#endif

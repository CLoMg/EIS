#ifndef __PMS7003_H
#define __PMS7003_H

typedef struct
{
	
	unsigned short dataLen;
	unsigned short dataLenPre;
	
	unsigned char buf[100];

} PMS_IO_INFO;

extern unsigned short PM25_Data;
enum PSM_CMD{
	State_Chanage=0xE1,Data_Read=0xE2,Standby_Ctl=0xE4
};
enum update_state{
		passive=0x00,acitive
};
enum standby_mode{
		standby=0x00,normal
};

void  PMS_init(void);
_Bool PMS_Data_Read(unsigned char *data);
unsigned short PMS_LRC_Check(unsigned char *data_buffer,unsigned char length);
void PMS_State_Change(enum update_state newstate);
void PMS_Send_CMD(enum PSM_CMD cmd,unsigned char state_bytes);
_Bool PMS_Receive_Data(void);
void PMS_GPIO_Config(void);
void PMS_IO_Init(void);
void PMS_IO_Send(unsigned char *str, unsigned short len);
_Bool PMS_IO_WaitRecive(void);
void PMS_IO_ClearRecive(void);
void PMS_UARTEx_RxEventCallback(unsigned short Size);
#endif

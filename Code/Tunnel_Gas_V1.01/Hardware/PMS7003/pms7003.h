#ifndef __PMS7003_H
#define __PMS7003_H
#include "sys.h"


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
Error_Code_t PMS_Data_Read(uint8_t *data);
uint16_t PMS_LRC_Check(uint8_t *data_buffer,uint8_t length);
void PMS_State_Change(enum update_state newstate);
void PMS_Send_CMD(enum PSM_CMD cmd,uint8_t state_bytes);
Error_Code_t 	PMS_Receive_Data(void);
void PMS_GPIO_Config(void);
#endif

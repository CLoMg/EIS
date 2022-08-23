#ifndef GAS_H
#define GAS_H

#define Delta_Max  5
#define MAX_GAS_BUFF_SIZE 255
//typedef struct Temp_Queue_s{
//	unsigned char Queue_len;
//	unsigned short temp_data;
//	struct temp_queue_s *next;
//}Temp_Queue_T;
typedef enum
{
	active_mode = 0x40, passivity_mode = 0x41
}Trans_Mode_T;
typedef enum
{
	defalut = 0x00, idle, data_acquire, info_query
}Gas_FSM_State_T;
typedef enum{
	init = 0x00, acquire,reply_valid,timeout,query_info
}Gas_FSM_Events_T;
typedef enum
{
	Gas_Commu_Idle = 0x00, Gas_Commu_TX = 0x01, Gas_Commu_RX,
}Commu_States_T;
typedef enum
{
	HCHO = 0x17, VOC, CO, CI2, H2, H2S, 
	HCI, HCN, HF, NH3, NO2, O2, O3, SO2
}Sensor_Type_T;
typedef struct Gas_S
{
	Trans_Mode_T tran_mode;
	Sensor_Type_T sensor_type;
	unsigned short range;
	unsigned char unit;
	unsigned char sign;
	unsigned short cur;
	unsigned short pre;
	unsigned short delta;
	unsigned short buff[5];
	float temp;
	float humi;
	unsigned short mean;
	unsigned char  alar_count;
}Gas_T;


void Gas_Init(void);
static void Gas_IO_Init(void);
/*structure config*/
static void Gas_Param_Config(Gas_T *instance);
extern void Gas_Data_Acquire(void);
static void Gas_Data_Calc(void);
static void Gas_Trans_Mode_Change(Trans_Mode_T mode);
_Bool Gas_Cmd_Write_Read(char *cmd_str,char cmd_len ,char *return_data,unsigned short timeout);
void Gas_Data_Send(char *data_send,unsigned short data_len);
unsigned char  CheckSum_Calc(char *packet,unsigned char length);
/**
  * @brief ADC Initialization Function
  * @retval None
  */
static void ADC_Init(void);

#endif
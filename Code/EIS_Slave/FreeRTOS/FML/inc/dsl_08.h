#ifndef __DSL08_H
#define __DSL08_H

#define Delta_Max 5

enum DSL_CMD{
	Start_Up=0x01,PM25_PM10_Read,Shut_Down,Partice_Read,_32_Bytes_Read,
	Start_Continuous_Measure,Stop_Continuous_Measure,Continuous_Measure_Changeless,Continuous_Measure_Relieve,
	Current_Mode_Read,Change_to_I2C=0x0C,
};

typedef enum DSL_Sensor_Mode{
	DSL_QA = 0x00,DSL_Continue
}DSL_Mode_T;
typedef struct DSL_Sensor
{
	DSL_Mode_T Mode;
	unsigned int Usart_Periph;
	unsigned char Usart_Channel;
	unsigned int Usart_BaudRate;
	unsigned char *Usart_buff_point;
	unsigned int *Usart_RX_NUM_Point;
	unsigned char  Index;
	unsigned short Cur;
	unsigned short Pre;
	unsigned short Delta;
	unsigned short Buff[5];
	unsigned short Mean;
	unsigned char  Alar_Count;
}DSL_Sensor_T;

extern DSL_Sensor_T *dsl_instance;
//enum DSL_update_state{
//		passive=0x00,acitive,
//};
//enum DSL_standby_mode{
//		standby=0x00,normal,
//};


void  DSL_init(unsigned char usart_num,unsigned short buad_rate);
void AQI_Process(void);
void DSL_GPIO_Config(void);
void DSL_Sensor_Init();
void DSL_Mode_Change(DSL_Sensor_T *instance);
_Bool DSL_Data_Read(DSL_Sensor_T *instance);
void DSL_Send_CMD(DSL_Sensor_T *instance,enum DSL_CMD cmd);
_Bool DSL_Receive_Data(DSL_Sensor_T *instance);
unsigned short DSL_LRC_Check(unsigned char *data_buffer,unsigned char length);

#endif

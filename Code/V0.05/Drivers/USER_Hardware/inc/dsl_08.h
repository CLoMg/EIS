#ifndef __DSL08_H
#define __DSL08_H


extern  unsigned char PM_Buff[10];
typedef struct
{
	
	unsigned short dataLen;
	unsigned short dataLenPre;
	
	unsigned char buf[100];

} PM_IO_INFO;

enum PM_CMD{
	Start_Up=0x01,PM25_PM10_Read,Shut_Down,Partice_Read,_32_Bytes_Read,
	Start_Continuous_Measure,Stop_Continuous_Measure,Continuous_Measure_Changeless,Continuous_Measure_Relieve,
	Current_Mode_Read,Change_to_I2C=0x0C,
};

//enum PM_CMD{
//	PM25_PM10_Read = 0x0B,Stop_Continuous_Measure = 0x0C,
//};

typedef enum PM_Sensor_Mode{
	PM_QA = 0x00,PM_Continue
}PM_Mode_T;

typedef struct PM_Data{
	unsigned char  Delta_MAX;
	unsigned char  Index;
	unsigned short Cur;
	unsigned short Pre;
	unsigned short Delta;
	unsigned short Buff[5];
	unsigned short Mean;
	unsigned char  Alar_Count;
}PM_Data_T;

typedef struct PM_Sensor
{
	PM_Mode_T Mode;
	//unsigned int Usart_Periph;
	unsigned char Usart_Channel;
	unsigned int Usart_BaudRate;
	unsigned char *Usart_buff_point;
	unsigned int *Usart_RX_NUM_Point;
	PM_Data_T *PM25;
	PM_Data_T *PM10;
}PM_Sensor_T;

extern int PM25_Data,PM10_Data;
//typedef struct PM_Sensor
//{
//	PM_Mode_T Mode;
//	unsigned int Usart_Periph;
//	unsigned char Usart_Channel;
//	unsigned int Usart_BaudRate;
//	unsigned char *Usart_buff_point;
//	unsigned int *Usart_RX_NUM_Point;
//	unsigned char  Index;
//	unsigned short Cur;
//	unsigned short Pre;
//	unsigned short Delta;
//	unsigned short Buff[5];
//	unsigned short Mean;
//	unsigned char  Alar_Count;
//}PM_Sensor_T;

extern PM_Sensor_T *PM_instance;
//enum PM_update_state{
//		passive=0x00,acitive,
//};
//enum PM_standby_mode{
//		standby=0x00,normal,
//};


void  PM_init(unsigned char usart_num,unsigned short buad_rate);
void AQI_Process(void);
void PM_GPIO_Config(void);
void PM_Sensor_Init(void);
void PM_Mode_Change(PM_Sensor_T *instance);
_Bool PM_Data_Read(void);
void PM_Send_CMD(enum PM_CMD cmd);
_Bool PM_Receive_Data(void);
unsigned short PM_LRC_Check(unsigned char *data_buffer,unsigned char length);
static void PM_Data_Calc(PM_Data_T *sensor_data);
void PM_IO_Send(unsigned char *str, unsigned short len);
_Bool PM_IO_WaitReceive(void);
void PM_IO_ClearReceive(void);

void PM_UARTEx_RxEventCallback(unsigned short Size);
unsigned char DS08_CS_Check(unsigned char *data_buffer,unsigned char length);
_Bool DS_Receive_Data(void);
_Bool DS_Data_Read(void);
#endif

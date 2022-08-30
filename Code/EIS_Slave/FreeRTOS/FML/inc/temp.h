#ifndef TEMP_H
#define TEMP_H

#define Delta_Max  5

//typedef struct Temp_Queue_s{
//	unsigned char Queue_len;
//	unsigned short temp_data;
//	struct temp_queue_s *next;
//}Temp_Queue_T;
typedef struct Temp_S
{
	unsigned short cur;
	unsigned short pre;
	unsigned short delta;
	unsigned short buff[5];
	unsigned short mean;
	unsigned char  alar_count;
}Temp_T;

extern Temp_T temp_instance;

void Temp_Init(void);
static void Temp_IO_Init(void);
/*structure config*/
static void Temp_Param_Config(Temp_T *instance);
extern void Temp_Data_Acquire(void);
static unsigned short Voltage_Get(void);
static signed short Vol_to_Temp(unsigned short voltage);

static void Temp_Data_Calc(void);
/**
  * @brief ADC Initialization Function
  * @retval None
  */
static void ADC_Init(void);

#endif
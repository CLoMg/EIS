#include "DSL_08.h"
#include "gpio.h"
#include "usart.h"
#include "stdlib.h"
#include "string.h"
#include "cmsis_os.h"

PM_IO_INFO pmIOInfo;
unsigned char PM_Buff[10] = {0,};
int PM25_Data=-1,PM10_Data=-1;
#define PM_Delta_Max 10
static uint8_t PM_CMD_Buff[9]={0xAA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xBB};
//static uint8_t PM_CMD_Buff[6]={0x11,0x00,0x00,0x00,0x00,0x00};

static uint8_t PM_Receive_Buff[32];
static uint8_t err_cnt = 0;
unsigned char  PM_buff_index = 0;

PM_Sensor_T *PM_instance;

/******MH4xxϵ�д������ӿں���******/
PM_Sensor_T *PM_Sensor_Creat(void)
{
	PM_Sensor_T *instance  = (PM_Sensor_T*)malloc(sizeof(PM_Sensor_T));
	instance->PM25 = (PM_Data_T*)malloc(sizeof(PM_Data_T));
	instance->PM10 = (PM_Data_T*)malloc(sizeof(PM_Data_T));
	
	return instance;
}
void AQI_Process(void){
	
	if(!PM_Data_Read())
//	if(!DS_Data_Read())
	{
		err_cnt++;
		if(err_cnt >10)
			PM_Sensor_Init();
	}
}

static void PM_Data_Calc(PM_Data_T *sensor_data)
{
	unsigned char i = 0;
	
	sensor_data->Buff[sensor_data->Index] = sensor_data->Cur;
	sensor_data->Delta = sensor_data->Cur - sensor_data->Pre;
	sensor_data->Pre = sensor_data->Cur;
	if(sensor_data->Delta > sensor_data->Delta_MAX)
		sensor_data->Alar_Count++;
	sensor_data->Mean=0;
  for(; i < 5 ; i++){
		sensor_data->Mean += sensor_data->Buff[i];
		sensor_data->Mean /=2;
	}
	sensor_data->Index =  (sensor_data->Index++) % 5; 
}

//void PM_Sensor_Param_Config(PM_Sensor_T *instance, UART_HandleTypeDef usart_periph,uint32_t baudrate,uint8_t channel)
//{
//	instance->Mode = PM_QA;
//	//instance->Usart_Periph = usart_periph;
//	instance->Usart_BaudRate = baudrate;
//	instance->Usart_buff_point = rx2_data_buff;
//	instance->Usart_Channel = channel;
//	instance->Usart_RX_NUM_Point = &USART2_RX_NUM;
//	
//  instance->PM25->Delta_MAX = 10;
//	instance->PM25->Alar_Count = 0;
//	instance->PM25->Index = 0;
//	instance->PM25->Cur = 0;
//	instance->PM25->Pre = 0;
//	instance->PM25->Delta = 0;
//	instance->PM25->Mean = 0;
//	memset(instance->PM10->Buff,0,5);
//	
//	instance->PM10->Delta_MAX = 10;
//	instance->PM10->Alar_Count = 0;
//	instance->PM10->Index = 0;
//	instance->PM10->Cur = 0;
//	instance->PM10->Pre = 0;
//	instance->PM10->Delta = 0;
//	instance->PM10->Mean = 0;
//	memset(instance->PM10->Buff,0,5);
//};

/* ��ʼ��PM-08 IO�� usart �˿�
Reset -- PB2    0-- reset
*/
void PM_IO_Init(PM_Sensor_T *instance)
{

	MX_USART3_UART_Init();
	HAL_UARTEx_ReceiveToIdle_IT(&huart3,pmIOInfo.buf,100);
	PM_IO_ClearReceive();
	PMSensor_GPIO_Init();
	MultiPlexer_GPIO_Init();
}
void PM_Sensor_Init(void)
{
	//PM_instance = PM_Sensor_Creat();
	//PM_Sensor_Param_Config(PM_instance,huart3,9600,1);
	
	PM_IO_Init(PM_instance);
	osDelay(100);
	PM_Mode_Change(PM_instance);
	PM_Buff[0] = 0x04;
	PM_Buff[5] = 0x05;
	err_cnt  = 0;
}

void PM_Mode_Change(PM_Sensor_T *instance)
{
	PM_Send_CMD(Stop_Continuous_Measure);
}

_Bool PM_Data_Read(void){
	uint8_t timeout =5;

	//PM_Send_CMD(PM25_PM10_Read);
	
	while(timeout--)
	{
		if(PM_IO_WaitReceive() ==1 )
		{
			if(PM_Receive_Data() == 1)
			//if((PM_Receive_Data() == 1)&&(PM_Receive_Buff[1] == PM25_PM10_Read))
			{
				PM25_Data = (uint16_t)PM_Receive_Buff[2]<<8 | PM_Receive_Buff[3];
				PM10_Data = (uint16_t)PM_Receive_Buff[4]<<8 | PM_Receive_Buff[5];
				
				memcpy(&PM_Buff[3],&PM_Receive_Buff[2],2);
				memcpy(&PM_Buff[8],&PM_Receive_Buff[4],2);
				return 1;
			}
		}
			osDelay(100);
	}
	
	PM10_Data = -1;
	PM25_Data = -1;

	return 0;
}

_Bool DS_Data_Read(void){
	uint8_t timeout =5;

	//PM_Send_CMD(PM25_PM10_Read);
	
	PM_CMD_Buff[1] = 0x02;
	PM_CMD_Buff[2] = 0x0B;
	PM_CMD_Buff[3] = 0x01;
	PM_CMD_Buff[4] = 0xE1;
	uint8_t length=5;
	PM_IO_ClearReceive();
	
	PM_IO_Send(PM_CMD_Buff,length);
	while(timeout--)
	{
		if(PM_IO_WaitReceive() ==1 )
		{
			if((DS_Receive_Data() == 1)&&(PM_Receive_Buff[0] == PM25_PM10_Read))
			{
				PM10_Data = (uint16_t)PM_Receive_Buff[2]<<8 | PM_Receive_Buff[3];
				PM25_Data = (uint16_t)PM_Receive_Buff[4]<<8 | PM_Receive_Buff[5];
				
				memcpy(&PM_Buff[1],&PM_Receive_Buff[1],4);
				memcpy(&PM_Buff[6],&PM_Receive_Buff[5],4);
				return 1;
			}
		}
			osDelay(100);
	}

	PM10_Data = -1;
	PM25_Data = -1;

	return 0;
}



void PM_Send_CMD(enum PM_CMD cmd){

	PM_CMD_Buff[1]=cmd;
	uint16_t check_sum=PM_LRC_Check(PM_CMD_Buff,6);
	PM_CMD_Buff[6]=(uint8_t)(check_sum>>8);
	PM_CMD_Buff[7]=(uint8_t)check_sum;

	uint8_t length=9;
	PM_IO_ClearReceive();
	
	PM_IO_Send(PM_CMD_Buff,length);
}

_Bool PM_Receive_Data(void){
	_Bool Error=0;
	uint8_t Read_buffer[60];

	if(pmIOInfo.dataLen>=9){
		 memcpy(Read_buffer,pmIOInfo.buf,pmIOInfo.dataLen);
			//�ʴ�ģʽ��ָ��ظ���ʽ
		 if((Read_buffer[0] == 0xAA)&&(Read_buffer[8] == 0xBB)&&(PM_LRC_Check(Read_buffer,6)==((uint16_t)(Read_buffer[6]<<8)|(Read_buffer[7])))){
				for(int i=1;i<6;++i){
						PM_Receive_Buff[i] = Read_buffer[i];	
					  Error=1;
				}
		 }
		 //�Զ��ϴ�ģʽ��ָ��ظ���ʽ
		 else if(pmIOInfo.dataLen >=32){
		 if((Read_buffer[0] == 0x42)&&(Read_buffer[1] == 0x4D)&&(PM_LRC_Check(Read_buffer,30)==((uint16_t)(Read_buffer[30]<<8)|(Read_buffer[31])))){
				for(int i=0;i<6;++i){
						PM_Receive_Buff[i] = Read_buffer[i+4];	
					  Error=1;
				}
		 }
	 }
	}
 
	
	PM_IO_ClearReceive();
	return Error;
}

_Bool DS_Receive_Data(void){
	_Bool Error=0;
	uint8_t Read_buffer[60];
	if((pmIOInfo.dataLen>=5)&&(pmIOInfo.dataLen >= pmIOInfo.buf[1]+3)){
		 memcpy(Read_buffer,pmIOInfo.buf,pmIOInfo.dataLen);
		 uint8_t data_len = Read_buffer[1];
		 if((Read_buffer[0] == 0x16)&&(DS08_CS_Check(Read_buffer,data_len+2) == Read_buffer[data_len]+2)){
				for(int i=2;i<data_len;++i){
						PM_Receive_Buff[i] = Read_buffer[i];	
					  Error=1;
				}
		 }
 
	}
	PM_IO_ClearReceive();
	return Error;
}

//==========================================================
//	�������ƣ�	NET_IO_Send
//
//	�������ܣ�	��������
//
//	��ڲ�����	str����Ҫ���͵�����
//				len�����ݳ���
//
//	���ز�����	��
//
//	˵����		�ײ�����ݷ�������
//
//==========================================================
void PM_IO_Send(unsigned char *str, unsigned short len)
{
	MultiPlexer_CH_Sel(1);
	HAL_UART_Transmit(&huart3,str,len,0xFFFF);

}

//==========================================================
//	�������ƣ�	NET_IO_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�������		0-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool PM_IO_WaitReceive(void)
{

	if(pmIOInfo.dataLen == 0) 						//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return 0;
	else
		return 1;								//���ؽ�����ɱ�־
}

//==========================================================
//	�������ƣ�	NET_IO_ClearRecive
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void PM_IO_ClearReceive(void)
{

	pmIOInfo.dataLen = 0;
	
	memset(pmIOInfo.buf, 0, sizeof(pmIOInfo.buf));
	
}

void PM_UARTEx_RxEventCallback(unsigned short Size)
{
		pmIOInfo.dataLen = Size;
	
		HAL_UARTEx_ReceiveToIdle_IT(&huart3,pmIOInfo.buf,100);

}


uint8_t DS08_CS_Check(uint8_t *data_buffer,uint8_t length){
	uint8_t  sum=0;
	
	for(int i=0;i<length;++i)
		 sum+= data_buffer[i];
	sum  = 0xFF -sum;
	return sum;
}

uint16_t PM_LRC_Check(uint8_t *data_buffer,uint8_t length){
	uint16_t  sum=0;
	if(length<=9){
	for(int i=0;i<6;++i)
		sum+= data_buffer[i];
	sum+=data_buffer[8];
	}
	else{
		for(int i=0;i<length-2;++i)
		 sum+= data_buffer[i];
	}
	
	return sum;
}
/**
  * @brief  UART error callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  HAL_StatusTypeDef Err;
	if(huart->Instance == USART3){
		__HAL_UNLOCK(&huart3);
		Err = HAL_UARTEx_ReceiveToIdle_IT(&huart3,pmIOInfo.buf,100);
	}
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_ErrorCallback could be implemented in the user file
   */
}
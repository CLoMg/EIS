
#include "gas.h"
#include "string.h"
#include "usart.h"
#include "stm32wlxx_hal.h"



unsigned char  Gas_buff_index = 0;
static Gas_T Gas_instance;
static unsigned char GasRxBuffer[MAX_GAS_BUFF_SIZE];
static unsigned short GasRXBufferSize=0;
//static Commu_States_T states;
static Gas_FSM_State_T gas_fsm_state;
Gas_FSM_Events_T gas_fsm_event;
unsigned char timeout_value = 0;
unsigned char acq_times_max =5;
void Gas_Init(void)
{
	Gas_IO_Init();
	Gas_Param_Config(&Gas_instance);
	Gas_Trans_Mode_Change(passivity_mode);
}

/*adc io channel init*/
static void Gas_IO_Init(void)
{
	
}

/*structure config*/
static void Gas_Param_Config(Gas_T *instance)
{
	instance->tran_mode = active_mode;
	instance->sensor_type = CO;
	instance->alar_count = 0;
	instance->cur = 0;
	instance->delta = 0;
	instance->pre = 0;
	instance->mean = 0;
	memset(instance->buff,0,5);
}
static void Gas_Trans_Mode_Change(Trans_Mode_T mode)
{
	char rec_buff[15]={0};
	char send_buff[9] ={0xFF,0x01,0x78,0x00,}; 
	send_buff[3] = (unsigned char)mode;
  send_buff[8] = CheckSum_Calc(send_buff,9);
	if(Gas_Cmd_Write_Read(send_buff,9,rec_buff,500))
	{
		
	}
 }

 static void Gas_Sensor_Info(void)
{
	char rec_buff[15]={0};
	char send_buff =0xD1; 
	if(Gas_Cmd_Write_Read(&send_buff,1,rec_buff,500))
	{
		if(rec_buff[8] == CheckSum_Calc(rec_buff,8))
			{
				Gas_instance.sensor_type = rec_buff[0];
				Gas_instance.range = (unsigned short)(rec_buff[1]<<8)|rec_buff[2];
				Gas_instance.unit = rec_buff[3];
				Gas_instance.sign = rec_buff[7];
			}
	}
}

extern void Gas_Data_Acquire(void)
{
	char rec_buff[15]={0};
	char send_buff[9] ={0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
	
	if(Gas_Cmd_Write_Read(send_buff,9,rec_buff,500))
	{
		if((rec_buff[0] == 0xFF) && (rec_buff[1] == 0x87) &&(rec_buff[12] == CheckSum_Calc(rec_buff,13)))
			{
				Gas_instance.cur = (uint16_t)(rec_buff[6]<<8) | rec_buff[7];
				Gas_instance.temp = (float)((int16_t)(rec_buff[8]<<8) | rec_buff[9]) / 100;
				Gas_instance.humi = (float)((uint16_t)(rec_buff[10]<<8) | rec_buff[11]) / 100;
			}
	}
}

static void Gas_Data_Calc(void)
{
	unsigned char i = 0;
	if(Gas_buff_index > 4)
		Gas_buff_index=0;
	Gas_instance.buff[Gas_buff_index++] = Gas_instance.cur;
	Gas_instance.delta = Gas_instance.cur - Gas_instance.pre;
	Gas_instance.pre = Gas_instance.cur;
	if(Gas_instance.delta > Delta_Max)
		Gas_instance.alar_count++;
	Gas_instance.mean=0;
  for(; i < 5 ; i++){
		Gas_instance.mean += Gas_instance.buff[Gas_buff_index];
		Gas_instance.mean /=2;
	}
}
/**
  * @brief  Ble CMD Write and receive response data
	* @note   
	*
	* @param  
  *                   							
  * @retval error_code
  */
_Bool Gas_Cmd_Write_Read(char *cmd_str,char cmd_len ,char *return_data,unsigned short timeout)
{
	 	Commu_States_T State = Gas_Commu_TX;
		char *ret;
		while(timeout!=0){
			switch(State)
			{
				case Gas_Commu_TX:
					Gas_Data_Send(cmd_str,cmd_len);
					State=Gas_Commu_RX;
					break;
				case Gas_Commu_RX:
					if(GasRXBufferSize>0)
					{
						memcpy(return_data,GasRxBuffer,GasRXBufferSize);
						return 1;
					}
					break;
				default:
					break;
			}
			timeout--;
		}
		return 0;
}

/**
  * @brief  Gas Data Send Function
	* @note   User need to fill this function according to their own comminication Interface
	*
	* @param  
  *                   							
  * @retval error_code
  */
void Gas_Data_Send(char *data_send,unsigned short data_len)
{
	HAL_UART_Transmit_IT(&huart2,(unsigned char *)data_send,data_len);
}

//数据校验值计算
uint8_t  CheckSum_Calc(char *packet,uint8_t length){
	
	uint8_t i;
	uint32_t checksum=0;
	for(i=0;i<length;++i){
		checksum +=packet[i];
	}
	checksum &=0xff;
	return checksum;
}

void Gas_FSM(void)
{
	switch(gas_fsm_state){
		case defalut:
			if(gas_fsm_event == init)
			{	
				Gas_Init();
				gas_fsm_state = idle;
			}
			break;
		case idle:
			if(gas_fsm_event == acquire)
			{
				char send_buff[9] ={0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
				Gas_Data_Send(send_buff,9);
				gas_fsm_state = data_acquire;
			}
			else if(gas_fsm_event == query_info)
			{
				char send_buff =0xD1;
				Gas_Data_Send(&send_buff,1);
				gas_fsm_state = info_query;
			}
			else;
			break;
		case info_query:
		{
			timeout_value++;
			if(GasRXBufferSize>0)
			{
				if(GasRxBuffer[8] == CheckSum_Calc((char *)GasRxBuffer,8))
				{
					Gas_instance.sensor_type = GasRxBuffer[0];
					Gas_instance.range = (unsigned short)(GasRxBuffer[1]<<8)|GasRxBuffer[2];
					Gas_instance.unit = GasRxBuffer[3];
					Gas_instance.sign = GasRxBuffer[7];
					memset(GasRxBuffer,0,GasRXBufferSize);
					GasRXBufferSize = 0;
					timeout_value= 0;
					gas_fsm_state = idle;
				}
				else
				{
					memset(GasRxBuffer,0,GasRXBufferSize);
					GasRXBufferSize = 0;
				}
			}
			else if(timeout_value %5 == 0){
				char send_buff =0xD1;
				Gas_Data_Send(&send_buff,1);
			}
			else if(timeout_value > acq_times_max)
			{
				timeout_value =0;
				gas_fsm_state = idle;
			}
			else;
		}
			break;
		case data_acquire:
		{
			timeout_value++;
			if(GasRXBufferSize>0)
			{
				if((GasRxBuffer[0] == 0xFF) && (GasRxBuffer[1] == 0x87) &&(GasRxBuffer[12] == CheckSum_Calc((char *)GasRxBuffer,13)))
				{
					Gas_instance.cur = (uint16_t)(GasRxBuffer[6]<<8) | GasRxBuffer[7];
					Gas_instance.temp = (float)((int16_t)(GasRxBuffer[8]<<8) | GasRxBuffer[9]) / 100;
					Gas_instance.humi = (float)((uint16_t)(GasRxBuffer[10]<<8) | GasRxBuffer[11]) / 100;
					memset(GasRxBuffer,0,GasRXBufferSize);
					GasRXBufferSize = 0;
					timeout_value= 0;
					gas_fsm_state = idle;
				}
				else
				{
					memset(GasRxBuffer,0,GasRXBufferSize);
					GasRXBufferSize = 0;
				}
			}
			else if(timeout_value %5 == 0){
				char send_buff[9] ={0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
				Gas_Data_Send(send_buff,9);
			}
			else if(timeout_value > acq_times_max)
			{
				timeout_value =0;
				gas_fsm_state = idle;
			}
			else;
		}
			break;
		default:
			break;
	}

}
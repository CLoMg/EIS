
#include "ec_tb600.h"
#include "string.h"
#include "uart.h"




unsigned char  Gas_buff_index = 0;
//static EC_Sense_T Gas_instance;
static unsigned char GasRxBuffer[MAX_GAS_BUFF_SIZE];
static unsigned short GasRXBufferSize=0;
EC_Sense_T instance_co;
//static Commu_States_T states;
static Gas_FSM_State_T gas_fsm_state;
Gas_FSM_Events_T gas_fsm_event;
unsigned char timeout_value = 0;
unsigned char acq_times_max =5;
void Gas_Init(EC_Sense_T *instance)
{
	Gas_IO_Init();
	Gas_Param_Config(instance,USART0);
	Gas_Trans_Mode_Change(instance,passivity_mode);
}

/*adc io channel init*/
static void Gas_IO_Init(void)
{
	
}

/*structure config*/
static void Gas_Param_Config(EC_Sense_T *instance,uint32_t usart_periph)
{
	instance->tran_mode = active_mode;
	instance->sensor_type = CO;
	instance->alar_count = 0;
	instance->cur = 0;
	instance->delta = 0;
	instance->pre = 0;
	instance->mean = 0;
	instance->usart_periph = usart_periph;
	memset(instance->buff,0,5);
}
static void Gas_Trans_Mode_Change(EC_Sense_T *instance,Trans_Mode_T mode)
{
	char rec_buff[15]={0};
	char send_buff[9] ={0xFF,0x01,0x78,0x00,}; 
	send_buff[3] = (unsigned char)mode;
  send_buff[8] = CheckSum_Calc(send_buff,9);
	if(Gas_Cmd_Write_Read(instance->usart_periph,send_buff,9,rec_buff,500))
	{
		
	}
 }

 static void Gas_Sensor_Info(EC_Sense_T *instance)
{
	char rec_buff[15]={0};
	char send_buff =0xD1; 
	if(Gas_Cmd_Write_Read(instance->usart_periph,&send_buff,1,rec_buff,500))
	{
		if(rec_buff[8] == CheckSum_Calc(rec_buff,8))
			{
				instance->sensor_type = rec_buff[0];
				instance->range = (unsigned short)(rec_buff[1]<<8)|rec_buff[2];
				instance->unit = rec_buff[3];
				instance->sign = rec_buff[7];
			}
	}
}

extern void Gas_Data_Acquire(EC_Sense_T *instance)
{
	char rec_buff[15]={0};
	char send_buff[9] ={0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
	
	if(Gas_Cmd_Write_Read(instance->usart_periph,send_buff,9,rec_buff,500))
	{
		if((rec_buff[0] == 0xFF) && (rec_buff[1] == 0x87) &&(rec_buff[12] == CheckSum_Calc(rec_buff,13)))
			{
				instance->cur = (uint16_t)(rec_buff[6]<<8) | rec_buff[7];
				instance->temp = (float)((int16_t)(rec_buff[8]<<8) | rec_buff[9]) / 100;
				instance->humi = (float)((uint16_t)(rec_buff[10]<<8) | rec_buff[11]) / 100;
			}
	}
}

static void Gas_Data_Calc(EC_Sense_T *instance)
{
	unsigned char i = 0;
	if(Gas_buff_index > 4)
		Gas_buff_index=0;
	instance->buff[Gas_buff_index++] = instance->cur;
	instance->delta = instance->cur - instance->pre;
	instance->pre = instance->cur;
	if(instance->delta > Delta_Max)
		instance->alar_count++;
	instance->mean=0;
  for(; i < 5 ; i++){
		instance->mean += instance->buff[Gas_buff_index];
		instance->mean /=2;
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
_Bool Gas_Cmd_Write_Read(uint32_t usart_periph,char *cmd_str,char cmd_len ,char *return_data,unsigned short timeout)
{
	 	Commu_States_T State = Gas_Commu_TX;
		char *ret;
		while(timeout!=0){
			switch(State)
			{
				case Gas_Commu_TX:
					Gas_Data_Send(usart_periph,cmd_str,cmd_len);
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
void Gas_Data_Send(uint32_t usart_periph,char *data_send,unsigned short data_len)
{
	
	USART_Data_Send(usart_periph,0,(uint8_t *)data_send,data_len);
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

//void Gas_FSM(void)
//{
//	switch(gas_fsm_state){
//		case defalut:
//			if(gas_fsm_event == init)
//			{	
//				Gas_Init();
//				gas_fsm_state = idle;
//			}
//			break;
//		case idle:
//			if(gas_fsm_event == acquire)
//			{
//				Gas_Data_Acquire(&Gas_instance);
//				gas_fsm_state = data_acquire;
//			}
//			else if(gas_fsm_event == query_info)
//			{

//			}
//			else;
//			break;
//		case info_query:
//		{
//			
//		}
//			break;
//		case data_acquire:
//		{
//			
//		}
//			break;
//		default:
//			break;
//	}

//}
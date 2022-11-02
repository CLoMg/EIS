#include "pms7003.h"
#include "usart.h"
#include "string.h"
#include "delay.h"
#include "LORA.h"


static uint8_t PMS_CMD_Buff[7]={0x42,0x4d};
static uint8_t PMS_Receive_Buff[32];


/**
  * @brief  PMS7003颗粒物传感器初始化：
	*					1、GPIO初始化
	*					2、串口初始化
	*					3、上电后，将传感器由主动上传数据改为被动读取模式
	*					4、读一次传感器，LORA发送初始化成功与否
  * @param  None.
  * @retval None.
  */
void  PMS_init(void){
	uint8_t dummy_buff[4];
	uint8_t i=0;
	char init_info[]="UART6:PMS7003\r\n";
	PMS_GPIO_Config();
	uart_init(USART6,9600);
	PMS_State_Change(passive);
	delay_ms(10);
	
	/*将传感器工作状态由主动上传数据切换为被动读取数据*/
	PMS_State_Change(passive);
	if(PMS_Data_Read(dummy_buff)==1){
			while(init_info[i++]!='\0'){
			USART_SendData(USART3,init_info[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
		}
		LORA_send_data(init_info,sizeof(init_info));
	}	

}


/**
  * @brief  初始化PSM_7003 IO口：
	* 				Reset -- PC2    0-- reset
	*					Set   -- PC1  : 1 -- Nomal  0--Sleep
  * @param  None.
  * @retval None.
  */
void PMS_GPIO_Config(void){

	if((device_type_num==0)||(device_type_num==1)){
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC总线时钟

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2; //GPIOC1与GPIOC2
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽模式
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
		
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		GPIO_SetBits(GPIOC,GPIO_Pin_1|GPIO_Pin_2);
	}

}

/**
  * @brief  PMS_7003 数据输出状态切换..
  * @param  newstate: passive=0x00,acitive=0x01
  * @retval None.
  */
void PMS_State_Change(enum update_state newstate){
	Error_Code_t err = 0;
	PMS_Send_CMD(State_Chanage,newstate);
	delay_ms(100);
	err = PMS_Receive_Data();
	

}

/**
  * @brief  PMS_7003待机状态状态切换..
  * @param  newstate: standby=0x00,normal=0x01
  * @retval None.
  */
void PMS_standby_Change(enum standby_mode newmode){
	
	PMS_Send_CMD(State_Chanage,newmode);
	delay_ms(100);
  PMS_Receive_Data();
}

/**
  * @brief  PMS_7003主动读取数据.
  * @param  *data: 数据读取后存储地址指针，uint8_t类型
	* @retval Error_Code: 1-读取成功 0-读取失败
  */
Error_Code_t PMS_Data_Read(uint8_t *data){
	Error_Code_t Error=0;
	uint8_t count=50;
	PMS_Send_CMD(Data_Read,0);
	do{
		
		delay_ms(1);
	}
	while((!(Error=PMS_Receive_Data()))&&((count--)>0));
	 USART_RX_CNT[5]=0;
	if(Error){
	data[0]=PMS_Receive_Buff[6];
	data[1]=PMS_Receive_Buff[7];
	data[2]=PMS_Receive_Buff[8];
	data[3]=PMS_Receive_Buff[9];
	}
	else 
	{
		data[0]=0xFF;
		data[1]=0xFF;
		data[2]=0xFF;
		data[3]=0xFF;
	}return Error;
}

/**
	* @brief  向PMS7003发送指令..
  * @param  cmd: State_Chanage(数据上传状态切换)=0xE1,Data_Read(数据读取)=0xE2,Standby_Ctl(待机状态控制)=0xE4
	*	@param  state_bytes:状态控制符，搭配指令使用
  * @retval None.
  */
void PMS_Send_CMD(enum PSM_CMD cmd,uint8_t state_bytes){
	PMS_CMD_Buff[2]=cmd;
	PMS_CMD_Buff[4]=state_bytes;
	uint16_t check_sum=PMS_LRC_Check(PMS_CMD_Buff,5);
	PMS_CMD_Buff[5]=(uint8_t)(check_sum>>8);
	PMS_CMD_Buff[6]=(uint8_t)check_sum;

	uint8_t length=7;
	memset(USART_RX_BUF[5],0,USART_REC_LEN);
	while(length--){
		USART_SendData(USART6,PMS_CMD_Buff[6-length]);
		while(USART_GetFlagStatus(USART6,USART_FLAG_TC)!=SET);
	}
}

/**
	* @brief  接收PMS7003传感器数据..
  * @param  None
	* @retval Error_Code:1- 接收到正确数据，0-未接收到正确数据.
  */
Error_Code_t PMS_Receive_Data(){
	Error_Code_t Error=0;
	uint8_t Read_buffer[60];
	if(USART_RX_CNT[5]>=7){
		 uint8_t length=USART_RX_CNT[5];
		 memcpy(Read_buffer,&USART_RX_BUF[5],USART_RX_CNT[5]);
		 if((Read_buffer[0]==0x42)&&(Read_buffer[1]==0x4D)&&(PMS_LRC_Check(Read_buffer,length-2)==((uint16_t)(Read_buffer[length-2]<<8)|(Read_buffer[length-1])))){
				for(int i=2;i<length;++i){
						PMS_Receive_Buff[i] = Read_buffer[i];	
					  Error=1;
				}
		 }
 
	}
	
	return Error;
}

/**
	* @brief  传感器数据LRC校验..
	* @param  *data_buff:待校验数据地址起始指针 uint8_t类型
	* @param  length:  待校验数据长度
	* @retval 计算所得校验码，uint16_t类型.
  */
uint16_t PMS_LRC_Check(uint8_t *data_buffer,uint8_t length){
	uint16_t  sum=0;
	for(int i=0;i<length;++i)
		sum+= data_buffer[i];
	return sum;
}


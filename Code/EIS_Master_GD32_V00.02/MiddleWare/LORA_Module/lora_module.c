#include "msg_handler.h"
#include "lora_module.h"
#include "uart.h"
#include "string.h"
#include "led.h"
#include "stdlib.h"
#include "timer.h"
#include "gd32f10x_eval.h"
#include "stdio.h"
#include "rs485.h"
#include "string.h"
#include "freertos.h"
#include "task.h"
#include "queue.h"
#include "fsm.h"

//#include "stmflash.h"
#define MSG_MIN_LEN 21
#define UNIQUE_ID_ADDR 0x1FFFF7E8

#define HEAD 0x4A54
#define ENCRY_MODE 0x00

#define LORA_USART 0                                                                                                                                                                                                                                         
#if LORA_USART
	#define LORA_RX_BUFF  rx1_data_buff
#else
	#define LORA_RX_BUFF  rx0_data_buff	
#endif
uint8_t unique_id[8];
uint8_t LORA_Spd_Default = 10;//设定lora模块默认通信速率等级为10 -- 21875bps
uint8_t LORA_Ch_Default = 72; //设定lora模块默认信道为72 （389+72MHz）

External_MSG_T *Ext_Msg_Struct = NULL;
Lora_TxMSG_T *Lora_TxMSG_Head = NULL;
Lora_Para_Record_T Lora_Param_Struct;
FSM_T *LORA_Config_FSM = NULL;

typedef struct AT_CMD_MSG{
	unsigned char cmd_len;
	char cmd_string[20];
	unsigned char reply_len;
	char reply_string[10];
}AT_CMD_MSG_T;

static uint8_t cmd_index = 0 ,retry_times = 0 ;
AT_CMD_MSG_T LORA_Config_CMD[]=
{
	{3,"+++",1,'a'},
	{1,"a",2,"OK"},
	{sizeof("AT+E=OFF\r\n")-1,"AT+E=OFF\r\n",2,"OK"},
	{sizeof("AT+WMODE=TRANS\r\n")-1,"AT+WMODE=TRANS\r\n",2,"OK"},
	{sizeof("AT+SPD=10\r\n")-1,"AT+SPD=10\r\n",2,"OK"},
	{sizeof("AT+CH=72\r\n")-1,"AT+CH=72\r\n",2,"OK"},
	{sizeof("AT+ADDR=65535\r\n")-1,"AT+ADDR=65535\r\n",2,"OK"},	
	{sizeof("AT+Z\r\n")-1,"AT+Z\r\n",2,"OK"},
};

typedef struct AT_CMD_List{
	unsigned char num_of_element;
	unsigned char index_of_element[10];
}AT_CMD_List_T;

AT_CMD_List_T AT_CMD_LIST;


enum lora_states
{
	UnInit = 0x00, TSP, AT_Idle, Wait_Reply, Restart, 
};

typedef enum  
{
	Nothing=0x00, AT_Enter_Event, Reply_Match_Event, Timeout_Event,Retryout_Event,
	Config_Finish_Event,
}Events_T;

LORA_MODE_T lora_mode;
static Events_T lora_cfg_event;
static StateTransform_T trans_table[] = 
{
	{UnInit,AT_Enter_Event,AT_Idle,CMDList_Refresh_Func},
	{TSP,AT_Enter_Event,AT_Idle,CMDList_Refresh_Func},
	{AT_Idle,NULL,Wait_Reply,CMD_Transimit_Func},
	{AT_Idle,Reply_Match_Event,Wait_Reply,CMD_Transimit_Func},
	{Wait_Reply,Reply_Match_Event,AT_Idle,CmdIdex_Inc_Func},
	{Wait_Reply,Timeout_Event,AT_Idle,RetryTimes_Inc_Func},
	{Wait_Reply,Retryout_Event,Restart,RetryTime_Clr_Func},
	{Wait_Reply,Config_Finish_Event,TSP,Mode_Change_Func},
};


void CMDList_Refresh_Func(){
	switch(lora_mode){
		case UnInit_Mode:{
			AT_CMD_LIST.num_of_element = 8;
			unsigned char i = 0;
			for(i=0;i<AT_CMD_LIST.num_of_element;++i)
				AT_CMD_LIST.index_of_element[i] = i;
			lora_mode = AT_Mode;
		}
			break;
		case Tsp_Mode:{
			AT_CMD_LIST.num_of_element = 5;
			unsigned char index_array[5] = {0,1,4,5,7};
			memcpy(AT_CMD_LIST.index_of_element,index_array,5);
			lora_mode = AT_Mode;
		}
			break;
		
		default:
			break;
	}
	cmd_index = 0;
	lora_cfg_event = Nothing;
}

void CMD_Transimit_Func(void){
	LORA_send_data(LORA_Config_CMD[cmd_index].cmd_string,LORA_Config_CMD[cmd_index].cmd_len);
}
void CmdIdex_Inc_Func(void){
	cmd_index++;
}
void RetryTimes_Inc_Func(void){
	retry_times++;
	lora_cfg_event = Nothing;
}
void RetryTime_Clr_Func(void){
	retry_times = 0;
}
void Mode_Change_Func(void){
	lora_mode = Tsp_Mode;
}

void LORA_Config_FSM_Init(void)
{
	LORA_Config_FSM = (FSM_T*)malloc(sizeof(FSM_T));
	
	FSM_Regist(LORA_Config_FSM,trans_table);
	FSM_StateTransfer(LORA_Config_FSM,UnInit);
	LORA_Config_FSM->transform_num = 8;
	lora_cfg_event = AT_Enter_Event;
}


void Device_ID_Get(void){
	uint32_t unique_id_32[3];
	uint8_t i = 0;
	/*读取芯片唯一ID(96位)，存储于3个32位数组中
	unique_id_32[0] : Device_ID[31:0]
	unique_id_32[1] : Device_ID[63:32]
	unique_id_32[2] : Device_ID[95:64]
	*/
	for(i=0;i<3;++i)
		unique_id_32[i] = *(__IO uint32_t *)(UNIQUE_ID_ADDR+i*4);
	/*将芯片唯一ID低84位存储于8个8位数组中
	unique_id_8[0] : Device_ID[63:56]
	...
	unique_id_8[7] : Device_ID[7:0]
	*/
	for(i=0;i<8;++i)
	{
		unique_id[i]  = i < 4? (uint8_t)(unique_id_32[1]>>((3-i)*8)) : (uint8_t)(unique_id_32[0]>>((7-i)*8));
	}
}
/*初始化外部消息通信结构体，包括帧头（head），加密方式（encry_mode）,项目编号（item_num）*/
External_MSG_T *Ext_MSG_Init(uint16_t head, uint8_t encry_mode, uint8_t item_num)
{
	uint32_t unique_id_32[3];
	uint8_t unique_id_8[8];
	uint8_t i = 0;
	/*读取芯片唯一ID(96位)，存储于3个32位数组中
	unique_id_32[0] : Device_ID[31:0]
	unique_id_32[1] : Device_ID[63:32]
	unique_id_32[2] : Device_ID[95:64]
	*/
	for(i=0;i<3;++i)
		unique_id_32[i] = *(__IO uint32_t *)(UNIQUE_ID_ADDR+i*4);
	/*将芯片唯一ID低84位存储于8个8位数组中
	unique_id_8[0] : Device_ID[63:56]
	...
	unique_id_8[7] : Device_ID[7:0]
	*/
	for(i=0;i<8;++i)
	{
		unique_id_8[i]  = i < 4? (uint8_t)(unique_id_32[1]>>((3-i)*8)) : (uint8_t)(unique_id_32[0]>>((7-i)*8));
	}
	
	External_MSG_T *ext_msg = (External_MSG_T *)malloc(sizeof(External_MSG_T));
	ext_msg->Head[0]  = (uint8_t)(head >>8); 
	ext_msg->Head[1]  = (uint8_t)head;
	memcpy(ext_msg->Device_Id,unique_id_8,8);
	ext_msg->Encry_Mode = encry_mode;
	ext_msg->Func_Code[0] = item_num;
	
	return ext_msg;
}

/*初始化外部消息通信结构体，包括帧头（head），加密方式（encry_mode）,项目编号（item_num）*/
Lora_TxMSG_T *Lora_TxMSG_Produce(uint16_t sn,uint8_t data_dir,uint16_t func_code,uint8_t *data ,uint16_t len)
{
	uint16_t check_sum;

	uint8_t *Data = (uint8_t *)malloc(sizeof(uint8_t)*len+21);  
	Lora_TxMSG_T *Node=(Lora_TxMSG_T *)malloc(sizeof(Lora_TxMSG_T));
	
	Data[0]  = (uint8_t)(HEAD >>8); 
	Data[1]  = (uint8_t)HEAD;
	memcpy(&Data[2],unique_id,8);
	Data[10] = (uint8_t)(sn>>8);
	Data[11] = (uint8_t)sn;
	Data[12] = ENCRY_MODE;
	Data[13] = ITEM_NUM;
	Data[14] = data_dir;
	Data[15] = (uint8_t)(func_code>>8);
	Data[16] = (uint8_t)func_code;
	Data[17] = (uint8_t)(len>>8);
	Data[18] = (uint8_t)len;
	memcpy(&Data[19],data,len);
	
	check_sum = CRC_Calc(Data,len+19);
	
	Data[19+len] = (uint8_t)(check_sum>>8);
	Data[20+len] = (uint8_t)check_sum;
	
	Node->len = len+21;
	Node->data = Data;
	Node->next = NULL;
	return Node;
}

void LORA_Setup_Parameter_Access(void){
	uint32_t LORA_Parameter_IN_Flash;
	LORA_Parameter_IN_Flash = *(__IO uint32_t *)(LORA_PARA_ADDR);
	
  if(LORA_Parameter_IN_Flash!=0xffffffff){
		LORA_Spd_Default=(uint8_t)(LORA_Parameter_IN_Flash>>24);
		LORA_Ch_Default = (uint8_t)(LORA_Parameter_IN_Flash>>16);
	}
	Lora_Param_Struct.src_para.spd = LORA_Spd_Default;
	Lora_Param_Struct.src_para.ch = LORA_Ch_Default;
}

void LORA_Init(void){
	#if LORA_USART
	  usart_init(USART1,115200);
	#else
		usart_init(USART0,115200);
	#endif

		
	  delay_ms(500);
	  LORA_Module_GPIO_Init();	
	  delay_ms(1000);
	  LORA_Hardware_Reset();
	  delay_ms(1000);
	  LORA_Setup_Parameter_Access();
	
	  //LORA_Param_Set();/*2022.07.27注释，使用LORA_Config_FSM_Init()代替*/
	
		LORA_Config_FSM_Init();
		Device_ID_Get();
		
		Ext_Msg_Struct = Ext_MSG_Init(HEAD, ENCRY_MODE, ITEM_NUM);
	
		Internal_MSG_T *p = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T));
	  p->next = NULL;
		InMsg_Lora_Head = p;
	
		Lora_TxMSG_T *q = (Lora_TxMSG_T *)malloc(sizeof(Lora_TxMSG_T));
		q->next = NULL;
		Lora_TxMSG_Head = q;
		
		Usart_Msg_Link_T *r = (Usart_Msg_Link_T *)malloc(sizeof(Usart_Msg_Link_T));
		r->Next = NULL;
		LORA_Rec_Msg_Head = r;
	
}

void LORA_Hardware_Reset(void){

		LORA_Module_Reset_Control(0);
		delay_ms(10);
		LORA_Module_Reset_Control(1);
}

_Bool LORA_Param_Set(void){
	_Bool Error=1;
	uint16_t timeout=10;
	uint8_t point=0;
	char command_string[15],len;
	//进入AT指令模式
	do{
		point=0;
		memset(LORA_RX_BUFF,0,rx_buff_size);	
		LORA_send_data("+++",sizeof("+++")-1);
		delay_ms(200);
		timeout--;
		while((LORA_RX_BUFF[point++]==0x00)&&(point<=60));
	}
	while((strchr((const char*) &(LORA_RX_BUFF[point-1]),'a')==NULL)&&(timeout>0));
	if(timeout==0) Error=0;
	
	timeout=5;
	do{
		point=0;
		memset(LORA_RX_BUFF,0,rx_buff_size);	
		LORA_send_data("a",sizeof("a")-1);
		delay_ms(200);
		timeout--;
		while((LORA_RX_BUFF[point++]==0x00)&&(point<=60));
	}
	while((strstr((const char*) &(LORA_RX_BUFF[point-1]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) Error=0;
	
	//关闭指令回显
	timeout=10;
	do{
		memset(LORA_RX_BUFF,0,rx_buff_size);	

		LORA_send_data("AT+E=OFF\r\n",sizeof("AT+E=OFF\r\n")-1);
		delay_ms(500);
		timeout--;
	}
 	while((strstr((const char*) &(LORA_RX_BUFF[0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) Error=0;
	
	//设置工作模式 ：透传模式 AT+WMODE=TRANS 定点模式AT+WMODE=FP
	timeout=10;
	do{
		memset(LORA_RX_BUFF,0,rx_buff_size);	

		LORA_send_data("AT+WMODE=TRANS\r\n",sizeof("AT+WMODE=TRANS\r\n")-1);
		delay_ms(500);
		timeout--;
	}
 	while((strstr((const char*) &(LORA_RX_BUFF[0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) Error=0;
	
	/******************************************************
	设置LORA控制速率等级 class：1~10（default）  AT+SPD=10
	速率对应关系（速率为理论峰值，实际速度要较小一些）
	1：268bps
	2：488bps
	3：537bps
	4：878bps
	5：977bps
	6：1758bps
	7：3125ps
	8：62500bps
	9：10937bps
	10：21875bps
	*********************************************************/
	timeout=10;
	do{
		len = sprintf(command_string,"AT+SPD=%d\r\n",LORA_Spd_Default);
		
		memset(LORA_RX_BUFF,0,rx_buff_size);	

		LORA_send_data(command_string,len);
		delay_ms(500);
		timeout--;
	}
 	while((strstr((const char*) &(LORA_RX_BUFF[0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) Error=0;

	/**********************************************
	设置信道 AT+CH=<ch><CR><LF>
	ch: L101-L:0~127(default:72,对应470MHZ)  (398+ch)MHZ
	    L101-H:0~127(default:65,对应868MHZ)  (803+ch)MHZ
	本次项目所用为L101-H系列模块
	***********************************************/
	timeout=10;
	do{

   	len = sprintf(command_string,"AT+CH=%d\r\n",LORA_Ch_Default);
		
		memset(LORA_RX_BUFF,'\0',rx_buff_size);	
		LORA_send_data(command_string,len);
		delay_ms(500);
		timeout--;
	}
	while((strstr((const char*) &(LORA_RX_BUFF[0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) Error=0;
	
		/**********************************************
	设置目标地址AT+ADDR=<addr><CR><LF>
	addr：0（default）~65535 65535为广播地址，同信道同速率的模块都能接收
	***********************************************/
	timeout=10;
	do{
		memset(LORA_RX_BUFF,'\0',rx_buff_size);	
		LORA_send_data("AT+ADDR=65535\r\n",sizeof("AT+ADDR=65535\r\n")-1);
		delay_ms(500);
		timeout--;
	}
	while((strstr((const char*) &(LORA_RX_BUFF[0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) Error=0;

		/**********************************************
	重启模块
	***********************************************/
	timeout=10;
	do{
		memset(LORA_RX_BUFF,'\0',rx_buff_size);	
		LORA_send_data("AT+Z\r\n",sizeof("AT+Z\r\n")-1);
		delay_ms(1000);
		timeout--;
	}
	while((strstr((const char*) &(LORA_RX_BUFF[0]),"Start")==NULL)&&(timeout>0));
	if(timeout==0)  Error=0;
	
	return Error;
}

//重启模块
_Bool LoRA_Restart_Mode(void){

	_Bool Error=1;
	uint8_t timeout=10;
	timeout=10;
	do{
		memset(LORA_RX_BUFF,'\0',rx_buff_size);	
		LORA_send_data("AT+Z\r\n",sizeof("AT+Z\r\n")-1);
		vTaskDelay(200 / portTICK_RATE_MS);
		timeout--;
		printf("%s\r\n",LORA_RX_BUFF);
	}
	while((strstr((const char*) &(LORA_RX_BUFF[0]),"Start")==NULL)&&(timeout>0));
	if(timeout==0)  Error=0;
	return Error;
}

//进入AT指令模式 返回值0-失败 1-成功
_Bool LoRA_Enter_Command_Mode(void){

	_Bool Error=1;
	uint8_t timeout=10;
	uint8_t point=0;
	do{
		point=0;
		memset(LORA_RX_BUFF,0,rx_buff_size);	
		LORA_send_data("+++",sizeof("+++")-1);
		vTaskDelay(200 / portTICK_RATE_MS);
		timeout--;
		while((LORA_RX_BUFF[point++]==0x00)&&(point<=60));
	}
	while((strchr((const char*) &(LORA_RX_BUFF[point-1]),'a')==NULL)&&(timeout>0));
	if(timeout==0) {
		Error=0;
		return Error;
	}
	
	timeout=10;
	do{
		point=0;

		memset(LORA_RX_BUFF,0,rx_buff_size);	
		LORA_send_data("a",sizeof("a")-1);
		vTaskDelay(200 / portTICK_RATE_MS);
		timeout--;
		while((LORA_RX_BUFF[point++]==0x00)&&(point<=60));
	}
	while((strstr((const char*) &(LORA_RX_BUFF[point-1]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) Error=0;
  return Error;
}

//设置LoRA模块速率等级 new_speed: 1~10  返回值 0：失败 1：成功
_Bool LoRA_Speed_Grade_Set(uint8_t new_speed,uint8_t mode_change,uint8_t restart){
	
	uint8_t ori_speed,timeout,speed_in_flash_u8;
	uint32_t speed_in_flash_u32;
	char command[11];
	if(mode_change){
		//模块从透传模式返回指令模式，失败则返回0
		timeout=5;
		while(!LoRA_Enter_Command_Mode()&&(timeout--))
		if(timeout==0)
			return 0;
		delay_ms(50);
	}
//	STMFLASH_Read(Group_LORA_Speed_Grade,&speed_in_flash_u32,1);
	speed_in_flash_u8=(uint8_t)speed_in_flash_u32;
	if(speed_in_flash_u8!=new_speed)
//		STMFLASH_Write(Group_LORA_Speed_Grade,(uint32_t *)&new_speed,1);
	//查询模块当前速率等级，如果与将要设置等级一致，则无需修改，直接返回 1
  ori_speed=LoRA_Query_Speed_Grade();
  if(ori_speed==new_speed)
		return 1;
	
	//模块设置为指定信道，失败则返回0
	delay_ms(50);
	timeout=10;
	sprintf(command,"AT+SPD=%d\r\n",new_speed);
	do{

			memset(LORA_RX_BUFF,0,rx_buff_size);	

			LORA_send_data(command,sizeof(command)-1);
			delay_ms(500);
			timeout--;
	}
	while((strstr((const char*) &(LORA_RX_BUFF[0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) 
		return 0;
	delay_ms(50);
	//重启模块
	if(restart)
		LoRA_Restart_Mode();
	return 1;
		
}

//查询模块速率等级 返回值-1：查询失败，1~10为速率等级
int8_t LoRA_Query_Speed_Grade(void){

	int8_t ori_speed=-1;
	uint8_t timeout=5;
	char *point=NULL;
	do{
		
		memset(LORA_RX_BUFF,'\0',rx_buff_size);	
		LORA_send_data("AT+SPD?\r\n",sizeof("AT+SPD?\r\n")-1);
		timeout--;
		vTaskDelay(200 / portTICK_RATE_MS);
		point=strstr((const char*) &(LORA_RX_BUFF[0]),"+SPD:");
	}
	while((point==NULL)&&(timeout>0));
	if(point!=NULL)
		ori_speed=atoi(point+5);
  return ori_speed;
}

//设置LoRA模块信道new_ch:1~127
_Bool LoRA_Channel_Set(uint8_t new_ch,uint8_t mode_change,uint8_t restart){
	
	uint8_t ori_channel,timeout,channel_in_flash_u8;
	uint32_t channel_in_flash_u32;
	char command[11];
	
	if(mode_change){	//模块从透传模式返回指令模式，失败则返回0
		timeout=5;
		while(!LoRA_Enter_Command_Mode()&&(timeout--))
		if(timeout==0)
			return 0;
		delay_ms(50);
	}
	
//	STMFLASH_Read(Group_LORA_Channel,&channel_in_flash_u32,1);
	channel_in_flash_u8=(uint8_t)channel_in_flash_u32;
	if(channel_in_flash_u8!=new_ch)
//	STMFLASH_Write(Group_LORA_Channel,(uint32_t *)&new_ch,1);
	
	//查询模块当前信道，如果与将要设置的信道一致，则不设置。直接返回1
  ori_channel=LoRA_Query_Channel();
  if(ori_channel==new_ch)
		return 1;

	//修改模块信道为预设信道，失败则返回0
	timeout=10;
	delay_ms(50);
	sprintf(command,"AT+CH=%d\r\n",new_ch);
	do{

			memset(LORA_RX_BUFF,0,rx_buff_size);	

			LORA_send_data(command,sizeof(command)-1);
			delay_ms(500);
			timeout--;
	}
	while((strstr((const char*) &(LORA_RX_BUFF[0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) 
		return 0;
	
	delay_ms(50);
	//重启模块
	if(restart)
		LoRA_Restart_Mode();
	
	return 1;
		
}

//查询模块信道 返回值-1：查询失败，0~127为当前信道
int8_t LoRA_Query_Channel(void){

	int8_t ori_channel=-1;
	uint8_t timeout=5;
	char *point=NULL;
	do{
		
		memset(LORA_RX_BUFF,'\0',rx_buff_size);	
		LORA_send_data("AT+CH?\r\n",sizeof("AT+CH?\r\n")-1);
		timeout--;
		vTaskDelay(200 / portTICK_RATE_MS);
		point=strstr((const char*) &(LORA_RX_BUFF[0]),"+CH:");
	}
	while((point==NULL)&&(timeout>0));
	if(point!=NULL)
		ori_channel=atoi(point+4);
  return ori_channel;
}

//设置LoRA模块速率等级和信道 new_speed(速率等级): 1~10 new_channel(信道)：0~127
_Bool LoRA_SPD_CH_Set(uint8_t new_speed,uint8_t new_channel){
	
	_Bool error=1;
	
	//进入指令模式
	LoRA_Enter_Command_Mode();
	
  // 设置新的速率等级	
	error&=LoRA_Speed_Grade_Set(new_speed,0,0);

	// 设置新信道
	error&=LoRA_Channel_Set(new_channel,0,0);
  
	//重启模块
	error&=LoRA_Restart_Mode();
	
	 
	return error;

}

//查询LoRA模块速率等级和信道 new_speed(速率等级): 1~10 new_channel(信道)：0~127
_Bool LoRA_SPD_CH_Query(int8_t *ch_reg,int8_t *spd_reg){
	
	_Bool error=1;
	//进入指令模式
	LoRA_Enter_Command_Mode();
	
  // 查询速率等级	
	*spd_reg = LoRA_Query_Speed_Grade();
	// 查询信道
	*ch_reg = LoRA_Query_Channel();
  
	//重启模块
	error&=LoRA_Restart_Mode();
	 
	return error;

}

//MCU发送数据给LoRA模块
void LORA_send_data(char *sendbuff,uint16_t length){
	char *data_to_send=NULL;
	data_to_send=sendbuff;
	#if LORA_USART
		USART_Data_DMA_Send(USART1,(uint8_t *)data_to_send,length);
	#else
		USART_Data_DMA_Send(USART0,(uint8_t *)data_to_send,length);
		//USART_Data_Send(USART0,0,(uint8_t *)data_to_send,length);
	#endif
}



uint16_t CRC_Calc(uint8_t *data_buff,uint8_t len){
	
	uint16_t sum=0;
	for(uint8_t count=0;count<len;++count)
		sum+=data_buff[count];
	return sum;
}

static unsigned short upload_sn = 0;

void LORA_TX_Func(void *tx_msg){
	xData *p = (xData *)tx_msg;
	Lora_TxMSG_T *tx_packet;
	uint8_t msg_dir = 0x00;
	switch(p->iMeaning){
		case FW_Acquire:
			msg_dir = 0x03;
		break;
		case Data_Upload :{
			msg_dir = 0x03;
			p->iMeaning = Data_Upload;
			p->sn = upload_sn++;
		  xSemaphoreTake(xStateMutex,portMAX_DELAY);
			p->iValue = Sensor_Buff;
			p->len = 20;
			xSemaphoreGive(xStateMutex);	
		}
		break;
		default :
			msg_dir = 0x02;
		break;
	}
	tx_packet = Lora_TxMSG_Produce(p->sn,msg_dir,p->iMeaning,p->iValue,p->len);
	LORA_send_data((char *)tx_packet->data,tx_packet->len);
 	free(tx_packet->data);
	free(tx_packet);
}

void *LORA_RX_Func(void *rx_msg){
	xData *p = (xData *)rx_msg;
	xData *re_code = NULL;

	if(p->len >= MSG_MIN_LEN)
		if((p->iValue[0]==0x4A)&&(p->iValue[1]==0x54)){
			uint16_t len = ((uint16_t)p->iValue[17]<<8 )| p->iValue[18];
			uint16_t crc_read = ((uint16_t)p->iValue[19+len]<<8) | p->iValue[20+len];
			if(crc_read == CRC_Calc(p->iValue,len+19)){
				if(strncmp((const char*)&p->iValue[2],(const char*)&Ext_Msg_Struct->Device_Id,8)==0){
					switch(p->iValue[13]){
						case ITEM_NUM:
							{
								re_code = (xData *)pvPortMalloc(sizeof(xData));
								re_code->sn = (uint16_t)p->iValue[10]<<8 | p->iValue[11];
								re_code->iMeaning = (uint16_t)p->iValue[15]<<8 | p->iValue[16];
								re_code->len = len;
								uint8_t *value = (uint8_t *)pvPortMalloc(sizeof(uint8_t)*len);
								memcpy(value,&p->iValue[19],len);
								re_code->iValue = value;
								switch(re_code->iMeaning)
								{
									case Factory_Rst ... Net_Time_Get:
										xQueueSend(SysInfo_Queue,re_code,50/portTICK_RATE_MS);
										break;
									case FW_Update ... FW_Replace:
										xQueueSend(SysInfo_Queue,re_code,50/portTICK_RATE_MS);
										break;
									case Data_Query ... Upload_Int:
										xQueueSend(SensorMonitor_Queue,re_code,50/portTICK_RATE_MS);
										break;
									case DataComp_Get ... DataAcq_Int_Issue:
										xQueueSend(SensorMonitor_Queue,re_code,50/portTICK_RATE_MS);
										break;
									case Light_Dir_Issue ...DevPosi_Query:
										xQueueSend(Direction_Queue,re_code,50/portTICK_RATE_MS);
										break;
									case Abnormal_Posi_Issue ...Abnormal_Event_CLR:
										xQueueSend(Direction_Queue,re_code,50/portTICK_RATE_MS);
										break;
									case LoraModule_Para_Set ... LoraModule_Para_Query:
										xQueueSend(LORAControl_Queue,re_code,50/portTICK_RATE_MS);
										break;
									default:
										break;
								}
								vPortFree(re_code);
							}
							break;
						default:
							break;
					}
				}
			}
		}
		vPortFree(p->iValue);
		p->iValue = NULL;
		return re_code;
}

char wait_reply_msg[20] = "";
char at_cmd_msg[20] = "";

void *LORA_Para_Func(void *msg){
	xData *p = (xData *)msg;
	if(lora_mode == Tsp_Mode){
		switch (p->iMeaning){
			case LoraModule_Para_Set:
				sprintf(LORA_Config_CMD[4].cmd_string,"AT+SPD=%d\r\n",p->iValue[0]);
				sprintf(LORA_Config_CMD[5].cmd_string,"AT+CH=%d\r\n",p->iValue[1]);
				lora_cfg_event = AT_Enter_Event;
				break;
			case LoraModule_Para_Query:
				sprintf(LORA_Config_CMD[4].cmd_string,"AT+SPD=?\r\n");
				sprintf(LORA_Config_CMD[5].cmd_string,"AT+CH=?\r\n");
				lora_cfg_event = AT_Enter_Event;
				break;
			default:
				break;	
		}
	}
	else if((lora_mode == AT_Mode)&&(LORA_Config_FSM->state == Wait_Reply))
	{
		if(strstr((char *)p->iValue,LORA_Config_CMD[cmd_index].reply_string)!=NULL){
			if(cmd_index == AT_CMD_LIST.num_of_element -1)
				lora_cfg_event = Config_Finish_Event;
			else
				lora_cfg_event = Reply_Match_Event;
		}
		else{
			if(retry_times <5 )
				lora_cfg_event = Timeout_Event;
			else 
				lora_cfg_event = Retryout_Event;
		}
	}
	else if(lora_mode == UnInit_Mode)
	{
		sprintf(LORA_Config_CMD[4].cmd_string,"AT+SPD=%d\r\n",LORA_Spd_Default);
		sprintf(LORA_Config_CMD[5].cmd_string,"AT+CH=%d\r\n",LORA_Ch_Default);
		lora_cfg_event = AT_Enter_Event;
	}
	return NULL;
}

void LORACTL_FMS_RUN(void){
	FSM_EventHandle(LORA_Config_FSM,lora_cfg_event);
}

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

//#include "stmflash.h"
#define MSG_MIN_LEN 21
#define UNIQUE_ID_ADDR 0x1FFFF7E8
#define LORA_PARA_ADDR 0x0800FC00
#define HEAD 0x4A54
#define ENCRY_MODE 0x00
#define ITEM_NUM 0x0F
#define LORA_USART 1                                                                                                                                                                                                                                         
#if LORA_USART
	#define LORA_RX_BUFF  rx1_data_buff
#else
	#define LORA_RX_BUFF  rx0_data_buff	
#endif
uint8_t unique_id[8];
uint8_t LORA_Speed_Grade_default = 10;//设定lora模块默认通信速率等级为10 -- 21875bps
uint8_t LORA_Channel_default = 72; //设定lora模块默认信道为72 （389+72MHz）

External_MSG_T *Ext_Msg_Struct = NULL;
Lora_TxMSG_T *Lora_TxMSG_Head = NULL;
Lora_Para_Record_T Lora_Param_Struct;

//LORA引脚初始化
void LORA_GPIO_Init(void){

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
		LORA_Speed_Grade_default=(uint8_t)(LORA_Parameter_IN_Flash>>24);
		LORA_Channel_default = (uint8_t)(LORA_Parameter_IN_Flash>>16);
	}
	Lora_Param_Struct.src_para.spd = LORA_Speed_Grade_default;
	Lora_Param_Struct.src_para.ch = LORA_Channel_default;
}

void LORA_Init(void){
	#if LORA_USART
	  usart_init(1,115200);
	#else
		usart_init(0,115200);
	#endif

		
	  delay_ms(500);
	  LORA_Module_GPIO_Init();	
	  delay_ms(1000);
	  LORA_Hardware_Reset();
	  delay_ms(1000);
	  LORA_Setup_Parameter_Access();
	  //delay_ms(100);
	  LORA_Param_Set();
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
		len = sprintf(command_string,"AT+SPD=%d\r\n",LORA_Speed_Grade_default);
		
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

   	len = sprintf(command_string,"AT+CH=%d\r\n",LORA_Channel_default);
		
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
	#endif
}

void LORA_TR_Func(void){
	if(LORA_Rec_Msg_Head->Next !=NULL){
		Usart_Msg_Link_T *p1 = LORA_Rec_Msg_Head->Next;
		LORA_Rec_Msg_Head->Next = LORA_Rec_Msg_Head->Next->Next;
		LOAR_ExMSG_Handler(p1);
	}
	if(InMsg_Lora_Head->next != NULL){
		Internal_MSG_T *p2 = InMsg_Lora_Head->next;
		InMsg_Lora_Head->next = p2->next;
		LORA_InMSG_Handler(p2);
		free(p2->data_addr);
		p2->data_addr = NULL;
		free(p2);
		p2 = NULL;
	}
	if(Lora_TxMSG_Head->next !=NULL){
		Lora_TxMSG_T *p3 = Lora_TxMSG_Head->next;
		Lora_TxMSG_Head->next = p3->next;
		LORA_send_data((char*)p3->data,p3->len);
		free(p3->data);
		p3->data = NULL;
		free(p3);
		p3 = NULL;
	}
}


uint16_t CRC_Calc(uint8_t *data_buff,uint8_t len){
	
	uint16_t sum=0;
	for(uint8_t count=0;count<len;++count)
		sum+=data_buff[count];
	return sum;
}

//void LORA_MSG_Handler(void){
//	uint8_t i = 0;
//	
//	uint8_t data_len = (Lora_Queue_Front + QueueMAX -Lora_Queue_Rear) % QueueMAX;

//	while(i < ((Lora_Queue_Front + QueueMAX -Lora_Queue_Rear) % QueueMAX)){
//			if((rx0_data_buff[(Lora_Queue_Front+i)%QueueMAX] == 0x4A)&&(rx0_data_buff[(Lora_Queue_Front+i+1)%QueueMAX]==0x54)){
//				if((i+21) <= data_len){
//					uint8_t frame_len = ((uint16_t)rx0_data_buff[(Lora_Queue_Front+17)%QueueMAX]<<8) | ( rx0_data_buff[(Lora_Queue_Front+18)%QueueMAX]);
//					if((i+frame_len+21) <= data_len){
//					   uint8_t  temp[frame_len+21];
//						 if((Lora_Queue_Front+ i + frame_len + 21) < QueueMAX)
//								memcpy(temp,&rx0_data_buff[Lora_Queue_Front+i],frame_len+21);
//						 else{
//								uint8_t fir_cp_num = QueueMAX - Lora_Queue_Front -i;
//							  uint8_t sec_cp_num = (Lora_Queue_Front +i + 21 + frame_len) % QueueMAX + 1;
//								memcpy(temp,&rx0_data_buff[Lora_Queue_Front+i],fir_cp_num);
//								memcpy(&temp[fir_cp_num],&rx0_data_buff[0],sec_cp_num);
//						 }
//						 Lora_Queue_Front += (frame_len+21);
//						 
//					}
//				}
//			}
//			else
//				i++;
//	}
//}

void LOAR_ExMSG_Handler(Usart_Msg_Link_T *p){
	if(p->Data.len >= MSG_MIN_LEN)
		if((p->Data.addr[0]==0x4A)&&(p->Data.addr[1]==0x54)){
			uint16_t len = ((uint16_t)p->Data.addr[17]<<8 )| p->Data.addr[18];
			uint16_t crc_read = ((uint16_t)p->Data.addr[19+len]<<8) | p->Data.addr[20+len];
			if(crc_read == CRC_Calc(p->Data.addr,len+19)){
				if(strncmp((const char*)&p->Data.addr[2],(const char*)&Ext_Msg_Struct->Device_Id,8)==0){
					switch(p->Data.addr[13]){
						case ITEM_NUM:
							{
									Internal_MSG_T *node = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T));
									uint8_t *data_addr =(uint8_t *)malloc(sizeof(uint8_t)*len);
									node->id = Undef;
									node->sn = (uint16_t)p->Data.addr[10]<<8 | p->Data.addr[11];
									node->func_code = (uint16_t)p->Data.addr[15]<<8 | p->Data.addr[16];
									node->len = len;
									memcpy(data_addr,&p->Data.addr[19],len);
									node->data_addr = data_addr;
			            node->next = NULL;
									Internal_MSG_T *tail= Int_Msg_Head;
									while(tail->next !=NULL)
										tail = tail->next;
									tail->next = node;
							}

							break;
						default:
							break;
					}
				};
			}
		}
		free(p->Data.addr);
		p->Data.addr = NULL;
		free(p);
		p = NULL;
}

void LORA_InMSG_Handler(Internal_MSG_T *p){
	Lora_TxMSG_T *Node=NULL,*q;
	switch(p->func_code){
		case LoraModule_Para_Set :
		{
			uint8_t re_code = 0x00;
			Lora_Param_Struct.new_para.ch = p->data_addr[0];
			Lora_Param_Struct.src_para.spd = p->data_addr[1];
			if((Lora_Param_Struct.new_para.ch>72)||(Lora_Param_Struct.new_para.ch<0)||
				(Lora_Param_Struct.new_para.spd>10)||(Lora_Param_Struct.new_para.spd<0))
			
				re_code = 0x02;

			else if((Lora_Param_Struct.new_para.ch == Lora_Param_Struct.src_para.ch)&&
							(Lora_Param_Struct.new_para.spd == Lora_Param_Struct.src_para.spd))
				
				re_code =0x01;
				
			else
			{
				re_code = LoRA_SPD_CH_Set(Lora_Param_Struct.src_para.spd,Lora_Param_Struct.new_para.ch);
				if(re_code)
					Lora_Param_Struct.src_para = Lora_Param_Struct.new_para;
			}
			Node = Lora_TxMSG_Produce(p->sn,0x02,p->func_code,&re_code,1);
		}
			break;
		case LoraModule_Para_Query :
		{
			int8_t para_query[2];
			LoRA_SPD_CH_Query(&para_query[0],&para_query[1]);
			if((para_query[0] != -1)&&(para_query[1] != -1)){
				Lora_Param_Struct.src_para.ch = para_query[0];
				Lora_Param_Struct.new_para.spd = para_query[1];
			}
			Node = Lora_TxMSG_Produce(p->sn,0x02,p->func_code,(uint8_t *)para_query,2);
		}
			break;
		case Data_Upload:
		{
			Node = Lora_TxMSG_Produce(p->sn,0x03,p->func_code,p->data_addr,p->len);
		
		}	
			break;
		default:{
			Node = Lora_TxMSG_Produce(p->sn,0x02,p->func_code,p->data_addr,p->len);
		}	
		break;
	}
		q = Lora_TxMSG_Head;
			while(q->next !=NULL)
				q = q->next;
		q->next = Node;
}

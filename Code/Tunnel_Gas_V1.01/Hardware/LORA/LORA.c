#include "LORA.h"
#include "delay.h"
#include "usart.h"
#include "string.h"
#include "led.h"
#include "stdlib.h"
#include "stmflash.h"

uint8_t LORA_Speed_Grade_default = 10;//设定lora模块默认通信速率等级为10 -- 21875bps
uint8_t LORA_Channel_default = 72; //设定lora模块默认信道为72 （389+72MHz）
LORA_Rec_Msg_Link_T *LORA_Rec_Msg_Head = NULL;
//LORA引脚初始化
void LORA_GPIO_Init(void){
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能LORA模块电源控制引脚,GPIOD_8

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //开漏模式
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //无上下拉
		
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		GPIO_SetBits(GPIOD,GPIO_Pin_8);
}
void LORA_Setup_Parameter_Access(void){
	uint32_t LORA_Parameter_IN_Flash[2];
	STMFLASH_Read(Group_LORA_Speed_Grade,LORA_Parameter_IN_Flash,2);
  if(LORA_Parameter_IN_Flash[0]!=0xffffffff)
		LORA_Speed_Grade_default=LORA_Parameter_IN_Flash[0];
	if(LORA_Parameter_IN_Flash[1]!=0XFFFFFFFF)
		LORA_Channel_default = LORA_Parameter_IN_Flash[1];
}
void LORA_Init(void){
	  uart_init(USART1,115200);
	  delay_ms(500);
	  LORA_GPIO_Init();	
	  delay_ms(1000);
	  LORA_Hardware_Reset();
	  delay_ms(1000);
	  LORA_Setup_Parameter_Access();
	  delay_ms(100);
	  LORA_test();
	  memset(USART_RX_BUF[0],0,USART_REC_LEN);
		LORA_Rec_Msg_Head = (LORA_Rec_Msg_Link_T*)malloc(sizeof(LORA_Rec_Msg_Link_T));
		LORA_Rec_Msg_Head->next = NULL;
}

void LORA_Hardware_Reset(void){

			LORA_PowerEN=0;
			delay_ms(1000);
			LORA_PowerEN=1;

}

Error_Code_t LORA_test(void){
	Error_Code_t Error=1;
	uint16_t timeout=10;
	uint8_t point=0;
	
	//进入AT指令模式
	do{
		point=0;
		printf("+++\r\n");
		memset(USART_RX_BUF[0],0,USART_REC_LEN);	
		LORA_send_data("+++",sizeof("+++")-1);
		delay_ms(200);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[0]);
		while((USART_RX_BUF[0][point++]==0x00)&&(point<=60));
	}
	while((strchr((const char*) &(USART_RX_BUF[0][point-1]),'a')==NULL)&&(timeout>0));
	if(timeout==0) Error=0;
	
	timeout=10;
	do{
		point=0;
		printf("a\r\n");
		memset(USART_RX_BUF[0],0,USART_REC_LEN);	
		LORA_send_data("a",sizeof("a")-1);
		delay_ms(200);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[0]);
		while((USART_RX_BUF[0][point++]==0x00)&&(point<=60));
	}
	while((strstr((const char*) &(USART_RX_BUF[0][point-1]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) Error=0;
	
	//关闭指令回显
	timeout=10;
	do{
		printf("AT+E=OFF\r\n");
		memset(USART_RX_BUF[0],0,USART_REC_LEN);	

		LORA_send_data("AT+E=OFF\r\n",sizeof("AT+E=OFF\r\n")-1);
		delay_ms(500);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[0]);
	}
 	while((strstr((const char*) &(USART_RX_BUF[0][0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) Error=0;
	
	//设置工作模式 ：透传模式 AT+WMODE=TRANS 定点模式AT+WMODE=FP
	timeout=10;
	do{
		printf("AT+WMODE=TRANS\r\n");
		memset(USART_RX_BUF[0],0,USART_REC_LEN);	

		LORA_send_data("AT+WMODE=TRANS\r\n",sizeof("AT+WMODE=TRANS\r\n")-1);
		delay_ms(500);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[0]);
	}
 	while((strstr((const char*) &(USART_RX_BUF[0][0]),"OK")==NULL)&&(timeout>0));
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
		char command_string[]="AT+SPD=5\r\n";
		sprintf(command_string,"AT+SPD=%d\r\n",LORA_Speed_Grade_default);

		memset(USART_RX_BUF[0],0,USART_REC_LEN);	

		LORA_send_data(command_string,sizeof(command_string)-1);
		delay_ms(500);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[0]);
	}
 	while((strstr((const char*) &(USART_RX_BUF[0][0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) Error=0;

	/**********************************************
	设置信道 AT+CH=<ch><CR><LF>
	ch: L101-L:0~127(default:72,对应470MHZ)  (398+ch)MHZ
	    L101-H:0~127(default:65,对应868MHZ)  (803+ch)MHZ
	本次项目所用为L101-H系列模块
	***********************************************/
	timeout=10;
	do{
	
		char command_string[] = "AT+CH=72\r\n";
    sprintf(command_string,"AT+CH=%d\r\n",LORA_Channel_default);		
   
		memset(USART_RX_BUF[0],'\0',USART_REC_LEN);	
		LORA_send_data(command_string,sizeof(command_string)-1);
		delay_ms(500);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[0]);
	}
	while((strstr((const char*) &(USART_RX_BUF[0][0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) Error=0;
	
		/**********************************************
	设置目标地址AT+ADDR=<addr><CR><LF>
	addr：0（default）~65535 65535为广播地址，同信道同速率的模块都能接收
	***********************************************/
	timeout=10;
	do{
		printf("AT+ADDR SET\r\n");
		memset(USART_RX_BUF[0],'\0',USART_REC_LEN);	
		LORA_send_data("AT+ADDR=65535\r\n",sizeof("AT+ADDR=65535\r\n")-1);
		delay_ms(500);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[0]);
	}
	while((strstr((const char*) &(USART_RX_BUF[0][0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) Error=0;

		/**********************************************
	重启模块
	***********************************************/
	timeout=10;
	do{
		printf("restart Moudle\r\n");
		memset(USART_RX_BUF[0],'\0',USART_REC_LEN);	
		LORA_send_data("AT+Z\r\n",sizeof("AT+Z\r\n")-1);
		delay_ms(1000);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[0]);
	}
	while((strstr((const char*) &(USART_RX_BUF[0][0]),"Start")==NULL)&&(timeout>0));
	if(timeout==0)  Error=0;
	else{
		uint8_t i=0;
		while(USART_RX_BUF[0][i++]!='\0'){
			USART_SendData(USART3,USART_RX_BUF[0][i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
		}
	
	}
//	/**********************************************
//	退出AT 指令模式
//	***********************************************/
//	timeout=10;
//	do{
//		printf("EXIT AT Command Mode\r\n");
//		memset(USART_RX_BUF[0],'\0',USART_REC_LEN);	
//		LORA_send_data("AT+ENTM\r\n",sizeof("AT+ENTM\r\n")-1);
//		delay_ms(500);
//		timeout--;
//		printf("%s\r\n",USART_RX_BUF[0]);
//	}
//	while((strstr((const char*) &(USART_RX_BUF[0][0]),"OK")==NULL)&&(timeout>0));
	//if(timeout==0)  Error=0;
	
	return Error;
}

//重启模块
_Bool LoRA_Restart_Mode(void){

	_Bool Error=1;
	uint8_t timeout=10;
	timeout=10;
	do{
		memset(USART_RX_BUF[0],'\0',USART_REC_LEN);	
		LORA_send_data("AT+Z\r\n",sizeof("AT+Z\r\n")-1);
		delay_ms(1000);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[0]);
	}
	while((strstr((const char*) &(USART_RX_BUF[0][0]),"Start")==NULL)&&(timeout>0));
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
		memset(USART_RX_BUF[0],0,USART_REC_LEN);	
		LORA_send_data("+++",sizeof("+++")-1);
		delay_ms(200);
		timeout--;
		while((USART_RX_BUF[0][point++]==0x00)&&(point<=60));
	}
	while((strchr((const char*) &(USART_RX_BUF[0][point-1]),'a')==NULL)&&(timeout>0));
	if(timeout==0) {
		Error=0;
		return Error;
	}
	timeout=10;
	do{
		point=0;
		printf("a\r\n");
		memset(USART_RX_BUF[0],0,USART_REC_LEN);	
		LORA_send_data("a",sizeof("a")-1);
		delay_ms(200);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[0]);
		while((USART_RX_BUF[0][point++]==0x00)&&(point<=60));
	}
	while((strstr((const char*) &(USART_RX_BUF[0][point-1]),"OK")==NULL)&&(timeout>0));
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
	STMFLASH_Read(Group_LORA_Speed_Grade,&speed_in_flash_u32,1);
	speed_in_flash_u8=(uint8_t)speed_in_flash_u32;
	if(speed_in_flash_u8!=new_speed)
		STMFLASH_Write(Group_LORA_Speed_Grade,(uint32_t *)&new_speed,1);
	//查询模块当前速率等级，如果与将要设置等级一致，则无需修改，直接返回 1
  ori_speed=LoRA_Query_Speed_Grade();
  if(ori_speed==new_speed)
		return 1;
	
	//模块设置为指定信道，失败则返回0
	delay_ms(50);
	timeout=10;
	sprintf(command,"AT+SPD=%d\r\n",new_speed);
	do{

			memset(USART_RX_BUF[0],0,USART_REC_LEN);	

			LORA_send_data(command,sizeof(command)-1);
			delay_ms(500);
			timeout--;
	}
	while((strstr((const char*) &(USART_RX_BUF[0][0]),"OK")==NULL)&&(timeout>0));
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
		
		memset(USART_RX_BUF[0],'\0',USART_REC_LEN);	
		LORA_send_data("AT+SPD\r\n",sizeof("AT+SPD\r\n")-1);
		timeout--;
		point=strstr((const char*) &(USART_RX_BUF[0][0]),"+SPD:");
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
	
	STMFLASH_Read(Group_LORA_Channel,&channel_in_flash_u32,1);
	channel_in_flash_u8=(uint8_t)channel_in_flash_u32;
	if(channel_in_flash_u8!=new_ch)
		STMFLASH_Write(Group_LORA_Channel,(uint32_t *)&new_ch,1);
	
	//查询模块当前信道，如果与将要设置的信道一致，则不设置。直接返回1
  ori_channel=LoRA_Query_Channel();
  if(ori_channel==new_ch)
		return 1;

	//修改模块信道为预设信道，失败则返回0
	timeout=10;
	delay_ms(50);
	sprintf(command,"AT+CH=%d\r\n",new_ch);
	do{

			memset(USART_RX_BUF[0],0,USART_REC_LEN);	

			LORA_send_data(command,sizeof(command)-1);
			delay_ms(500);
			timeout--;
	}
	while((strstr((const char*) &(USART_RX_BUF[0][0]),"OK")==NULL)&&(timeout>0));
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
		
		memset(USART_RX_BUF[0],'\0',USART_REC_LEN);	
		LORA_send_data("AT+CH\r\n",sizeof("AT+CH\r\n")-1);
		timeout--;
		point=strstr((const char*) &(USART_RX_BUF[0][0]),"+CH:");
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
	
	delay_ms(100);
	
	
	 
	return error;

}

//MCU发送数据给LoRA模块
void LORA_send_data(char *sendbuff,uint16_t length){
	char *data_to_send=NULL;
	data_to_send=sendbuff;
	while(length--){
		USART_SendData(USART1,*(data_to_send++));
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);	}
}

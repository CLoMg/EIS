#include "ec20.h"
#include "stm32f4xx_gpio.h"
#include "delay.h"
#include "usart.h"
#include "string.h"
#include "led.h"
#include "stmflash.h"
#include "LORA.h"

//char ip_addr_connect[]="AT+QIOPEN=1,0,\"TCP\",\"182.140.146.130\",20010,0,2\r\n";//公网
char ip_addr_connect[]="AT+QIOPEN=1,0,\"TCP\",\"103.46.128.49\",51959,0,2\r\n";//mine
//char ip_addr_connect[]="AT+QIOPEN=1,0,\"TCP\",\"103.46.128.21\",52488,0,2\r\n";//cy
//char ip_addr_connect[]="AT+QIOPEN=1,0,\"TCP\",\"169.254.223.88\",20010,0,2\r\n";//cy

//char ip_addr_connect[]="AT+QIOPEN=1,0,\"TCP\",\"103.46.128.53\",38038,0,2\r\n";//YM
//char ip_addr_connect_bak[]="AT+QIOPEN=1,0,\"TCP\",\"103.46.128.49\",51959,0,2\r\n";//mine
char ip_addr_connect_bak[]="AT+QIOPEN=1,0,\"TCP\",\"182.140.146.130\",20010,0,2\r\n";//公网

//char ip_addr_connect[]="AT+QIOPEN=1,0,\"TCP\",\"103.46.128.45\",26071,0,2\r\n";//LT

char connect_state[]="AT+QISTATE?\r\n";
_Bool ec20_reconnect_flag=0;

/************初始化EC20************/
/**********************************
	1、 串口初始化
	2、Power 和 Reset引脚初始化
	3、AT指令测试 关闭回显 CPIN测试
	4、连接TCP Server
***********************************/
void ec20_get_server_addr(uint32_t *addr_read){
	uint8_t server_addr_u8[8];
	STMFLASH_Read(Server_Addr_Memory,addr_read,2);
  if((addr_read[0]!=0xffffffff)&&(addr_read[1]!=0xffffffff)){
		memcpy(server_addr_u8,addr_read,8);
		sprintf(ip_addr_connect,"AT+QIOPEN=1,0,\"TCP\",\"%d.%d.%d.%d\",%d,0,2\r\n",server_addr_u8[0],server_addr_u8[1],server_addr_u8[2],server_addr_u8[3],(uint16_t)server_addr_u8[4]<<8|server_addr_u8[5]);
	}
}

Error_Code_t ec20_modify_server_addr(uint32_t *addr_to_write){
	Error_Code_t error_code=0;
	uint32_t server_addr_read[2];
	uint32_t flash_para_bak[27]={};
	STMFLASH_Read(User_Parameter_BASE,flash_para_bak,27);
	memcpy(&flash_para_bak[(Server_Addr_Memory-User_Parameter_BASE)/4],addr_to_write,2);
	STMFLASH_Write(User_Parameter_BASE,flash_para_bak,27);
	//STMFLASH_Write(Server_Addr_Memory,addr_to_write,2);
	ec20_get_server_addr(server_addr_read);
	if(memcmp((u8*)addr_to_write,(u8*)server_addr_read,6)==0)
		error_code=1;
	return error_code;

}
void ec20_init(void){
	uint32_t dummy_ip_addr[2];
	ec20_reconnect_flag=0;
	uint16_t timeout=10;
	ec20_get_server_addr(dummy_ip_addr);
	uart_init(USART2,115200);//初始化Usart2
	delay_ms(200);
	memset(USART_RX_BUF[1],0,USART_REC_LEN);
	
	ec20_gpio_config();

	ec20_Hardware_Reset();
	while((!ec20_test())&&(timeout>0)){
		timeout--;
		ec20_Hardware_Reset();
		
	}
	if(timeout==0) 
		printf("ec20 initialize fialed\r\n");
	else
		printf("ec20 initialize success\r\n");

	timeout=0;
	while(!ec20_connnect()){
	  memset(USART_RX_BUF[1],0,USART_REC_LEN);	
	  ec20_send_data("AT+QICLOSE=0,1\r\n",sizeof("AT+QICLOSE=0,1\r\n"));
	  //while(strstr((const char*)&USART_RX_BUF[1],"OK")==NULL)
	  delay_ms(1000);
		ec20_Hardware_Reset();
		ec20_test();
		timeout++;
		if(timeout>5)
			memcpy(ip_addr_connect,ip_addr_connect_bak,sizeof(ip_addr_connect_bak));
	}
	printf("ec20 connect success\r\n");
	
}

//EC20 Power Reset 引脚初始化
void ec20_gpio_config(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//使能EC20_PERSTN控制引脚,旧版本 GPIOG_11

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //开漏模式
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //无上下拉
//	
//	GPIO_Init(GPIOG, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOG,GPIO_Pin_11);

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能EC20_PERSTN控制引脚,新版本 GPIOC_1

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //开漏模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //无上下拉
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC,GPIO_Pin_1);
	
	delay_ms(100);
	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能EC20_PoweEN控制引脚,旧版本PA15

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //PA15
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //开漏模式
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
//	
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOA,GPIO_Pin_15);
//	delay_ms(1000);
//	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能EC20_PoweEN控制引脚,新版本PA3

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //	PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //开漏模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
	delay_ms(1000);
}

//EC20硬件复位
void ec20_Hardware_Reset(void){
	uint16_t cnt;
	
	do{
			cnt=60;

			ec20_PERSTN=0;
			delay_ms(1000);
			ec20_PERSTN=1;
			for(int i=0;i<10;i++){
				led_beep(1,100);
			}
			while((strstr((const char*) &(USART_RX_BUF[1][0]),"RDY")==NULL)&&(cnt>0)){
				cnt--;

				memset(USART_RX_BUF[1],0,USART_REC_LEN);
				for(int i=0;i<4;i++){
					led_beep(1,100);
				}

			}
		}while(cnt==0);

		printf("Power on success!\r\n");
}


//EC20软件复位
void ec20_Software_Reset(void){
	uint16_t cnt=60;
	memset(USART_RX_BUF[1],0,USART_REC_LEN);
	ec20_send_data("AT+CFUN=1,1\r\n",sizeof("AT+CFUN=1,1\r\n"));
	delay_ms(300);
	while((strstr((const char*) &(USART_RX_BUF[1][0]),"RDY")==NULL)&&(--cnt>0)){
		printf("%s\r\n",USART_RX_BUF[1]);
		memset(USART_RX_BUF[1],'\0',USART_REC_LEN);
		delay_ms(500);
	}
	if(cnt==0)
		ec20_Hardware_Reset();
	printf("reset  success!\r\n");
}


/**********EC20 Test**********
	1、AT指令测试
	2、ATE0关闭回显
	3、AT+CPIN 检测SIM卡插入
******************************/

Error_Code_t ec20_test(void){
	Error_Code_t Error=1;
	uint16_t timeout=10;
	do{
		delay_ms(5000);
		printf("AT TEST\r\n");
		memset(USART_RX_BUF[1],0,USART_REC_LEN);	
		ec20_send_data("AT\r\n",sizeof("AT\r\n"));
		delay_ms(200);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[1]);
	}
	while((strstr((const char*) &(USART_RX_BUF[1][0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) Error=0;
	
	timeout=10;
	do{
		printf("ATEO\r\n");
		memset(USART_RX_BUF[1],0,USART_REC_LEN);	
		ec20_send_data("ATE0\r\n",sizeof("ATE0\r\n"));
		delay_ms(200);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[1]);
	}
	while((strstr((const char*) &(USART_RX_BUF[1][0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0) Error=0;
	
	timeout=10;
	do{
		printf("AT+CPIN TEST\r\n");
		memset(USART_RX_BUF[1],'\0',USART_REC_LEN);	
		ec20_send_data("AT+CPIN?\r\n",sizeof("AT+CPIN?\r\n"));
		delay_ms(200);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[1]);
	}
	while((strstr((const char*) &(USART_RX_BUF[1][0]),"READY")==NULL)&&(timeout>0));
	if(timeout==0)  Error=0;
	
	timeout=10;
	char *rec_point;
	do{
		rec_point=NULL;
		printf("AT+QCCID TEST\r\n");
		memset(USART_RX_BUF[1],'\0',USART_REC_LEN);	
		ec20_send_data("AT+QCCID\r\n",sizeof("AT+QCCID\r\n"));
		delay_ms(200);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[1]);
		rec_point=strstr((const char*) &(USART_RX_BUF[1][0]),"+QCCID:");
	}
	while((rec_point==NULL)&&(timeout>0));
	if(timeout==0)  
		Error=0;
	else{
		LORA_send_data(rec_point,30);
		while(*rec_point!='\0'){
			USART_SendData(USART3,*(rec_point++));
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
		}
	}
	
	
	
	timeout=10;
	do{
		printf("AT+CEREG TEST\r\n");
		memset(USART_RX_BUF[1],'\0',USART_REC_LEN);	
		ec20_send_data("AT+CEREG?\r\n",sizeof("AT+CEREG?\r\n"));
		delay_ms(200);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[1]);
	}
	while((strstr((const char*) &(USART_RX_BUF[1][0]),"+CEREG:")==NULL)&&(timeout>0));
	if(timeout==0)  Error=0;
	
	timeout=10;
	do{
		printf("AT+QICSGP TEST\r\n");
		memset(USART_RX_BUF[1],'\0',USART_REC_LEN);	
		ec20_send_data("AT+QICSGP=1,1,\"CMNET\",\"\",\"\",1\r\n",sizeof("AT+QICSGP=1,1,\"CMNET\",\"\",\"\",1\r\n"));
		delay_ms(200);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[1]);
	}
	while((strstr((const char*) &(USART_RX_BUF[1][0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0)  Error=0;
	
	timeout=10;
	do{
		printf("AT+QIDEACT TEST\r\n");
		memset(USART_RX_BUF[1],'\0',USART_REC_LEN);	
		ec20_send_data("AT+QIDEACT=1\r\n",sizeof("AT+QIDEACT=1\r\n"));
		delay_ms(200);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[1]);
	}
	while((strstr((const char*) &(USART_RX_BUF[1][0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0)  
		Error=0;
	else 
		delay_ms(5000);
	timeout=10;
	do{
		printf("AT+QIACT TEST\r\n");
		memset(USART_RX_BUF[1],'\0',USART_REC_LEN);	
		ec20_send_data("AT+QIACT=1\r\n",sizeof("AT+QIACT=1\r\n"));
		delay_ms(200);
		timeout--;
		printf("%s\r\n",USART_RX_BUF[1]);
	}
	while((strstr((const char*) &(USART_RX_BUF[1][0]),"OK")==NULL)&&(timeout>0));
	if(timeout==0)  Error=0;
	
	return Error;
}

void ec20_send_data(char *sendbuff,uint16_t length){
	char *data_to_send=NULL;
	data_to_send=sendbuff;
	while(length--){
		USART_SendData(USART2,*(data_to_send++));
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);	}
}

/**********EC20 Tcp连接**********
连接count次 失败则返回Error 再重新初始化
******************************/
Error_Code_t ec20_connnect(void){
	Error_Code_t Error=0;
	uint16_t count=20;
	delay_ms(10000);
	do{
		    delay_ms(2000);
				count--;
				printf("connnect to TCP\r\n");
				
				memset(USART_RX_BUF[1],0,USART_REC_LEN);	
				ec20_send_data(ip_addr_connect,sizeof(ip_addr_connect));
				
				led_beep(1,500);

				printf("%s\r\n",USART_RX_BUF[1]);
			}
	while((strstr((const char*) &(USART_RX_BUF[1][0]),"CONNECT")==NULL)&&(count>0));
	delay_ms(3000);
	if((strstr((const char*) &(USART_RX_BUF[1][0]),"NO CARRIER")!=NULL))
		return Error;
	char *rec_point = (char *)USART_RX_BUF[1];
	//串口3和LORA接口输出连接信息
	LORA_send_data(rec_point,30);
	while(*rec_point!='\0'){
			USART_SendData(USART3,*(rec_point++));
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
	}
	memset(USART_RX_BUF[1],0,USART_REC_LEN);
	if(count>0){
		Error=1;
		LED1=0 ;
	}
	return Error;
}

/**********EC20 Tcp断连后重新连接**********
1、+++退出透传
2、AT+QICLOSE 关闭连接
*******************************************/
void ec20_reconnnect(void){
	uint32_t dummy_ip_addr[2];
	memset(USART_RX_BUF[1], 0, USART_REC_LEN);

	delay_ms(1000);
	delay_ms(1000);
	ec20_send_data("+++",sizeof("+++")-1);
	delay_ms(1000);
	delay_ms(1000);
	memset(USART_RX_BUF[1],0,USART_REC_LEN);	
	ec20_send_data("AT+QICLOSE=0,1\r\n",sizeof("AT+QICLOSE=0,1\r\n"));
	//while(strstr((const char*)&USART_RX_BUF[1],"OK")==NULL)
	delay_ms(1000);
	ec20_get_server_addr(dummy_ip_addr);
	if(!ec20_connnect())
		ec20_init();
	//ec2clw0_init();
	
}




#include "usr_k7.h"
#include "usart.h"
#include "delay.h"
#include "string.h"
char data_to_send[30]="AT+";
char command_head[]="AT+";
char IP_SET[]="WANN=static,192.168.0.10,255.255.0,192.168.0.1\r";
char DNS_SET[]="DNS=208.67.222.222\r";


//Reset -- PD14 Reload -- PD15��ʼ��
void usr_k7_gpio_config(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOB����ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15; //GPIOD14��GPIOD15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //��©ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //��������
	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_14|GPIO_Pin_15);
}
/*��ʼ��ģ��
1����ѯ���Թ��� [�رջ���]
2����ѯ����״̬
3���޸Ĺ�����ʽ
4����ѯWAN��IP  [����IP]
5����ѯDNS��������ַ [����DNS��������ַ]
6��������������
7������͸��ģʽ*/
void usr_k7_init(void){
	uart_init(USART3,115200);
	delay_ms(100);
	usr_k7_gpio_config();
	delay_ms(100);
//	usr_echo_set(0);
//	usr_uartclbuf_set(1);
//	usr_wann_set();
//	usr_dns_set ();
	
	usr_data_send("init success!",sizeof("init success!"));
	memset(USART_RX_BUF[2],0,USART_REC_LEN);
}

//��ѯ ����״̬ 0--�ر� 1--���� -1--����
int8_t usr_echo_query(){

	char command[]="E\r";
	char reply[30];
	

	usr_write_read(command,reply);
	if((strstr(reply,"+OK=ON")))
		return 1;
	else if((strstr(reply,"+OK=OFF")))
		return 0;
	else
		return -1;
}

//���û���״̬ 0--�ر� 1--���� 
Error_Code_t usr_echo_set(int8_t newstate){
	Error_Code_t Error=0;
	char echo_state[2][6]={"E=OFF\r","E=ON\r "};
	char reply[30];
	
	usr_write_read(echo_state[newstate],reply);
	if(strstr(reply,"+OK")){
		if(newstate==usr_echo_query())
			Error=1;
		else 
			Error=0;
	}
	else 
		Error=0;
	return Error;
}

Error_Code_t usr_reset(){
	Error_Code_t Error=0;
	char command[]="Z\r";
	char reply[30];
	
	usr_write_read(command,reply);
	if(strstr(reply,"+OK")){
		Error=1;
	}
	else 
		Error=0;
	return Error;
}

Error_Code_t usr_transparent_mode (){
	Error_Code_t Error=0;
	char command[]="ENTM\r";
	char reply[30];
	
	usr_write_read(command,reply);
	if(strstr(reply,"+OK")){
		Error=1;
	}
	else 
		Error=0;
	return Error;
}

Error_Code_t usr_Mac_query (){
	Error_Code_t Error=0;
	char command[]="MAC\r";
	char reply[30];
	char MAC_Addr[14]={0,};
	char *point=NULL;
	
	usr_write_read(command,reply);
	if((point=strstr(reply,"+OK"))!=NULL){
		uint8_t i=0;
		while(point[i+4]!='\0'){
			MAC_Addr[i]=point[i+4];
			++i;
		}
		Error=1;
	}
	else {
		Error=0;
		MAC_Addr[0]=0;
	}
	
	return Error;
}

Error_Code_t usr_default_mode (){
	Error_Code_t Error=0;
	char command[]="RELD\r";
	char reply[30];
	
	usr_write_read(command,reply);
	if(strstr(reply,"+OK")){
		Error=1;
	}
	else 
		Error=0;
	return Error;
}

Error_Code_t usr_wann_query (char *info_buff){
	Error_Code_t Error=0;
	char command[]="WANN\r";
	char reply[30],WAN_INFO[50];
	char *point=NULL;
	
	usr_write_read(command,reply);
	if((point=strstr(reply,"+OK"))){
		uint8_t i=0;
		while(point[i+4]!='\0'){
			WAN_INFO[i]=point[i+4];
			++i;
		}
		info_buff=WAN_INFO;
		Error=1;
	}
	else 
		Error=0;
	return Error;
}

Error_Code_t usr_wann_set (){
	Error_Code_t Error=0;
	char reply[30];
//	char WAN_INFO[50];
	char WANN_Read[50];
	
	usr_write_read(IP_SET,reply);
	if(strstr(reply,"+OK")){
		delay_ms(200);
		usr_wann_query(WANN_Read);
		if(strstr(WANN_Read,&IP_SET[5])!=NULL)
			Error=1;
		else
			Error=0;
	}
	else 
		Error=0;
	return Error;
}


Error_Code_t usr_dns_query (){
	Error_Code_t Error=0;
	char command[]="DNS\r";
	char reply[30],DNS_INFO[20];
	char *point=NULL;
	
	usr_write_read(command,reply);
	if((point=strstr(reply,"+OK"))){
		uint8_t i=0;
		while(point[i+4]!='\0'){
			DNS_INFO[i]=point[i+4];
			++i;
		}
		Error=1;
	}
	else 
		Error=0;
	return Error;
}

Error_Code_t usr_dns_set (){
	Error_Code_t Error=0;
	char reply[30];
	char DNS_Read[50];

	usr_write_read(DNS_SET,reply);
	if(strstr(reply,"+OK")){
		delay_ms(200);
		usr_wann_query(DNS_Read);
		if(strstr(DNS_Read,&IP_SET[5])!=NULL)
			Error=1;
		else
			Error=0;
	}
	else 
		Error=0;
	return Error;
}


//��ѯģ������ǰ�Ƿ������ڻ���
uint8_t usr_uartclbuf_query (){

	char command[]="UARTCLBUF\r";
	char reply[30];

	usr_write_read(command,reply);
	if((strstr(reply,"+OK=ON")))
		return 1;
	else if((strstr(reply,"+OK=OFF")))
		return 0;
	else
		return -1;
}

//��������ǰ�Ƿ������ڻ��� 0--������ 1--����
Error_Code_t usr_uartclbuf_set (int8_t newstate){
	Error_Code_t Error=0;
	char echo_state[2][15]={"UARTCLBUF=OFF\r","UARTCLBUF=ON\r"};
	char reply[30];
	
	usr_write_read(echo_state[newstate],reply);
	if(strstr(reply,"+OK")){
		if(newstate==usr_uartclbuf_query())
			Error=1;
		else 
			Error=0;
	}
	else 
		Error=0;
	return Error;
}


char *usr_write_read(char *command,char *read_buffer){
	char data_to_send[30]="AT+";
	strcat(data_to_send,command);
	usr_data_send(data_to_send,sizeof(data_to_send));
	delay_ms(300);
	usr_data_receive(read_buffer);
	return 0;
}

// ���ڷ������ݣ������Լ�ƽ̨�Ĵ��ڷ��ͺ������
void usr_data_send(char *sendbuff,uint16_t length){
	char *data_to_send=NULL;
	data_to_send=sendbuff;
	while(length--){
		USART_SendData(USART3,*(data_to_send++));
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);	}
}

void usr_data_receive(char *data_read){
	
// ���ڽ������ݣ������Լ�ƽ̨�Ĵ��ڽ��պ������	
	memcpy(data_read,USART_RX_BUF[2],USART_RX_CNT[2]);
	USART_RX_CNT[2]=0;

}


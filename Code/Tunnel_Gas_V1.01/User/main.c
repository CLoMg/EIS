#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "mh4xx.h"
#include "rs485.h"
#include "max30105.h"
#include "timer.h"
#include "command.h"
#include "sht3x.h"
#include "wdg.h"
#include "ec20.h"
#include "pms7003.h"
#include "led.h"
#include "usr_k7.h"
#include "string.h"
#include "motor.h"
#include "adc.h"
#include "uestc_gas.h"
#include "mmdxx.h"
#include "LORA.h"
#include "upgrade.h"
#include "stdlib.h"
#include "task.h"

#define test 0
RCC_ClocksTypeDef get_rcc_clock;

extern _Bool Device_ID_Update_Flag;

int main(void){

	uint8_t voltage[2];
	char vol_info[]="voltage:%03d";
	char device_type_str[]="device type:%d\r\n";
	SCB->VTOR = FLASH_BASE|0x10000;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	led_init();
	Sensor3V3_Power_init();
	Sensor3V3_Power=1;
	Sensor_5V_PowerEN_Init();
	Sensor_5V_PowerEN=1;
	delay_init(168);
	//Adc_Init();
	delay_ms(1000);
	//Voltage_Monitor(voltage);
	uart_init(USART3,115200);//��ʼ��Usart3
	delay_ms(100);
	
	LORA_Init();
	

	sprintf(device_type_str,"Device Type:%d\r\n",device_type_num-1);
	LORA_send_data(device_type_str,sizeof(device_type_str));
	
//�豸����0--κ��ɽ/��Ϫ���A��: U1:CH4���ʢ�� U4:NH3(�ʢ) U5:CO(�ʢ) U6:PM2.5/PM10
if(device_type_num==0){
	PMS_init();
	MH4xx_ZE_Init_All(9600);
}

//�豸1--κ��ɽ/��Ϫ���B�ͣ�U1:SO2���ʢ�� U4:H2S(�ʢ) U5:NO2(�ʢ) U6:CO2(�ʢ)
else if (device_type_num==1)
	MH4xx_ZE_Init_All(9600);

//�豸����2--����/�ɵ��� I��     U4:CH4(�ʢ) U5:NH3(�ʢ) U6:PM2.5/PM10
else if (device_type_num==2){
  MH4xx_ZE_Init_All(9600);
	PMS_init();	
}
//�豸����3--����       II��    U4:CO(�ʢ)  U5:H2S(�ʢ) 
else if (device_type_num==3)
	MH4xx_ZE_Init_All(9600);

//�豸����4--          III��    U4:SO2(�ʢ)  U5:NO2(�ʢ�� U6:CO2(�ʢ) 
else if (device_type_num==4)
	MH4xx_ZE_Init_All(9600);
// �������ʹ����
else if (device_type_num==5){
	MH4xx_ZE_Init_All(9600);
	PMS_init();	
}

else;

	command_recieved_flag_4G=0;
	command_recieved_flag_ETH=0;
	command_recieved_flag_LORA=0;

  SHT30_Init();
	max30105_init();
	Motor_Init();
	Data_Package_init();
	delay_ms(500);
	
	#if gateway
 	usr_k7_init();
	ec20_init();
	#endif
	Timer_Init();
	#if iwatch_dog_work
		IWDG_Init(6,4095);
  #endif
#if test==0
	LED1=0;
	LED2=1;
#else
	LED1=1;//D4
	LED2=0;
#endif
  while(1){		
		TaskProcess();
//		if(Node_Upgrade_Flag){
//				firmware_send_process();
//				Node_Upgrade_Flag=0;
//		}
		delay_ms(10);
		}
}


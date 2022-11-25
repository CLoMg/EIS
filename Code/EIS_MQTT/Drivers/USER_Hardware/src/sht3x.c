#include "SHT3x.h"
#include "myiic.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "delay.h"
#include "selfcheck.h"

#define hardwareiic 0
unsigned char sht_iic_num=1;
unsigned char Refresh_SHT30_Data=0;


void SHT30_Init(void)
{
	uint8_t dummy_buff[4];
	uint8_t i=0;

	#if hardwareiic
	//I2Cx_Init(I2C1);
	#else
	IIC_Init(sht_iic_num);
	#endif
}	

unsigned char SHT30_Acquire (unsigned char slave_addr,uint8_t *data_buff){
	uint8_t *read_buff;
	read_buff=data_buff;
	unsigned char Error=0;
	uint8_t count=5;
	while((!(Error=SHT30_read_result(slave_addr,read_buff)))&&((count--)>0));

//	if(!Error)
//	{
//		read_buff[0]=0xFF;
//		read_buff[1]=0xFF;
//		read_buff[2]=0xFF;
//		read_buff[3]=0xFF;
//	}
	////printf("sensor %d data is %d\r\n",sensor_num,((uint16_t)sensor_data[0]<<8|sensor_data[1]));
	return Error;
}
/*******************************************************************

 温湿度获取函数               

函数原型: SHT30_read_result(unsigned char addr);
功能: 用来接收从器件采集并合成温湿度

********************************************************************/ 
float Temperature=0;
float Humidity=0;
unsigned char SHT30_read_result(unsigned char slave_addr,uint8_t *data_buff)

{
	
	uint16_t cnt=65535;
	unsigned char Error=0;
	unsigned short tem,hum;
  uint8_t *read_buff=data_buff;
	uint8_t buff[6];


	uint32_t temp_mean,humi_mean;
	#if hardwareiic
	//开启应答
	I2C_AcknowledgeConfig(I2C1,ENABLE); 
	//start
	I2C_GenerateSTART(I2C1,ENABLE); 
	Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_MODE_SELECT,cnt);

	//写7位I2C设备地址加0作为写取位,1为读取位
	I2C_Send7bitAddress(I2C1,slave_addr<<1,I2C_Direction_Transmitter); 
	Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,cnt);
	
	I2C_SendData(I2C1,0x2C);
	Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);

	I2C_SendData(I2C1,0x06);
	Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);

	//I2C_GenerateSTOP(I2C1,ENABLE); 
	delay_ms(50);
	
	I2C_GenerateSTART(I2C1,ENABLE); 
	Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_MODE_SELECT,cnt);

	//写7位I2C设备地址加0作为写取位,1为读取位
	I2C_Send7bitAddress(I2C1,slave_addr<<1,I2C_Direction_Receiver); 
	Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED,cnt);

	for(int cnt=0;cnt<5;++cnt){
		I2C_AcknowledgeConfig(I2C1,ENABLE); 
		Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED,cnt);
		CHECK_ERROR(Error);
		//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)){;}
		buff[cnt] = I2C_ReceiveData(I2C1);
	}
	//关闭应答
	I2C_AcknowledgeConfig(I2C1,DISABLE); 
	Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED,cnt);
	buff[cnt] = I2C_ReceiveData(I2C1);
	
	//stop
	I2C_GenerateSTOP(I2C1,ENABLE); 


	//	//开启应答
	I2C_AcknowledgeConfig(I2C1,ENABLE); 
	//start
	I2C_GenerateSTART(I2C1,ENABLE); 
		
	Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_MODE_SELECT,cnt);

	//写7位I2C设备地址加0作为写取位,1为读取位
	I2C_Send7bitAddress(I2C1,slave_addr<<1,I2C_Direction_Transmitter); 
		
	Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,cnt);
		
	I2C_SendData(I2C1,0x2C);

	Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);

		
	I2C_SendData(I2C1,0x06);

	Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);

		
	//I2C_GenerateSTOP(I2C1,ENABLE); 

	I2C_GenerateSTART(I2C1,ENABLE); 

	Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_MODE_SELECT,cnt);

	//写7位I2C设备地址加0作为写取位,1为读取位
	I2C_Send7bitAddress(I2C1,slave_addr<<1,I2C_Direction_Receiver); 

	Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED,cnt);

		
		I2C_AcknowledgeConfig(I2C1,ENABLE); 
		Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED,cnt);
		CHECK_ERROR(Error);
		//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)){;}
		buff[cnt] = I2C_ReceiveData(I2C1);

		
	//关闭应答
	I2C_AcknowledgeConfig(I2C1,DISABLE); 
	Error=I2Cx_Acknowled_Check(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED,cnt);
	buff[cnt] = I2C_ReceiveData(I2C1);

	//stop
	I2C_GenerateSTOP(I2C1,ENABLE); 
			
	

	#else
		IIC_Start(sht_iic_num);
		IIC_Send_Byte(sht_iic_num,(slave_addr<<1)+0);
		Error=!IIC_Wait_Ack(sht_iic_num);
		CHECK_ERROR(Error);
		IIC_Send_Byte(sht_iic_num,0x2C);
		Error=!IIC_Wait_Ack(sht_iic_num);
		CHECK_ERROR(Error);
		IIC_Send_Byte(sht_iic_num,0x06);
		Error=!IIC_Wait_Ack(sht_iic_num);
		CHECK_ERROR(Error);
		
		IIC_Stop(sht_iic_num);
  	DelayMs(50);
	
		IIC_Start(sht_iic_num);
		IIC_Send_Byte(sht_iic_num,(slave_addr<<1)+1);
		if(IIC_Wait_Ack(sht_iic_num)==0)
		{
			for(int cnt=0;cnt<5;++cnt){
					buff[cnt] = IIC_Read_Byte(sht_iic_num,1);			
			}
		}
		buff[cnt] = IIC_Read_Byte(sht_iic_num,0);
		IIC_Stop(sht_iic_num);
	#endif
	tem = (((uint16_t)buff[0]<<8) | buff[1]);//温度拼接
	hum = (((uint16_t)buff[3]<<8) | buff[4]);//湿度拼接

	/*转换实际温度*/
	Temperature= (175.0*(float)tem/65535.0-45.0) ;// T = -45 + 175 * tem / (2^16-1)
	Humidity= (100.0*(float)hum/65535.0);// RH = hum*100 / (2^16-1)
 // //printf("Temperature: %.2fC Humidity:%.2f%%\r\n",Temperature,Humidity);
	if((Temperature>=-20)&&(Temperature<=125)&&(Humidity>=0)&&(Humidity<=100))//过滤错误数据
{
//		Temperature =(Temperature+45)*100;
//		Humidity *=100;

	  Error=1;
	}
	else{

		Error=0;
	}

	////printf("温湿度：%s\n",humiture_buff1);
	*(read_buff++)=(uint8_t)((uint16_t)Temperature>>8);
	*(read_buff++)=(uint8_t)Temperature;
	*(read_buff++)=(uint8_t)((uint16_t)Humidity>>8);
	*(read_buff++)=(uint8_t)Humidity;
	hum=0;
	tem=0;
	return Error;
}

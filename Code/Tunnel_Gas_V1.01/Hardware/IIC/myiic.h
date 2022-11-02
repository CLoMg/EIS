#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h" 

//IO��������
#define SDA_IN_1()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//PB6����ģʽ
#define SDA_OUT_1() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PB6���ģʽ
//IO��������	 
#define IIC_SCL_1    PBout(6) //SCL
#define IIC_SDA_1    PBout(7) //SDA	 
#define READ_SDA_1  PBin(7)  //����SDA 

//IO��������
#define SDA_IN_2()  {GPIOG->MODER&=~(3<<(1*2));GPIOG->MODER|=0<<1*2;}	//PB6����ģʽ
#define SDA_OUT_2() {GPIOG->MODER&=~(3<<(1*2));GPIOG->MODER|=1<<1*2;} //PB6���ģʽ
//IO��������	 
#define IIC_SCL_2    PGout(0) //SCL
#define IIC_SDA_2    PGout(1) //SDA	 
#define READ_SDA_2  PGin(1)  //����SDA 



//IIC���в�������
void IIC_Init(u8);                //��ʼ��IIC��IO��				 
void IIC_Start(u8);				//����IIC��ʼ�ź�
void IIC_Stop(u8);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8,u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(u8,unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(u8); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(u8);					//IIC����ACK�ź�
void IIC_NAck(u8);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8,u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8,u8 daddr,u8 addr);	  
//��ʱ�˳����� �������ӡ�

//I2C �����Ĵ���д��
Error_Code_t I2C_Write_Single(u8,u8 slave_addr,u8 memory_addr,u8 *write_buff);

//I2C �����Ĵ���д��
Error_Code_t I2C_Write_Multiple(u8,u8 slave_addr,u8 memory_addr,u8 *write_buff,u8 length);

//I2C �����Ĵ�����ȡ
u8 I2C_Read_Single(u8,u8 slave_addr,u8 memory_addr,u8 *read_buff);

//I2C�����Ĵ�����ȡ
u8 I2C_Read_Multiple(u8,u8 slave_addr,u8 memory_addr,u8 *read_buff,uint16_t length);

#endif

















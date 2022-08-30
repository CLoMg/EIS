#ifndef __MYIIC_H
#define __MYIIC_H


#define Error_Code_t _Bool


//IIC���в�������
void IIC_Init(unsigned char);                //��ʼ��IIC��IO��				 
void IIC_Start(unsigned char);				//����IIC��ʼ�ź�
void IIC_Stop(unsigned char);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(unsigned char,unsigned char txd);			//IIC����һ���ֽ�
unsigned char IIC_Read_Byte(unsigned char,unsigned char ack);//IIC��ȡһ���ֽ�
unsigned char IIC_Wait_Ack(unsigned char); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(unsigned char);					//IIC����ACK�ź�
void IIC_NAck(unsigned char);				//IIC������ACK�ź�

void IIC_Write_One_Byte(unsigned char,unsigned char daddr,unsigned char addr,unsigned char data);
unsigned char IIC_Read_One_Byte(unsigned char,unsigned char daddr,unsigned char addr);	  
//��ʱ�˳����� �������ӡ�

//I2C �����Ĵ���д��
Error_Code_t I2C_Write_Single(unsigned char,unsigned char slave_addr,unsigned char memory_addr,unsigned char *write_buff);

//I2C �����Ĵ���д��
Error_Code_t I2C_Write_Multiple(unsigned char,unsigned char slave_addr,unsigned char memory_addr,unsigned char *write_buff,unsigned char length);

//I2C �����Ĵ�����ȡ
unsigned char I2C_Read_Single(unsigned char,unsigned char slave_addr,unsigned char memory_addr,unsigned char *read_buff);

//I2C�����Ĵ�����ȡ
unsigned char I2C_Read_Multiple(unsigned char,unsigned char slave_addr,unsigned char memory_addr,unsigned char *read_buff,unsigned short length);

#endif

















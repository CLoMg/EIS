#ifndef __MYIIC_H
#define __MYIIC_H


#define Error_Code_t _Bool


//IIC所有操作函数
void IIC_Init(unsigned char);                //初始化IIC的IO口				 
void IIC_Start(unsigned char);				//发送IIC开始信号
void IIC_Stop(unsigned char);	  			//发送IIC停止信号
void IIC_Send_Byte(unsigned char,unsigned char txd);			//IIC发送一个字节
unsigned char IIC_Read_Byte(unsigned char,unsigned char ack);//IIC读取一个字节
unsigned char IIC_Wait_Ack(unsigned char); 				//IIC等待ACK信号
void IIC_Ack(unsigned char);					//IIC发送ACK信号
void IIC_NAck(unsigned char);				//IIC不发送ACK信号

void IIC_Write_One_Byte(unsigned char,unsigned char daddr,unsigned char addr,unsigned char data);
unsigned char IIC_Read_One_Byte(unsigned char,unsigned char daddr,unsigned char addr);	  
//超时退出函数 ，待增加。

//I2C 单个寄存器写入
Error_Code_t I2C_Write_Single(unsigned char,unsigned char slave_addr,unsigned char memory_addr,unsigned char *write_buff);

//I2C 连续寄存器写入
Error_Code_t I2C_Write_Multiple(unsigned char,unsigned char slave_addr,unsigned char memory_addr,unsigned char *write_buff,unsigned char length);

//I2C 单个寄存器读取
unsigned char I2C_Read_Single(unsigned char,unsigned char slave_addr,unsigned char memory_addr,unsigned char *read_buff);

//I2C连续寄存器读取
unsigned char I2C_Read_Multiple(unsigned char,unsigned char slave_addr,unsigned char memory_addr,unsigned char *read_buff,unsigned short length);

#endif

















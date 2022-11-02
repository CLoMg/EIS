#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h" 

//IO方向设置
#define SDA_IN_1()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//PB6输入模式
#define SDA_OUT_1() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PB6输出模式
//IO操作函数	 
#define IIC_SCL_1    PBout(6) //SCL
#define IIC_SDA_1    PBout(7) //SDA	 
#define READ_SDA_1  PBin(7)  //输入SDA 

//IO方向设置
#define SDA_IN_2()  {GPIOG->MODER&=~(3<<(1*2));GPIOG->MODER|=0<<1*2;}	//PB6输入模式
#define SDA_OUT_2() {GPIOG->MODER&=~(3<<(1*2));GPIOG->MODER|=1<<1*2;} //PB6输出模式
//IO操作函数	 
#define IIC_SCL_2    PGout(0) //SCL
#define IIC_SDA_2    PGout(1) //SDA	 
#define READ_SDA_2  PGin(1)  //输入SDA 



//IIC所有操作函数
void IIC_Init(u8);                //初始化IIC的IO口				 
void IIC_Start(u8);				//发送IIC开始信号
void IIC_Stop(u8);	  			//发送IIC停止信号
void IIC_Send_Byte(u8,u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(u8,unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(u8); 				//IIC等待ACK信号
void IIC_Ack(u8);					//IIC发送ACK信号
void IIC_NAck(u8);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8,u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8,u8 daddr,u8 addr);	  
//超时退出函数 ，待增加。

//I2C 单个寄存器写入
Error_Code_t I2C_Write_Single(u8,u8 slave_addr,u8 memory_addr,u8 *write_buff);

//I2C 连续寄存器写入
Error_Code_t I2C_Write_Multiple(u8,u8 slave_addr,u8 memory_addr,u8 *write_buff,u8 length);

//I2C 单个寄存器读取
u8 I2C_Read_Single(u8,u8 slave_addr,u8 memory_addr,u8 *read_buff);

//I2C连续寄存器读取
u8 I2C_Read_Multiple(u8,u8 slave_addr,u8 memory_addr,u8 *read_buff,uint16_t length);

#endif

















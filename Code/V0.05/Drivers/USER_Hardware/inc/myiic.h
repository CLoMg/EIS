#ifndef __MYIIC_H
#define __MYIIC_H


#define Error_Code_t _Bool

////IO方向设置
//#define SDA_IN_1()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//PB6输入模式
//#define SDA_OUT_1() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PB6输出模式

#define SDA_IN_1()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(unsigned int)8<<28;} //宏定义，PB7：上拉/下拉输入模式
#define SDA_OUT_1() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(unsigned int)3<<28;} //宏定义，PB7：50MHz推挽输出模式
//其中GPIOB->CRL&=0X0FFFFFFF;表示PB7先清零
//其中(u32)8<<28表示0000 0000 0000 0000 0000 0000 0000 1000中的1左移28位（4*7）变为1000 0000 0000 0000 0000 0000 0000 0000

////IO操作函数	 
//#define IIC_SCL_1    PBout(6) //SCL
//#define IIC_SDA_1    PBout(7) //SDA	 
//#define READ_SDA_1  PBin(7)  //输入SDA 

#define GPIO_PORT_I2C1 GPIOB
#define I2C1_SCL_PIN	 GPIO_PIN_6
#define I2C1_SDA_PIN	 GPIO_PIN_7

#define I2C1_SCL_1() GPIO_PORT_I2C1->BSRR = I2C1_SCL_PIN
#define I2C1_SCL_0() GPIO_PORT_I2C1->BSRR = (uint32_t) I2C1_SCL_PIN<< 16U

#define I2C1_SDA_1() GPIO_PORT_I2C1->BSRR = I2C1_SDA_PIN
#define I2C1_SDA_0() GPIO_PORT_I2C1->BSRR = (uint32_t) I2C1_SDA_PIN<< 16U

#define I2C1_SDA_READ() (GPIO_PORT_I2C1->IDR & I2C1_SDA_PIN)
#define I2C1_SDA_READ() (GPIO_PORT_I2C1->IDR & I2C1_SDA_PIN)

//IO方向设置
#define SDA_IN_2()  ;//{GPIOG->MODER&=~(3<<(1*2));GPIOG->MODER|=0<<1*2;}	//PB6输入模式
#define SDA_OUT_2() ;//{GPIOG->MODER&=~(3<<(1*2));GPIOG->MODER|=1<<1*2;} //PB6输出模式
//IO操作函数	 
#define IIC_SCL_2    ;//PGout(0) //SCL
#define IIC_SDA_2    ;//PGout(1) //SDA	 
#define READ_SDA_2  ;//PGin(1)  //输入SDA 


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

















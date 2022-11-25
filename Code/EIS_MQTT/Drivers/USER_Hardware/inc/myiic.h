#ifndef __MYIIC_H
#define __MYIIC_H


#define Error_Code_t _Bool

////IO��������
//#define SDA_IN_1()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//PB6����ģʽ
//#define SDA_OUT_1() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PB6���ģʽ

#define SDA_IN_1()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(unsigned int)8<<28;} //�궨�壬PB7������/��������ģʽ
#define SDA_OUT_1() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(unsigned int)3<<28;} //�궨�壬PB7��50MHz�������ģʽ
//����GPIOB->CRL&=0X0FFFFFFF;��ʾPB7������
//����(u32)8<<28��ʾ0000 0000 0000 0000 0000 0000 0000 1000�е�1����28λ��4*7����Ϊ1000 0000 0000 0000 0000 0000 0000 0000

////IO��������	 
//#define IIC_SCL_1    PBout(6) //SCL
//#define IIC_SDA_1    PBout(7) //SDA	 
//#define READ_SDA_1  PBin(7)  //����SDA 

#define GPIO_PORT_I2C1 GPIOB
#define I2C1_SCL_PIN	 GPIO_PIN_6
#define I2C1_SDA_PIN	 GPIO_PIN_7

#define I2C1_SCL_1() GPIO_PORT_I2C1->BSRR = I2C1_SCL_PIN
#define I2C1_SCL_0() GPIO_PORT_I2C1->BSRR = (uint32_t) I2C1_SCL_PIN<< 16U

#define I2C1_SDA_1() GPIO_PORT_I2C1->BSRR = I2C1_SDA_PIN
#define I2C1_SDA_0() GPIO_PORT_I2C1->BSRR = (uint32_t) I2C1_SDA_PIN<< 16U

#define I2C1_SDA_READ() (GPIO_PORT_I2C1->IDR & I2C1_SDA_PIN)
#define I2C1_SDA_READ() (GPIO_PORT_I2C1->IDR & I2C1_SDA_PIN)

//IO��������
#define SDA_IN_2()  ;//{GPIOG->MODER&=~(3<<(1*2));GPIOG->MODER|=0<<1*2;}	//PB6����ģʽ
#define SDA_OUT_2() ;//{GPIOG->MODER&=~(3<<(1*2));GPIOG->MODER|=1<<1*2;} //PB6���ģʽ
//IO��������	 
#define IIC_SCL_2    ;//PGout(0) //SCL
#define IIC_SDA_2    ;//PGout(1) //SDA	 
#define READ_SDA_2  ;//PGin(1)  //����SDA 


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

















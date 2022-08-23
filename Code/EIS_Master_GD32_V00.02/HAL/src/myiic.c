#include "myiic.h"
#include "gd32f10x_eval.h"
#include "timer.h"

//初始化IIC
void IIC_Init(uint8_t iic_num)
{		
	BSP_IIC_Init((IIC_TypeDef)iic_num);
	IIC1_SCL_HIGH();
	IIC1_SDA_HIGH();
}
//产生IIC起始信号
void IIC_Start(uint8_t iic_num)
{ 
	if(iic_num==1){
		SDA_OUT_1();     //sda线输出
		IIC1_SDA_HIGH();	  	  
		IIC1_SCL_HIGH();
		delay_us(4);
		IIC1_SDA_LOW();//START:when CLK is high,DATA change form high to low 
		delay_us(4);
		IIC1_SCL_LOW();//钳住I2C总线，准备发送或接收数据 
	}
//	else if(iic_num==2){
//		SDA_OUT_2();     //sda线输出
//		IIC_SDA_2=1;	  	  
//		IIC_SCL_2=1;
//		delay_us(4);
//		IIC_SDA_2=0;//START:when CLK is high,DATA change form high to low 
//		delay_us(4);
//		IIC_SCL_2=0;//钳住I2C总线，准备发送或接收数据 
//	}
	else;
}	  
//产生IIC停止信号
void IIC_Stop(uint8_t iic_num)
{
	if(iic_num==1){
		SDA_OUT_1();//sda线输出
		IIC1_SCL_LOW();
		IIC1_SDA_LOW();//STOP:when CLK is high DATA change form low to high
		delay_us(4);
		IIC1_SCL_HIGH(); 
		delay_us(2);
		IIC1_SDA_HIGH();//发送I2C总线结束信号
		delay_us(4);	
	}	
//	else if(iic_num==2){
//		SDA_OUT_2();//sda线输出
//		IIC_SCL_2=0;
//		IIC_SDA_2=0;//STOP:when CLK is high DATA change form low to high
//		delay_us(4);
//		IIC_SCL_2=1; 
//		delay_us(2);
//		IIC_SDA_2=1;//发送I2C总线结束信号
//		delay_us(4);	
//	}		
		else;
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(uint8_t iic_num)
{
	uint8_t ucErrTime=0;
	if(iic_num==1){
		SDA_IN_1();      //SDA设置为输入  
		IIC1_SDA_HIGH();delay_us(1);	   
		IIC1_SCL_HIGH();delay_us(1);	 
		while(READ_IIC1_SDA)
		{
			ucErrTime++;
			if(ucErrTime>250)
			{
				IIC_Stop(1);
				return 1;
			}
		}
		IIC1_SCL_LOW();//时钟输出0 
	}	
//	else if(iic_num==2){
//		SDA_IN_2();      //SDA设置为输入  
//		IIC_SDA_2=1;delay_us(1);	   
//		IIC_SCL_2=1;delay_us(1);	 
//		while(READ_SDA_2)
//		{
//			ucErrTime++;
//			if(ucErrTime>250)
//			{
//				IIC_Stop(2);
//				return 1;
//			}
//		}
//		IIC_SCL_2=0;//时钟输出0 
//	}
	else;
	return 0;  
} 
//产生ACK应答
void IIC_Ack(uint8_t iic_num)
{
	if(iic_num==1){
		IIC1_SCL_LOW();
		SDA_OUT_1();
		IIC1_SDA_LOW();
		delay_us(2);
		IIC1_SCL_HIGH();
		delay_us(2);
		IIC1_SCL_LOW();
	}
//	else if(iic_num==2){
//		IIC_SCL_2=0;
//		SDA_OUT_2();
//		IIC_SDA_2=0;
//		delay_us(2);
//		IIC_SCL_2=1;
//		delay_us(2);
//		IIC_SCL_2=0;
//	}
	else;
}
//不产生ACK应答		    
void IIC_NAck(uint8_t iic_num)
{
	if(iic_num==1){
		IIC1_SCL_LOW();
		SDA_OUT_1();
		IIC1_SDA_HIGH();
		delay_us(2);
		IIC1_SCL_HIGH();
		delay_us(2);
		IIC1_SCL_LOW();
	}
//	else if(iic_num==2){
//		IIC_SCL_2=0;
//		SDA_OUT_2();
//		IIC_SDA_2=1;
//		delay_us(2);
//		IIC_SCL_2=1;
//		delay_us(2);
//		IIC_SCL_2=0;
//	}
else;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t iic_num,uint8_t txd)
{                        
    uint8_t t;   
	if(iic_num==1){
		SDA_OUT_1(); 	    
    IIC1_SCL_LOW();//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {         
			if((txd&0x80)>>7)
				IIC1_SDA_HIGH();
			else
				IIC1_SDA_LOW();
      txd<<=1; 	  
			delay_us(2);   //对TEA5767这三个延时都是必须的
			IIC1_SCL_HIGH();
			delay_us(2); 
			IIC1_SCL_LOW();	
			delay_us(2);
    }	 
	}
//	else if(iic_num==2){
//		SDA_OUT_2(); 	    
//    IIC_SCL_2=0;//拉低时钟开始数据传输
//    for(t=0;t<8;t++)
//    {              
//        IIC_SDA_2=(txd&0x80)>>7;
//        txd<<=1; 	  
//		delay_us(2);   //对TEA5767这三个延时都是必须的
//		IIC_SCL_2=1;
//		delay_us(2); 
//		IIC_SCL_2=0;	
//		delay_us(2);
//    }	 
//	}
	else;
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(uint8_t iic_num,unsigned char ack)
{
	unsigned char i,receive=0;
	if(iic_num==1){
		SDA_IN_1();//SDA设置为输入
			for(i=0;i<8;i++ )
		{
				IIC1_SCL_LOW(); 
				delay_us(2);
				IIC1_SCL_HIGH();
				receive<<=1;
				if(READ_IIC1_SDA)receive++;   
			delay_us(1); 
			}					 
			if (!ack)
					IIC_NAck(1);//发送nACK
			else
					IIC_Ack(1); //发送ACK  
	}
//	else if(iic_num==2){
//		SDA_IN_2();//SDA设置为输入
//			for(i=0;i<8;i++ )
//		{
//				IIC_SCL_2=0; 
//				delay_us(2);
//				IIC_SCL_2=1;
//				receive<<=1;
//				if(READ_SDA_2)receive++;   
//			delay_us(1); 
//			}					 
//			if (!ack)
//					IIC_NAck(2);//发送nACK
//			else
//					IIC_Ack(2); //发送ACK  
//	}
    return receive;
}

//超时退出函数 ，待增加。
//I2C 单个寄存器写入
Error_Code_t I2C_Write_Single(uint8_t iic_num,uint8_t slave_addr,uint8_t memory_addr,uint8_t *write_buff)
{
  Error_Code_t Error=0;
 

	IIC_Start(iic_num);

	IIC_Send_Byte(iic_num,slave_addr+0);
	Error = !IIC_Wait_Ack(iic_num);

	IIC_Send_Byte(iic_num,memory_addr);
	Error = !IIC_Wait_Ack(iic_num);

	IIC_Send_Byte(iic_num,*write_buff);
	Error = !IIC_Wait_Ack(iic_num);

	IIC_Stop(iic_num);
	return Error;
}

//I2C 连续寄存器写入
Error_Code_t I2C_Write_Multiple(uint8_t iic_num,uint8_t slave_addr,uint8_t memory_addr,uint8_t *write_buff,uint8_t length)
{
	Error_Code_t Error=0;

	uint8_t *pData = write_buff;

	
  IIC_Start(iic_num);

	IIC_Send_Byte(iic_num,slave_addr+0);
	Error = !IIC_Wait_Ack(iic_num);

	IIC_Send_Byte(iic_num,memory_addr);
	Error = !IIC_Wait_Ack(iic_num);

  for(uint8_t cnt=0;cnt<(length-1);++cnt)
	{	

		IIC_Send_Byte(iic_num,*pData);
		Error = !IIC_Wait_Ack(iic_num);
		CHECK_ERROR(Error);
		pData++;
	}

	IIC_Send_Byte(iic_num,*pData);
	Error = !IIC_Wait_Ack(iic_num);

	IIC_Stop(iic_num);
	return Error;
}

//I2C 单个寄存器读取
uint8_t I2C_Read_Single(uint8_t iic_num,uint8_t slave_addr,uint8_t memory_addr,uint8_t *read_buff){
	uint8_t *Data;
	Error_Code_t Error=0;
	
	Data = read_buff;

	IIC_Start(iic_num);

	IIC_Send_Byte(iic_num,slave_addr+0);
	Error = !IIC_Wait_Ack(iic_num);

	IIC_Send_Byte(iic_num,memory_addr);
	Error = !IIC_Wait_Ack(iic_num);
	
//	IIC_Stop();
//	delay_us(50);

  IIC_Start(iic_num);
	//Error = !IIC_Wait_Ack();

	IIC_Send_Byte(iic_num,slave_addr+1);
	Error = !IIC_Wait_Ack(iic_num);

	*Data = IIC_Read_Byte(iic_num,0);

	IIC_Stop(iic_num);
	return Error;
}

//I2C连续寄存器读取
uint8_t I2C_Read_Multiple(uint8_t iic_num,uint8_t slave_addr,uint8_t memory_addr,uint8_t *read_buff,uint16_t length){
	uint8_t *pData;
	Error_Code_t Error=0;

	pData=read_buff;
	
	IIC_Start(iic_num);

	IIC_Send_Byte(iic_num,slave_addr+0);
	Error = !IIC_Wait_Ack(iic_num);

	IIC_Send_Byte(iic_num,memory_addr);
	Error = !IIC_Wait_Ack(iic_num);
  
	IIC_Start(iic_num);

	IIC_Send_Byte(iic_num,slave_addr+1);
	Error = !IIC_Wait_Ack(iic_num);

	for(int cnt=0;cnt<(length-1);++cnt){
		*pData = IIC_Read_Byte(iic_num,1);
		 pData++;
	}
	
	*pData = IIC_Read_Byte(iic_num,0);
	IIC_Stop(iic_num);

	return Error;
}





























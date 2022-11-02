#include "myiic.h"
#include "delay.h"

//初始化IIC
void IIC_Init(uint8_t iic_num)
{			
	if(iic_num==1){
		GPIO_InitTypeDef  GPIO_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟

		//GPIOB8,B9初始化设置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
		GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
		IIC_SCL_1=1;
		IIC_SDA_1=1;
	}
	else if(iic_num==2){
		GPIO_InitTypeDef  GPIO_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//使能GPIOB时钟

		//GPIOB8,B9初始化设置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
		GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化
		IIC_SCL_2=1;
		IIC_SDA_2=1;
	}
	else{}
}
//产生IIC起始信号
void IIC_Start(uint8_t iic_num)
{ 
	if(iic_num==1){
		SDA_OUT_1();     //sda线输出
		IIC_SDA_1=1;	  	  
		IIC_SCL_1=1;
		delay_us(4);
		IIC_SDA_1=0;//START:when CLK is high,DATA change form high to low 
		delay_us(4);
		IIC_SCL_1=0;//钳住I2C总线，准备发送或接收数据 
	}
	else if(iic_num==2){
		SDA_OUT_2();     //sda线输出
		IIC_SDA_2=1;	  	  
		IIC_SCL_2=1;
		delay_us(4);
		IIC_SDA_2=0;//START:when CLK is high,DATA change form high to low 
		delay_us(4);
		IIC_SCL_2=0;//钳住I2C总线，准备发送或接收数据 
	}
	else;
}	  
//产生IIC停止信号
void IIC_Stop(uint8_t iic_num)
{
	if(iic_num==1){
		SDA_OUT_1();//sda线输出
		IIC_SCL_1=0;
		IIC_SDA_1=0;//STOP:when CLK is high DATA change form low to high
		delay_us(4);
		IIC_SCL_1=1; 
		delay_us(2);
		IIC_SDA_1=1;//发送I2C总线结束信号
		delay_us(4);	
	}	
	else if(iic_num==2){
		SDA_OUT_2();//sda线输出
		IIC_SCL_2=0;
		IIC_SDA_2=0;//STOP:when CLK is high DATA change form low to high
		delay_us(4);
		IIC_SCL_2=1; 
		delay_us(2);
		IIC_SDA_2=1;//发送I2C总线结束信号
		delay_us(4);	
	}		
		else;
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(uint8_t iic_num)
{
	u8 ucErrTime=0;
	if(iic_num==1){
		SDA_IN_1();      //SDA设置为输入  
		IIC_SDA_1=1;delay_us(1);	   
		IIC_SCL_1=1;delay_us(1);	 
		while(READ_SDA_1)
		{
			ucErrTime++;
			if(ucErrTime>250)
			{
				IIC_Stop(1);
				return 1;
			}
		}
		IIC_SCL_1=0;//时钟输出0 
	}	
	else if(iic_num==2){
		SDA_IN_2();      //SDA设置为输入  
		IIC_SDA_2=1;delay_us(1);	   
		IIC_SCL_2=1;delay_us(1);	 
		while(READ_SDA_2)
		{
			ucErrTime++;
			if(ucErrTime>250)
			{
				IIC_Stop(2);
				return 1;
			}
		}
		IIC_SCL_2=0;//时钟输出0 
	}
	else;
	return 0;  
} 
//产生ACK应答
void IIC_Ack(uint8_t iic_num)
{
	if(iic_num==1){
		IIC_SCL_1=0;
		SDA_OUT_1();
		IIC_SDA_1=0;
		delay_us(2);
		IIC_SCL_1=1;
		delay_us(2);
		IIC_SCL_1=0;
	}
	else if(iic_num==2){
		IIC_SCL_2=0;
		SDA_OUT_2();
		IIC_SDA_2=0;
		delay_us(2);
		IIC_SCL_2=1;
		delay_us(2);
		IIC_SCL_2=0;
	}
	else;
}
//不产生ACK应答		    
void IIC_NAck(uint8_t iic_num)
{
	if(iic_num==1){
		IIC_SCL_1=0;
		SDA_OUT_1();
		IIC_SDA_1=1;
		delay_us(2);
		IIC_SCL_1=1;
		delay_us(2);
		IIC_SCL_1=0;
	}
	else if(iic_num==2){
		IIC_SCL_2=0;
		SDA_OUT_2();
		IIC_SDA_2=1;
		delay_us(2);
		IIC_SCL_2=1;
		delay_us(2);
		IIC_SCL_2=0;
	}
else;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 iic_num,u8 txd)
{                        
    u8 t;   
	if(iic_num==1){
		SDA_OUT_1(); 	    
    IIC_SCL_1=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA_1=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL_1=1;
		delay_us(2); 
		IIC_SCL_1=0;	
		delay_us(2);
    }	 
	}
	else if(iic_num==2){
		SDA_OUT_2(); 	    
    IIC_SCL_2=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA_2=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL_2=1;
		delay_us(2); 
		IIC_SCL_2=0;	
		delay_us(2);
    }	 
	}
	else;
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(u8 iic_num,unsigned char ack)
{
	unsigned char i,receive=0;
	if(iic_num==1){
		SDA_IN_1();//SDA设置为输入
			for(i=0;i<8;i++ )
		{
				IIC_SCL_1=0; 
				delay_us(2);
				IIC_SCL_1=1;
				receive<<=1;
				if(READ_SDA_1)receive++;   
			delay_us(1); 
			}					 
			if (!ack)
					IIC_NAck(1);//发送nACK
			else
					IIC_Ack(1); //发送ACK  
	}
	else if(iic_num==2){
		SDA_IN_2();//SDA设置为输入
			for(i=0;i<8;i++ )
		{
				IIC_SCL_2=0; 
				delay_us(2);
				IIC_SCL_2=1;
				receive<<=1;
				if(READ_SDA_2)receive++;   
			delay_us(1); 
			}					 
			if (!ack)
					IIC_NAck(2);//发送nACK
			else
					IIC_Ack(2); //发送ACK  
	}
    return receive;
}

//超时退出函数 ，待增加。
//I2C 单个寄存器写入
Error_Code_t I2C_Write_Single(u8 iic_num,u8 slave_addr,u8 memory_addr,u8 *write_buff)
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
Error_Code_t I2C_Write_Multiple(u8 iic_num,u8 slave_addr,u8 memory_addr,u8 *write_buff,u8 length)
{
	Error_Code_t Error=0;

	u8 *pData = write_buff;

	
   IIC_Start(iic_num);

	IIC_Send_Byte(iic_num,slave_addr+0);
	Error = !IIC_Wait_Ack(iic_num);

	IIC_Send_Byte(iic_num,memory_addr);
	Error = !IIC_Wait_Ack(iic_num);

  for(u8 cnt=0;cnt<(length-1);++cnt)
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
u8 I2C_Read_Single(u8 iic_num,u8 slave_addr,u8 memory_addr,u8 *read_buff){
	u8 *Data;
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
u8 I2C_Read_Multiple(u8 iic_num,u8 slave_addr,u8 memory_addr,u8 *read_buff,uint16_t length){
	u8 *pData;
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





























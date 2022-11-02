#include "i2c.h"
uint16_t cnt=65535;
//I2C1 GPIO初始化
static void I2Cx_GPIO_Config(I2C_TypeDef* I2Cx){

	if(I2Cx==I2C1){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB总线时钟
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_I2C1);//配置管脚为复用模式
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_I2C1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOB6与GPIOB7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; //开漏模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //无上下拉
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
	else if(I2Cx==I2C2){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOB总线时钟
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource0,GPIO_AF_I2C2);//配置管脚为复用模式
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource1,GPIO_AF_I2C2);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //GPIOB6与GPIOB7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; //开漏模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //无上下拉
	
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	}
}

 
//I2C1 初始化
void I2Cx_Init(I2C_TypeDef* I2Cx){
	
	I2Cx_GPIO_Config(I2Cx);
	I2C_InitTypeDef I2C_InitStructure;
	
	if(I2Cx==I2C1){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		I2C_DeInit(I2C1);
		I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; //定义I2C模式
		I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;// I2C fast mode Tlow/Thigh = 2
		I2C_InitStructure.I2C_OwnAddress1 = 0x77;
		I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //7位地址
		I2C_InitStructure.I2C_ClockSpeed = 10000; //I2C总线时钟频率
		I2C_Init(I2C1, &I2C_InitStructure);
		I2C_Cmd(I2C1, ENABLE);
		
		I2C_AcknowledgeConfig(I2C1,ENABLE);//开启应答
	}
	else if(I2Cx==I2C2){

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
		I2C_DeInit(I2C2);
		I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; //定义I2C模式
		I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;// I2C fast mode Tlow/Thigh = 2
		I2C_InitStructure.I2C_OwnAddress1 = 0x78;
		I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //7位地址
		I2C_InitStructure.I2C_ClockSpeed = 10000; //I2C总线时钟频率
		I2C_Init(I2C2, &I2C_InitStructure);
		I2C_Cmd(I2C2, ENABLE);
		
		I2C_AcknowledgeConfig(I2C2,ENABLE);//开启应答
	}
}

//超时退出函数 ，待增加。
//I2C 单个寄存器写入
Error_Code_t I2Cx_Write_Single(I2C_TypeDef* I2Cx,u8 slave_addr,u8 memory_addr,u8 *write_buff)
{
  Error_Code_t Error=0;

	I2C_AcknowledgeConfig(I2Cx,ENABLE); 
	//start信号
	I2C_GenerateSTART(I2Cx,ENABLE); 
	//while(!(Error=I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))&&cnt--){;}//EV5
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_MODE_SELECT,cnt);
  //从机地址+写 信号
	I2C_Send7bitAddress(I2Cx,slave_addr,I2C_Direction_Transmitter); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,cnt);
	//while(!(Error=I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&cnt--){;} //EV6
  //寄存器地址
	I2C_SendData(I2Cx,memory_addr); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);
	//while(!(Error=I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&cnt--){;} //EV8
  //写入数据
	I2C_SendData(I2Cx,*write_buff);
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);
	//while(!(Error=I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&cnt--){;} 
 //stop信号
	I2C_GenerateSTOP(I2Cx,ENABLE); 
	return Error;
}

//I2C 连续寄存器写入
Error_Code_t I2Cx_Write_Multiple(I2C_TypeDef* I2Cx,u8 slave_addr,u8 memory_addr,u8 *write_buff,u8 length)
{
	Error_Code_t Error=0;

	u8 *pData = write_buff;
	//开启应答
	I2C_AcknowledgeConfig(I2Cx,ENABLE); 
	//start
	I2C_GenerateSTART(I2Cx,ENABLE); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_MODE_SELECT,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;}//EV5
  //从机地址+写
	I2C_Send7bitAddress(I2Cx,slave_addr,I2C_Direction_Transmitter); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){;} //EV6
  //memory起始地址
	I2C_SendData(I2Cx,memory_addr); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} //EV8
	//写入前（length-1）个bytes
  for(u8 cnt=0;cnt<(length-1);++cnt)
	{	
		I2C_SendData(I2Cx,*pData);
		Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);
		CHECK_ERROR(Error);
		//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} 
		pData++;
	}
	//写入最后一个byte
	I2C_SendData(I2Cx,*pData);
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} 
	//stop	
	I2C_GenerateSTOP(I2Cx,ENABLE); 
	return Error;
}

//I2C 单个寄存器读取
u8 I2Cx_Read_Single(I2C_TypeDef* I2Cx,u8 slave_addr,u8 memory_addr,u8 *read_buff){
	u8 *Data;
	Error_Code_t Error=0;
	
	Data = read_buff;
	//开启应答
	I2C_AcknowledgeConfig(I2Cx,ENABLE); 
	//start
	I2C_GenerateSTART(I2Cx,ENABLE); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_MODE_SELECT,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;} 
  //从机地址+写
	I2C_Send7bitAddress(I2Cx,slave_addr,I2C_Direction_Transmitter); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){;}
  //待读取memory地址
	I2C_SendData(I2Cx,memory_addr);
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} 
  //restart
	I2C_GenerateSTART(I2Cx,ENABLE);
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_MODE_SELECT,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;} 
  //从机地址+读
	I2C_Send7bitAddress(I2Cx,slave_addr,I2C_Direction_Receiver); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){;} 
  //关闭应答
	I2C_AcknowledgeConfig(I2Cx,DISABLE); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)){;} 
	*Data = I2C_ReceiveData(I2Cx);//读取寄存器数据
	//stop	
	I2C_GenerateSTOP(I2Cx,ENABLE); 
	
	return Error;
}

//I2C连续寄存器读取
u8 I2Cx_Read_Multiple(I2C_TypeDef* I2Cx,u8 slave_addr,u8 memory_addr,u8 *read_buff,uint16_t length){
	u8 *pData;
	Error_Code_t Error=0;

	pData=read_buff;
	//开启应答
	I2C_AcknowledgeConfig(I2Cx,ENABLE); 
	//start
	I2C_GenerateSTART(I2Cx,ENABLE); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_MODE_SELECT,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;} 
  //从机地址+写
	I2C_Send7bitAddress(I2Cx,slave_addr,I2C_Direction_Transmitter); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){;}
	//memory起始地址
	I2C_SendData(I2Cx,memory_addr);
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} 
	//restart
	I2C_GenerateSTART(I2Cx,ENABLE); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_MODE_SELECT,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;} 
	//从机地址+读
	I2C_Send7bitAddress(I2Cx,slave_addr,I2C_Direction_Receiver); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){;} 
	//读取前（length-1）个bytes数据
	for(int cnt=0;cnt<(length-1);++cnt){
		I2C_AcknowledgeConfig(I2Cx,ENABLE); 
		Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED,cnt);
		CHECK_ERROR(Error);
		//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)){;}
		*pData = I2C_ReceiveData(I2Cx);
		 pData++;
	}
	//关闭应答
	I2C_AcknowledgeConfig(I2Cx,DISABLE); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)){;}
	//读取最后1个byte数据
	*pData = I2C_ReceiveData(I2Cx);
	//stop
	I2C_GenerateSTOP(I2Cx,ENABLE); 

	return Error;
}


//待修改，超时退出函数
Error_Code_t I2Cx_Acknowled_Check(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT,uint16_t Count){
	Error_Code_t Error=0;
	Count=0;
	while((!(Error=I2C_CheckEvent(I2Cx,I2C_EVENT)))&&(Count<65535)){
		Count++;
	};
	return Error;
}

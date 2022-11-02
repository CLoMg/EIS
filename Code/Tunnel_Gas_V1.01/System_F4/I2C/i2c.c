#include "i2c.h"
uint16_t cnt=65535;
//I2C1 GPIO��ʼ��
static void I2Cx_GPIO_Config(I2C_TypeDef* I2Cx){

	if(I2Cx==I2C1){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOB����ʱ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_I2C1);//���ùܽ�Ϊ����ģʽ
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_I2C1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //GPIOB6��GPIOB7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; //��©ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //��������
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
	else if(I2Cx==I2C2){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOB����ʱ��
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource0,GPIO_AF_I2C2);//���ùܽ�Ϊ����ģʽ
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource1,GPIO_AF_I2C2);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //GPIOB6��GPIOB7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; //��©ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //��������
	
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	}
}

 
//I2C1 ��ʼ��
void I2Cx_Init(I2C_TypeDef* I2Cx){
	
	I2Cx_GPIO_Config(I2Cx);
	I2C_InitTypeDef I2C_InitStructure;
	
	if(I2Cx==I2C1){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		I2C_DeInit(I2C1);
		I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; //����I2Cģʽ
		I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;// I2C fast mode Tlow/Thigh = 2
		I2C_InitStructure.I2C_OwnAddress1 = 0x77;
		I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //7λ��ַ
		I2C_InitStructure.I2C_ClockSpeed = 10000; //I2C����ʱ��Ƶ��
		I2C_Init(I2C1, &I2C_InitStructure);
		I2C_Cmd(I2C1, ENABLE);
		
		I2C_AcknowledgeConfig(I2C1,ENABLE);//����Ӧ��
	}
	else if(I2Cx==I2C2){

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
		I2C_DeInit(I2C2);
		I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; //����I2Cģʽ
		I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;// I2C fast mode Tlow/Thigh = 2
		I2C_InitStructure.I2C_OwnAddress1 = 0x78;
		I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //7λ��ַ
		I2C_InitStructure.I2C_ClockSpeed = 10000; //I2C����ʱ��Ƶ��
		I2C_Init(I2C2, &I2C_InitStructure);
		I2C_Cmd(I2C2, ENABLE);
		
		I2C_AcknowledgeConfig(I2C2,ENABLE);//����Ӧ��
	}
}

//��ʱ�˳����� �������ӡ�
//I2C �����Ĵ���д��
Error_Code_t I2Cx_Write_Single(I2C_TypeDef* I2Cx,u8 slave_addr,u8 memory_addr,u8 *write_buff)
{
  Error_Code_t Error=0;

	I2C_AcknowledgeConfig(I2Cx,ENABLE); 
	//start�ź�
	I2C_GenerateSTART(I2Cx,ENABLE); 
	//while(!(Error=I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT))&&cnt--){;}//EV5
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_MODE_SELECT,cnt);
  //�ӻ���ַ+д �ź�
	I2C_Send7bitAddress(I2Cx,slave_addr,I2C_Direction_Transmitter); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,cnt);
	//while(!(Error=I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&cnt--){;} //EV6
  //�Ĵ�����ַ
	I2C_SendData(I2Cx,memory_addr); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);
	//while(!(Error=I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&cnt--){;} //EV8
  //д������
	I2C_SendData(I2Cx,*write_buff);
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);
	//while(!(Error=I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&cnt--){;} 
 //stop�ź�
	I2C_GenerateSTOP(I2Cx,ENABLE); 
	return Error;
}

//I2C �����Ĵ���д��
Error_Code_t I2Cx_Write_Multiple(I2C_TypeDef* I2Cx,u8 slave_addr,u8 memory_addr,u8 *write_buff,u8 length)
{
	Error_Code_t Error=0;

	u8 *pData = write_buff;
	//����Ӧ��
	I2C_AcknowledgeConfig(I2Cx,ENABLE); 
	//start
	I2C_GenerateSTART(I2Cx,ENABLE); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_MODE_SELECT,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;}//EV5
  //�ӻ���ַ+д
	I2C_Send7bitAddress(I2Cx,slave_addr,I2C_Direction_Transmitter); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){;} //EV6
  //memory��ʼ��ַ
	I2C_SendData(I2Cx,memory_addr); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} //EV8
	//д��ǰ��length-1����bytes
  for(u8 cnt=0;cnt<(length-1);++cnt)
	{	
		I2C_SendData(I2Cx,*pData);
		Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);
		CHECK_ERROR(Error);
		//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} 
		pData++;
	}
	//д�����һ��byte
	I2C_SendData(I2Cx,*pData);
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} 
	//stop	
	I2C_GenerateSTOP(I2Cx,ENABLE); 
	return Error;
}

//I2C �����Ĵ�����ȡ
u8 I2Cx_Read_Single(I2C_TypeDef* I2Cx,u8 slave_addr,u8 memory_addr,u8 *read_buff){
	u8 *Data;
	Error_Code_t Error=0;
	
	Data = read_buff;
	//����Ӧ��
	I2C_AcknowledgeConfig(I2Cx,ENABLE); 
	//start
	I2C_GenerateSTART(I2Cx,ENABLE); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_MODE_SELECT,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;} 
  //�ӻ���ַ+д
	I2C_Send7bitAddress(I2Cx,slave_addr,I2C_Direction_Transmitter); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){;}
  //����ȡmemory��ַ
	I2C_SendData(I2Cx,memory_addr);
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} 
  //restart
	I2C_GenerateSTART(I2Cx,ENABLE);
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_MODE_SELECT,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;} 
  //�ӻ���ַ+��
	I2C_Send7bitAddress(I2Cx,slave_addr,I2C_Direction_Receiver); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){;} 
  //�ر�Ӧ��
	I2C_AcknowledgeConfig(I2Cx,DISABLE); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)){;} 
	*Data = I2C_ReceiveData(I2Cx);//��ȡ�Ĵ�������
	//stop	
	I2C_GenerateSTOP(I2Cx,ENABLE); 
	
	return Error;
}

//I2C�����Ĵ�����ȡ
u8 I2Cx_Read_Multiple(I2C_TypeDef* I2Cx,u8 slave_addr,u8 memory_addr,u8 *read_buff,uint16_t length){
	u8 *pData;
	Error_Code_t Error=0;

	pData=read_buff;
	//����Ӧ��
	I2C_AcknowledgeConfig(I2Cx,ENABLE); 
	//start
	I2C_GenerateSTART(I2Cx,ENABLE); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_MODE_SELECT,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;} 
  //�ӻ���ַ+д
	I2C_Send7bitAddress(I2Cx,slave_addr,I2C_Direction_Transmitter); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){;}
	//memory��ʼ��ַ
	I2C_SendData(I2Cx,memory_addr);
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){;} 
	//restart
	I2C_GenerateSTART(I2Cx,ENABLE); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_MODE_SELECT,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){;} 
	//�ӻ���ַ+��
	I2C_Send7bitAddress(I2Cx,slave_addr,I2C_Direction_Receiver); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){;} 
	//��ȡǰ��length-1����bytes����
	for(int cnt=0;cnt<(length-1);++cnt){
		I2C_AcknowledgeConfig(I2Cx,ENABLE); 
		Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED,cnt);
		CHECK_ERROR(Error);
		//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)){;}
		*pData = I2C_ReceiveData(I2Cx);
		 pData++;
	}
	//�ر�Ӧ��
	I2C_AcknowledgeConfig(I2Cx,DISABLE); 
	Error=I2Cx_Acknowled_Check(I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED,cnt);
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED)){;}
	//��ȡ���1��byte����
	*pData = I2C_ReceiveData(I2Cx);
	//stop
	I2C_GenerateSTOP(I2Cx,ENABLE); 

	return Error;
}


//���޸ģ���ʱ�˳�����
Error_Code_t I2Cx_Acknowled_Check(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT,uint16_t Count){
	Error_Code_t Error=0;
	Count=0;
	while((!(Error=I2C_CheckEvent(I2Cx,I2C_EVENT)))&&(Count<65535)){
		Count++;
	};
	return Error;
}

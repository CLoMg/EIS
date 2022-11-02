#ifndef __STMFLASH_H__
#define __STMFLASH_H__


//////////////////////////////////////////////////////////////////////////////////////////////////////
//用户根据自己的需要设置
#define STM32_FLASH_SIZE 	64 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 	1              	//使能FLASH写入(0，不是能;1，使能)
#define FLASH_WAITETIME  	5000          	//FLASH等待超时时间

extern void    FLASH_PageErase(unsigned int PageAddress);

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 		//STM32 FLASH的起始地址

unsigned char STMFLASH_GetStatus(void);				  //获得状态
unsigned char STMFLASH_WaitDone(unsigned short time);				  //等待操作结束
unsigned char STMFLASH_ErasePage(unsigned int paddr);			  //擦除页
unsigned char STMFLASH_WriteHalfWord(unsigned int faddr, unsigned short dat);//写入半字
unsigned short STMFLASH_ReadHalfWord(unsigned int faddr);		  //读出半字  
void STMFLASH_WriteLenByte(unsigned int WriteAddr,unsigned int DataToWrite,unsigned short Len);	//指定地址开始写入指定长度的数据
unsigned int STMFLASH_ReadLenByte(unsigned int ReadAddr,unsigned short Len);						//指定地址开始读取指定长度数据
void STMFLASH_Write(unsigned int WriteAddr,unsigned short *pBuffer,unsigned short NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(unsigned int ReadAddr,unsigned short *pBuffer,unsigned short NumToRead);   		//从指定地址开始读出指定长度的数据

//测试写入
void Test_Write(unsigned int WriteAddr,unsigned short WriteData);								   
#endif


















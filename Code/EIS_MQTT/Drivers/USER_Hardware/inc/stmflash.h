#ifndef __STMFLASH_H__
#define __STMFLASH_H__


//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 	64 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 	1              	//ʹ��FLASHд��(0��������;1��ʹ��)
#define FLASH_WAITETIME  	5000          	//FLASH�ȴ���ʱʱ��

extern void    FLASH_PageErase(unsigned int PageAddress);

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 		//STM32 FLASH����ʼ��ַ

unsigned char STMFLASH_GetStatus(void);				  //���״̬
unsigned char STMFLASH_WaitDone(unsigned short time);				  //�ȴ���������
unsigned char STMFLASH_ErasePage(unsigned int paddr);			  //����ҳ
unsigned char STMFLASH_WriteHalfWord(unsigned int faddr, unsigned short dat);//д�����
unsigned short STMFLASH_ReadHalfWord(unsigned int faddr);		  //��������  
void STMFLASH_WriteLenByte(unsigned int WriteAddr,unsigned int DataToWrite,unsigned short Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
unsigned int STMFLASH_ReadLenByte(unsigned int ReadAddr,unsigned short Len);						//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(unsigned int WriteAddr,unsigned short *pBuffer,unsigned short NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(unsigned int ReadAddr,unsigned short *pBuffer,unsigned short NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

//����д��
void Test_Write(unsigned int WriteAddr,unsigned short WriteData);								   
#endif


















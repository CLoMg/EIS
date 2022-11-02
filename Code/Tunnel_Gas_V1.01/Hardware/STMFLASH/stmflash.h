#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  
#include "stm32f4xx_conf.h"



//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ

#define User_Parameter_BASE 0x080E0000 	//�û���������ʼ��ַ
#define Node_firmware_bak_addr      		0x080A0000  //128k
#define flash_important_mes_addr User_Parameter_BASE+0
#define Data_Update_Frequence User_Parameter_BASE+8
#define Server_Addr_Memory    User_Parameter_BASE+12
#define device_id_addr  User_Parameter_BASE+20
#define terminal_mac_addr User_Parameter_BASE+28 // �������������ն˵�mac��ַ һ��8*9=72���ֽ�(0x080E001C~0x080E004C) 
#define Group_LORA_Speed_Grade User_Parameter_BASE+100 // �豸�������Lora ���ʵȼ����ò��� ռ��1��32λ��ַ
#define Group_LORA_Channel     User_Parameter_BASE+104 //�豸�������LORA�ŵ����ò��� ռ��1��32λ��ַ

#define Upgrade_Parameter_BASE					  0x080C0000//�����ڶ�ҳ��ַ����128k
#define gateway_upgrade_flag_addr      				  Upgrade_Parameter_BASE+0      //0
#define gateway_version_now_addr     						Upgrade_Parameter_BASE+0x04   //1
#define gateway_version_new_addr     						Upgrade_Parameter_BASE+0x08   //2
#define gatway_total_byte_nums_addr     				Upgrade_Parameter_BASE+0x0c   //3
#define gatway_reve_byte_nums_addr      				Upgrade_Parameter_BASE+0x10   //4
#define addr_gatway_total_package_nums      		Upgrade_Parameter_BASE+0x14   //5
#define addr_gatway_rec_package_nums      		  Upgrade_Parameter_BASE+0x18   //6

#define node_upgrade_flag_addr      						Upgrade_Parameter_BASE+0x20   //8
#define node_version_now_addr      							Upgrade_Parameter_BASE+0x24   //9
#define node_version_new_addr      							Upgrade_Parameter_BASE+0x28   //10
#define node_total_byte_nums_addr     					Upgrade_Parameter_BASE+0x2c   //11
#define node_reve_byte_nums_addr      					Upgrade_Parameter_BASE+0x30   //12
#define node_total_package_nums_addr            Upgrade_Parameter_BASE+0x34   //13
#define node_rec_package_nums_addr              Upgrade_Parameter_BASE+0x38   //14

#define send_to_node_upgrade_status_addr     	Upgrade_Parameter_BASE+0x40   //16
#define send_to_node_upgrade_cnt1_addr      	Upgrade_Parameter_BASE+0x44   //17
#define send_to_node_upgrade_cnt2_addr     		Upgrade_Parameter_BASE+0x48   //18

#define send_to_node_package_nums1_addr      	Upgrade_Parameter_BASE+0x4c   //19
#define send_to_node_package_nums2_addr      	Upgrade_Parameter_BASE=0x50   //20
#define send_to_node_package_nums3_addr      	Upgrade_Parameter_BASE=0x54   //21
#define send_to_node_package_nums4_addr      	Upgrade_Parameter_BASE+0x58   //22
#define send_to_node_package_nums5_addr      	Upgrade_Parameter_BASE+0x5c   //23

#define addr_paltform_to_gateway_serial       Upgrade_Parameter_BASE+0x68   //26
#define addr_gateway_to_node_serial           Upgrade_Parameter_BASE+0x6C   //27

#define total_node_dev_nums_addr     			Upgrade_Parameter_BASE+0x80   //32
#define node1_addr1      									Upgrade_Parameter_BASE+0x84   //33
#define node1_addr2      									Upgrade_Parameter_BASE+0x88   //34
#define node2_addr1      									Upgrade_Parameter_BASE+0x8c   //35
#define node2_addr2      									Upgrade_Parameter_BASE+0x90   //36
#define node3_addr1      									Upgrade_Parameter_BASE+0x94   //37
#define node3_addr2      									Upgrade_Parameter_BASE+0x98   //38
#define node4_addr1      									Upgrade_Parameter_BASE+0x9c   //39
#define node4_addr2      									Upgrade_Parameter_BASE+0xA0   //40
#define node5_addr1      									Upgrade_Parameter_BASE+0xA4   //41
#define node5_addr2      									Upgrade_Parameter_BASE+0xA8   //42

 

//FLASH ��������ʼ��ַ
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//����0��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//����1��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//����2��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//����3��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//����4��ʼ��ַ, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//����5��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//����6��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//����7��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//����8��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//����9��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//����10��ʼ��ַ,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//����11��ʼ��ַ,128 Kbytes  

u32 STMFLASH_ReadWord(u32 faddr);		  	//������  
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
						   
#endif


















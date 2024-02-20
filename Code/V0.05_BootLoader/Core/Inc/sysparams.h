#ifndef _SYSPARAMS_H_
#define _SYSPARAMS_H_

/*Flash��������*/
/*
0x0800 0000 ~ 0x080023FF : BootLoader
0x0800 2400 ~ 0x080027FF : SystemParameters
0x0800 2800 ~ 0x080093FF : Appliction_Code Area
0x0800 9400 ~ 0x0800FFFF : Backup Area
*/


#define SysParamAddr_Base 0x08002400
#define AppRunAddr_Base 0x08002800
#define AppArea_Size 0x00006C00
#define BackUpAddr_Base 0x08009400

#define FWPack_MaxSize 0x80
#define UNIQUE_ID_ADDR 0x1FFFF7E8

//
/*����ϵͳ���� Flash�洢��ַ*/
#define DevLoc_FlashAddr             SysParamAddr_Base + 0x00 //
#define DevUpLoadPer_FlashAddr       SysParamAddr_Base + 0x04 //
#define DevGroupInfo_FlashAddr       SysParamAddr_Base + 0x08
#define DevLightInfo_FlashAddr       SysParamAddr_Base + 0x0c
#define OTAStatus_FlashAddr					 SysParamAddr_Base + 0x10
#define OTAFWSize_FlashAddr          SysParamAddr_Base + 0x14
#define OTAFWVER_FlashAddr 					 SysParamAddr_Base + 0x18
#define OTAFWRecSN_FlashAddr 				 SysParamAddr_Base + 0x1C

#define ITEM_ID 0x0F
extern unsigned short Group_Info[2];
extern unsigned short DEV_LOC[2];
extern unsigned short Upload_Period;
extern char mac_strings[20];
extern unsigned char unique_id[8];
typedef enum {
	Failed = 0x00, Success = 0x01, Invalid_Para = 0x03,
}Re_Code_T;


/*�豸λ����Ϣ��ʼ��*/
void Dev_Location_Init(void);
/*�豸λ����Ϣ�޸�*/
Re_Code_T Dev_Location_Change(unsigned short *devloc_new);
/*�豸λ����Ϣ��ȡ*/
Re_Code_T Dev_Loc_Read(int*dev_loc);
/*�������ܣ�	�豸������Ϣ��ʼ��*/
void Dev_GroupInfo_Init(void);
/*�������ܣ�	�豸������Ϣ�޸�*/
Re_Code_T Dev_GroupInfo_Change(unsigned short *groupinfo_new);
/*�豸�ϴ����ڳ�ʼ��*/
void Dev_UpLoad_Per_Init(void);
/*�豸�ϴ������޸�*/
Re_Code_T Dev_UpLoad_Per_Change(unsigned short upload_per_new);
/*�豸�ϴ����ڶ�ȡ*/
Re_Code_T Dev_UpLoad_Per_Read(unsigned short *upload_per);
Re_Code_T Dev_FwVer_Read(unsigned short *fw_ver);
void Dev_Mac_Init(void);
void Device_Reboot(void);
unsigned char Dev_LightInfo_Read(void);
Re_Code_T Dev_LightInfo_Write(unsigned char dir);
unsigned short Dev_OTAStatus_Read(void);
unsigned short Dev_OTAFWSize_Read(void);
unsigned short Dev_OTAStatus_Write(unsigned short status);
unsigned short Dev_OTAFWSize_Write(unsigned short size);
unsigned short Dev_OTAFW_Migration(unsigned int des, unsigned int src,unsigned short size);
unsigned short Dev_OTAFlags_Write(unsigned int addr,unsigned  short data);
#endif


#ifndef _SYSPARAMS_H_
#define _SYSPARAMS_H_

/*Flash��������*/
/*
0x0800 0000 ~ 0x080023FF : BootLoader
0x0800 2400 ~ 0x080027FF : SystemParameters
0x0800 2800 ~ 0x080093FF : Appliction_Code Area
0x0800 9400 ~ 0x0800FFFF : Backup Area
*/

#define SysParamAddr_Base 0x08009000
#define UNIQUE_ID_ADDR 0x1FFFF7E8
//
/*����ϵͳ���� Flash�洢��ַ*/
#define DevLoc_FlashAddr             SysParamAddr_Base + 0x00 //
#define DevUpLoadPer_FlashAddr       SysParamAddr_Base + 0x04 //


extern unsigned short DEV_LOC[2];
extern unsigned short Upload_Period;
extern char mac_strings[20];
typedef enum {
	Success = 0x00, Failed = 0x01, Invalid_Para = 0x03,
}Re_Code_T;


/*�豸λ����Ϣ��ʼ��*/
void Dev_Location_Init(void);
/*�豸λ����Ϣ�޸�*/
Re_Code_T Dev_Location_Change(unsigned short *devloc_new);
/*�豸�ϴ����ڳ�ʼ��*/
void Dev_UpLoad_Per_Init(void);
/*�豸�ϴ������޸�*/
Re_Code_T Dev_UpLoad_Per_Change(unsigned short upload_per_new);
void Dev_FwVer_Read(unsigned short *fw_ver);
void Dev_Mac_Init(void);
#endif


#include "sysparams.h"
#include "stm32f1xx_hal.h"
#include "stmflash.h"
#include "stdio.h"

unsigned short DEV_LOC[2]={0,0};
unsigned short Upload_Period = 30;
unsigned short FW_Version = 0004;
unsigned char unique_id[8] = {0, };
char mac_strings[20] ="";

void Dev_Mac_Init(void)
{
	unsigned int unique_id_32[3];
	unsigned char i = 0;
	/*��ȡоƬΨһID(96λ)���洢��3��32λ������
	unique_id_32[0] : Device_ID[31:0]
	unique_id_32[1] : Device_ID[63:32]
	unique_id_32[2] : Device_ID[95:64]
	*/
	for(i=0;i<3;++i)
		unique_id_32[i] = *(unsigned int *)(UNIQUE_ID_ADDR+i*4);
	/*��оƬΨһID��84λ�洢��8��8λ������
	unique_id_8[0] : Device_ID[63:56]
	...
	unique_id_8[7] : Device_ID[7:0]
	*/
	for(i=0;i<8;++i)
	{
		unique_id[i]  = i < 4? (unsigned char)(unique_id_32[1]>>((3-i)*8)) : (unsigned char)(unique_id_32[0]>>((7-i)*8));
		sprintf(&mac_strings[i*2],"%2x",unique_id[i]);
	}
	
	
}
/*�豸λ����Ϣ��ʼ��*/
void Dev_Location_Init(void)
{
	unsigned short DevLocInFlash[2];
	/*��ȡFlash���Ƿ���λ�ò�������Ӧ��ֵַ������0xFFFF��*/
	STMFLASH_Read(DevLoc_FlashAddr,DevLocInFlash,2);
	/*����У������Ϊ�豸λ����Ϣ����*/
	if((DevLocInFlash[0] != 0xFFFF)&&(DevLocInFlash[1] != 0xFFFF))
	{
		DEV_LOC[0] = DevLocInFlash[0];
		DEV_LOC[1] = DevLocInFlash[1];
	}
	/*���û�У���Ĭ�ϲ�������Ϊ�豸λ����Ϣ����*/
}

/*�豸λ����Ϣ�޸�*/
Re_Code_T Dev_Location_Change(unsigned short *devloc_new)
{
		/*�ж���������Ϸ���,�����Ϸ������ز������Ϸ�*/
	 if(devloc_new[1]<1000)
	{
		/*�ж���������Ƿ�͵�ǰֵ��ͬ*/
		if((devloc_new[0]!=DEV_LOC[0])||(devloc_new[1]!=DEV_LOC[1]))
		{
			/*���²���д��flash*/
			STMFLASH_Write(DevLoc_FlashAddr,devloc_new,2);
			/*����flash�����ж��Ƿ�д��ɹ�*/
			STMFLASH_Read(DevLoc_FlashAddr,DEV_LOC,2);
			if((DEV_LOC[0] == devloc_new[0]) && (DEV_LOC[1] == devloc_new[1]))
				/*�����޸ĳɹ�*/
				return  Success;
			else
				/*�����޸�ʧ��*/
				return Failed;
		}
		else
		/*��������ʹ��޸Ĳ���һ�� ֱ�ӷ����޸ĳɹ�*/	
				return Success;
	}
	else
			/*���ز�������������Ϸ�*/
		return Invalid_Para;
}


/*�豸�ϴ����ڳ�ʼ��*/
void Dev_UpLoad_Per_Init(void)
{
	unsigned short DevUploadPerInFlash;
	/*��ȡFlash���Ƿ����ϴ����ڲ�������Ӧ��ֵַ������0xFFFF��*/
	STMFLASH_Read(DevUpLoadPer_FlashAddr ,&DevUploadPerInFlash ,1);
	/*����У������Ϊ�豸�ϴ����ڲ���*/
	if(DevUploadPerInFlash != 0xFFFF)
	{
		Upload_Period = DevUploadPerInFlash;
	}
	/*���û�У���Ĭ�ϲ�������Ϊ�ϴ����ڲ���*/
}

/*�豸�ϴ������޸�*/
Re_Code_T Dev_UpLoad_Per_Change(unsigned short upload_per_new)
{
		/*�ж���������Ϸ���,�����Ϸ������ز������Ϸ�*/
	 if(upload_per_new <= 3600)
	{
		/*�ж���������Ƿ�͵�ǰֵ��ͬ*/
		if(upload_per_new != Upload_Period)
		{
			/*���²���д��flash*/
			STMFLASH_Write(DevUpLoadPer_FlashAddr,&upload_per_new,1);
			/*����flash�����ж��Ƿ�д��ɹ�*/
			STMFLASH_Read(DevUpLoadPer_FlashAddr,&Upload_Period,1);
			if(Upload_Period == upload_per_new)
				/*�����޸ĳɹ�*/
				return  Success;
			else
				/*�����޸�ʧ��*/
				return Failed;
		}
		else
		/*��������ʹ��޸Ĳ���һ�� ֱ�ӷ����޸ĳɹ�*/	
				return Success;
	}
	else
			/*���ز�������������Ϸ�*/
		return Invalid_Para;
}

/*�豸λ�ö�ȡ*/
Re_Code_T Dev_UpLoad_Per_Read(unsigned short *upload_per)
{
		
		*upload_per = Upload_Period;
			/*���ز�������������Ϸ�*/
		return Success;
}
/*�豸�ϴ����ڶ�ȡ*/
Re_Code_T Dev_Loc_Read(int *dev_loc)
{
		
	dev_loc[0] = DEV_LOC[0];
	dev_loc[1] = DEV_LOC[1];
			/*���ز�������������Ϸ�*/
		return Success;
}

/*�豸�汾�Ŷ�ȡ*/
Re_Code_T Dev_FwVer_Read(unsigned short *fw_ver)
{
	*fw_ver = FW_Version;
	return Success;
}

void Device_Reboot(void)
{
	HAL_NVIC_SystemReset();
}



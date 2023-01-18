#include "sysparams.h"
#include "stm32f1xx_hal.h"
#include "stmflash.h"
#include "stdio.h"

unsigned short Group_Info[2] = {0,0};
unsigned short DEV_LOC[2]={0,0};
unsigned short Upload_Period = 60;
unsigned short FW_Version = 0004;
unsigned char unique_id[8] = {0, };
char mac_strings[20] ="";

void Dev_Mac_Init(void)
{
	unsigned int unique_id_32[3];
	unsigned char i = 0;
	/*读取芯片唯一ID(96位)，存储于3个32位数组中
	unique_id_32[0] : Device_ID[31:0]
	unique_id_32[1] : Device_ID[63:32]
	unique_id_32[2] : Device_ID[95:64]
	*/
	for(i=0;i<3;++i)
		unique_id_32[i] = *(unsigned int *)(UNIQUE_ID_ADDR+i*4);
	/*将芯片唯一ID低84位存储于8个8位数组中
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
/*设备位置信息初始化*/
void Dev_Location_Init(void)
{
	unsigned short DevLocInFlash[2];
	/*读取Flash中是否有位置参数（对应地址值不等于0xFFFF）*/
	STMFLASH_Read(DevLoc_FlashAddr,DevLocInFlash,2);
	/*如果有，则更新为设备位置信息参数*/
	if((DevLocInFlash[0] != 0xFFFF)&&(DevLocInFlash[1] != 0xFFFF))
	{
		DEV_LOC[0] = DevLocInFlash[0];
		DEV_LOC[1] = DevLocInFlash[1];
	}
	/*如果没有，则将默认参数设置为设备位置信息参数*/
}

/*设备位置信息修改*/
Re_Code_T Dev_Location_Change(unsigned short *devloc_new)
{
		/*判断输出参数合法性,若不合法，返回参数不合法*/
	 if(devloc_new[1]<1000)
	{
		/*判断输入参数是否和当前值不同*/
		if((devloc_new[0]!=DEV_LOC[0])||(devloc_new[1]!=DEV_LOC[1]))
		{
			/*更新参数写入flash*/
			STMFLASH_Write(DevLoc_FlashAddr,devloc_new,2);
			/*读出flash，并判断是否写入成功*/
			STMFLASH_Read(DevLoc_FlashAddr,DEV_LOC,2);
			if((DEV_LOC[0] == devloc_new[0]) && (DEV_LOC[1] == devloc_new[1]))
				/*返回修改成功*/
				return  Success;
			else
				/*返回修改失败*/
				return Failed;
		}
		else
		/*输入参数和待修改参数一致 直接返回修改成功*/	
				return Success;
	}
	else
			/*返回参数输入参数不合法*/
		return Invalid_Para;
}

/*
************************************************************
*	函数名称：	Dev_GroupInfo_Init
*
*	函数功能：	设备分组信息初始化
*
*	入口参数：	void
*
*	返回参数：	void
*
*	说明：		 
************************************************************
*/
void Dev_GroupInfo_Init(void)
{
	unsigned int DevGroupInfoInFlash;
	/*读取Flash中是否有上传周期参数（对应地址值不等于0xFFFF）*/
	DevGroupInfoInFlash = STMFLASH_ReadWord(DevGroupInfo_FlashAddr);
	
	/*如果有，则更新为设备上传周期参数*/
	if(DevGroupInfoInFlash != 0xFFFFFFFF)
	{
		Group_Info[1] =(unsigned short)(DevGroupInfoInFlash >> 16);
		Group_Info[0] = (unsigned short)(DevGroupInfoInFlash &0x0000FFFF);
	}
	/*如果没有，则将默认参数设置为上传周期参数*/
}

/*
************************************************************
*	函数名称：	Dev_GroupInfo_Change
*
*	函数功能：	设备分组信息修改
*
*	入口参数：	unsigned short groupinfo_new 新的分组信息
*
*	返回参数：	修改结果
*             0 ： 修改失败
*             1 ： 修改成功
*
*	说明：		  将新的设备分组信息写入Flash，并更新全局变量
************************************************************
*/
Re_Code_T Dev_GroupInfo_Change(unsigned short *groupinfo_new)
{
		/*判断输出参数合法性,若不合法，返回参数不合法*/
		/*判断输入参数是否和当前值不同*/
		if((groupinfo_new[0]!=Group_Info[0])||(groupinfo_new[1]!=Group_Info[1]))
		{
			//GroupInfo_Buff = (unsigned int)groupinfo_new[0]<<16 | groupinfo_new[1];
			/*更新参数写入flash*/
			STMFLASH_Write(DevGroupInfo_FlashAddr,groupinfo_new,2);
			/*读出flash，并判断是否写入成功*/
			STMFLASH_Read(DevGroupInfo_FlashAddr,Group_Info,2);

			if((Group_Info[0] == groupinfo_new[0]) && (Group_Info[1] == groupinfo_new[1]))
				/*返回修改成功*/
				return  Success;
			else
				/*返回修改失败*/
				return Failed;
		}
		else
		/*输入参数和待修改参数一致 直接返回修改成功*/	
				return Success;
}


/*设备上传周期初始化*/
void Dev_UpLoad_Per_Init(void)
{
	unsigned short DevUploadPerInFlash;
	/*读取Flash中是否有上传周期参数（对应地址值不等于0xFFFF）*/
	STMFLASH_Read(DevUpLoadPer_FlashAddr ,&DevUploadPerInFlash ,1);
	/*如果有，则更新为设备上传周期参数*/
	if(DevUploadPerInFlash != 0xFFFF)
	{
		Upload_Period = DevUploadPerInFlash;
	}
	/*如果没有，则将默认参数设置为上传周期参数*/
}

/*设备上传周期修改*/
Re_Code_T Dev_UpLoad_Per_Change(unsigned short upload_per_new)
{
		/*判断输入参数合法性,若不合法，返回参数不合法*/
	 if(upload_per_new <= 3600)
	{
		/*判断输入参数是否和当前值不同*/
		if(upload_per_new != Upload_Period)
		{
			/*更新参数写入flash*/
			STMFLASH_Write(DevUpLoadPer_FlashAddr,&upload_per_new,1);
			/*读出flash，并判断是否写入成功*/
			STMFLASH_Read(DevUpLoadPer_FlashAddr,&Upload_Period,1);
			if(Upload_Period == upload_per_new)
				/*返回修改成功*/
				return  Success;
			else
				/*返回修改失败*/
				return Failed;
		}
		else
		/*输入参数和待修改参数一致 直接返回修改成功*/	
				return Success;
	}
	else
			/*返回参数输入参数不合法*/
		return Invalid_Para;
}

/*设备位置读取*/
Re_Code_T Dev_UpLoad_Per_Read(unsigned short *upload_per)
{
		
		*upload_per = Upload_Period;
			/*返回参数输入参数不合法*/
		return Success;
}
/*设备上传周期读取*/
Re_Code_T Dev_Loc_Read(int *dev_loc)
{
		
	dev_loc[0] = DEV_LOC[0];
	dev_loc[1] = DEV_LOC[1];
			/*返回参数输入参数不合法*/
		return Success;
}

/*设备版本号读取*/
Re_Code_T Dev_FwVer_Read(unsigned short *fw_ver)
{
	*fw_ver = FW_Version;
	return Success;
}

void Device_Reboot(void)
{
	HAL_NVIC_SystemReset();
}



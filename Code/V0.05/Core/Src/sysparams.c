#include "sysparams.h"
#include "stm32f1xx_hal.h"
#include "stmflash.h"
#include "stdio.h"

unsigned short Group_Info[2] = {50,1};
unsigned short DEV_LOC[2]={0,0};
unsigned short Upload_Period = 60;

unsigned short FW_REC_VER = 0x0000;
unsigned short FW_Size = 0x0000;
unsigned char fwpack_total = 0;
unsigned char packsn_query = 0x00;
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

/*设备OTA信息初始化，主要检查OTAStatus标志，判断是否需要断点续传*/
void Dev_OTA_Init(void)
{
	unsigned short ota_status ,fwqrysn = 0xFFFF;
	
	STMFLASH_Read(OTAStatus_FlashAddr,&ota_status,1);
	//如果ota_status == 0X44 说明在上次接收固件时中断，需要续传
	if(ota_status == 0x44){
		STMFLASH_Read(OTAFWSize_FlashAddr,&FW_Size,1);
		STMFLASH_Read(OTAFWVER_FlashAddr,&FW_REC_VER,1);
		STMFLASH_Read(OTAFWQrySN_FlashAddr,&fwqrysn,1);
		fwpack_total = FW_Size / 128;
		if(FW_Size % 128 != 0)
			fwpack_total += 1;
		packsn_query = fwqrysn;
	}
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
*	函数名称：	Dev_LightInfo_Init
*
*	函数功能：	设备分组信息初始化
*
*	入口参数：	void
*
*	返回参数：	unsigned char [0~3] 灯光指示方向
*
*	说明：		 
************************************************************
*/
unsigned char Dev_LightInfo_Read(void)
{
	unsigned int DevLightInfoInFlash;
	/*读取Flash中是否有上传周期参数（对应地址值不等于0xFFFF）*/
	DevLightInfoInFlash = STMFLASH_ReadWord(DevLightInfo_FlashAddr);
	
	/*如果有，则更新为设备上传周期参数*/
	if(DevLightInfoInFlash != 0xFFFFFFFF)
	{
		return (unsigned char)DevLightInfoInFlash;
	}
	/*如果没有，则将默认参数设置为上传周期参数*/
	else 
		return 3;
}

/*
************************************************************
*	函数名称：	Dev_LightInfo_Write
*
*	函数功能：	设备分组信息修改
*
*	入口参数：	unsigned char  新灯光状态信息
*
*	返回参数：	修改结果
*             0 ： 修改失败
*             1 ： 修改成功
*							3 :  参数不合法
*
*	说明：		  将新的设备分组信息写入Flash，并更新全局变量
************************************************************
*/
Re_Code_T Dev_LightInfo_Write(unsigned char dir)
{
		/*判断输出参数合法性,若不合法，返回参数不合法*/
		if( dir > 3 )
			return Invalid_Para;
		else
		{
				unsigned short DevLightInfoInFlash;
		/*读取Flash中是否有上传周期参数（对应地址值不等于0xFFFF）*/
				STMFLASH_Read(DevLightInfo_FlashAddr,&DevLightInfoInFlash,1);
				if((unsigned char)DevLightInfoInFlash == dir)
					return Success;
				else
				{
					DevLightInfoInFlash = dir;
					STMFLASH_Write(DevLightInfo_FlashAddr,&DevLightInfoInFlash,1);
					/*读出flash，并判断是否写入成功*/
					STMFLASH_Read(DevLightInfo_FlashAddr,&DevLightInfoInFlash,1);

					if((unsigned char)DevLightInfoInFlash == dir)
						return Success;
					else
				/*返回修改失败*/
						return Failed;
				}
		}
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

/*设备上传周期读取*/
Re_Code_T Dev_UpLoad_Per_Read(unsigned short *upload_per)
{
		
		*upload_per = Upload_Period;
			/*返回参数输入参数不合法*/
		return Success;
}
/*设备位置读取*/

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

/*新固件推送信息验证
	1、对比新固件版本号是否高于当前版本，若不高于当前版本则返回 0x02
  2、验证新固件体积是否超过上限，若超过则返回0x03

*/
int FwInfo_Verify(unsigned short fw_ver_new,unsigned short fw_size)
{
	if(fw_ver_new > FW_Version){
		if( fw_size <= AppArea_Size){
			FW_REC_VER  = fw_ver_new;
			packsn_query = 0x00;
			FW_Size = fw_size;
			fwpack_total = fw_size / 128;
			if(fw_size % 128 != 0)
				fwpack_total += 1;
			Dev_OTAFlags_Write(OTAStatus_FlashAddr,0x44);
			Dev_OTAFlags_Write(OTAFWQrySN_FlashAddr,0x00);
			Dev_OTAFlags_Write(OTAFWVER_FlashAddr,FW_REC_VER);
			Dev_OTAFlags_Write(OTAFWSize_FlashAddr,FW_Size);
			return 0x01;
		}
		else
			return 0x02;
	}
	else
		return 0x03;
}

/*新固件推送信息验证
	1、对比新固件版本号是否高于当前版本，若不高于当前版本则返回 0x02
  2、验证新固件体积是否超过上限，若超过则返回0x03

*/
int FwRec_Verify(unsigned short fw_ver_new,unsigned char packsn_recv,unsigned char *start_addr,unsigned char pack_size)
{
	if(fw_ver_new != FW_REC_VER){
		return  0x02;
	}
	else
	{
		if(packsn_recv != packsn_query)
			return 0x04;
		else
		{
			if(pack_size > FWPack_MaxSize)
				return 0x03;
			else
			{
				FW_Write(packsn_recv,start_addr,pack_size);
				packsn_query++;
				Dev_OTAFlags_Write(OTAFWQrySN_FlashAddr,(unsigned short)packsn_query);
				if(packsn_query == fwpack_total)
				{
					FW_REC_VER = 0;
					packsn_query = 0;
					Dev_OTAFlags_Write(OTAFWSize_FlashAddr,FW_Size);
					Dev_OTAFlags_Write(OTAStatus_FlashAddr,0x55);
					//跳转至bootloader
					Device_Reboot();
				}
				return 0x01;
			}
		
		}
		
	}
}


int FW_Write(unsigned char pack_sn,unsigned char *pBuffer,unsigned char pack_size){
	//定义16字节数组缓冲区 
	unsigned short uscachebuff[64] ={0,};
	//定义需要写入flash的16-bits数据的个数
	unsigned char ucNumtoWrite  = 0;
	//整除计算第一次copy的16-bits数据数量
	ucNumtoWrite = pack_size / 2;
	int i = 0;
	for(i = 0; i < ucNumtoWrite; ++i)
	{
		uscachebuff[i] = ((uint16_t)(*(pBuffer+i*2 + 1)) << 8) | (*(pBuffer + i*2));
	}
	//如果写入的8-bits数据数量为奇数个，则最后还需要补一个0xFF后，写入最后一个16-bits数据
	if( pack_size % 2 != 0)
	{
		uscachebuff[i] = 0xFF00 | (*(pBuffer + i*2));
		ucNumtoWrite += 1;
	}
	
	//将固件写入Flash
	unsigned int writeaddr = 0;
	writeaddr = pack_sn * 128 + BackUpAddr_Base;
	STMFLASH_Write(writeaddr,uscachebuff,ucNumtoWrite);	
	return 1;
}

unsigned short Dev_OTAStatus_Read(void)
{
	unsigned short status = 0x0000;
	/*读出flash，并判断是否写入成功*/
	STMFLASH_Read(OTAStatus_FlashAddr,&status,1);
	return status;
}


unsigned short Dev_OTAFWSize_Read(void)
{
	unsigned short size = 0x0000;
	/*读出flash，并判断是否写入成功*/
	STMFLASH_Read(OTAStatus_FlashAddr,&size,1);
	return size;
}




unsigned short Dev_OTAFlags_Write(unsigned int addr,unsigned  short data)
{
	unsigned short data_write = 0;
	data_write  = data;
	/*读出flash，并判断是否写入成功*/
	STMFLASH_Write(addr,&data_write,1);
	return 1;
}


void Device_Reboot(void)
{
	HAL_NVIC_SystemReset();
}



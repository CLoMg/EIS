#ifndef _SELFCHECK_H_
#define _SELFCHECK_H_





#define DEV_OK		1
#define DEV_ERR		0
#define CHECK_ERROR(e) if(e != 0) return e;

typedef struct
{

	unsigned short SHT20_OK;		//温湿度传感器正常标志位
	unsigned short ADXL345_OK ;		//三轴传感器正常标志位
	unsigned short EEPROM_OK ;		//存储器正常标志位
	unsigned short OLED_OK ;			//OLE正常标志位
	
	unsigned short NET_DEVICE_OK ;	//网络设备正常标志位

} CHECK_INFO;



extern CHECK_INFO checkInfo;



void Check_PowerOn(void);


#endif

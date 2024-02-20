#ifndef _SELFCHECK_H_
#define _SELFCHECK_H_





#define DEV_OK		1
#define DEV_ERR		0
#define CHECK_ERROR(e) if(e != 0) return e;

typedef struct
{

	unsigned short SHT20_OK;		//��ʪ�ȴ�����������־λ
	unsigned short ADXL345_OK ;		//���ᴫ����������־λ
	unsigned short EEPROM_OK ;		//�洢��������־λ
	unsigned short OLED_OK ;			//OLE������־λ
	
	unsigned short NET_DEVICE_OK ;	//�����豸������־λ

} CHECK_INFO;



extern CHECK_INFO checkInfo;



void Check_PowerOn(void);


#endif

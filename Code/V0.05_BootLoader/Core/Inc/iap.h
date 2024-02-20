#ifndef _IAP_
#define _IAP_

#include "stm32f1xx.h"
#include "stdint.h"

#define FLASH_APP1_ADDR     0x08002800     //��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)

typedef  void (*iapfun)(void);              //����һ���������͵Ĳ���.
void MSR_MSP(uint32_t addr);    //���ö�ջ��ַ


void iap_load_app(uint32_t appxaddr);           //ִ��flash�����app����

#endif

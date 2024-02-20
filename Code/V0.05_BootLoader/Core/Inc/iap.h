#ifndef _IAP_
#define _IAP_

#include "stm32f1xx.h"
#include "stdint.h"

#define FLASH_APP1_ADDR     0x08002800     //第一个应用程序起始地址(存放在FLASH)

typedef  void (*iapfun)(void);              //定义一个函数类型的参数.
void MSR_MSP(uint32_t addr);    //设置堆栈地址


void iap_load_app(uint32_t appxaddr);           //执行flash里面的app程序

#endif

#ifndef __LED_H
#define __LED_H
#include "sys.h"

//定义LED控制引脚，旧版本 LED1-PB8 LED2--PB9
//                 新版本 LED1-PD11 LED2--PD12
//#define LED1 PBout(8)
//#define LED2 PBout(9)

#define LED1 PDout(11)
#define LED2 PDout(12)

void led_init(void);
void led_beep(uint8_t pin,uint16_t gap);
#endif

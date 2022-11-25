#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>


void timer0_config(uint16_t prescaler,uint16_t period);
void delay_us(uint32_t n_us);
void delay_ms(uint16_t n_ms);
#endif

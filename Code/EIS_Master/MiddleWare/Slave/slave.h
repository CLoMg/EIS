#ifndef SLAVE_H
#define SLAVE_H
#include <stdint.h>

void Slave_Init(void);
void Slave_Process(void);
unsigned char LRC_Value_Calc(unsigned char *data_buff,unsigned char len);
#endif

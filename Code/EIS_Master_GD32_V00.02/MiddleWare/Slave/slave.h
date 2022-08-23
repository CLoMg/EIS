#ifndef SLAVE_H
#define SLAVE_H
#include <stdint.h>

void Sensor_Init(void);
void Sensor_Process(void);
unsigned char LRC_Value_Calc(unsigned char *data_buff,unsigned char len);
void FIRE_Monitor_Init(void);
void Water_Monitor_Init(void);
#endif

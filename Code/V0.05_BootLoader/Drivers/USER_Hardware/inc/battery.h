#ifndef BATTERY_H
#define BATTERY_H

void PowerMonitor_Init(void);
static void ADC_Init(void);
static unsigned char Voltage_Get(void);
extern void PowerMonitor_Process(void);
unsigned short ADC_Value_Get(void);
#endif

#ifndef BATTERY_H
#define BATTERY_H

extern float BAT_PCT;
void PowerMonitor_Init(void);
extern void PowerMonitor_Process(void);
float ADC_Value_Get(void);
unsigned char Battery_Percentage_Calc(float vol);
#endif

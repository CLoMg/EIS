#ifndef  __MOTOR_H_
#define  __MOTOR_H_

#include "sys.h"

extern _Bool Motor_Active_Flag;

#define Motor_Phase_A PEout(13)
#define Motor_Phase_B PEout(12)
#define Motor_Phase_C PEout(11)
#define Motor_Phase_D PEout(10)

void Motor_Init(void);
void Motor_Driver_Config(void);
void Motor_PowerEN_Config(void);
void Motor_One_Step(uint8_t ms);
void Motor_One_Circle(uint8_t ms);

void Motor_N_Circle(uint16_t circle_num,uint8_t ms);

#endif

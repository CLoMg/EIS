#ifndef LED_H
#define LED_H

void LED_FSM_Init(void);
static void LED_Init_Func(void);
static void	Light_Slow_Func(void);
static void	Light_Inter_Func(void);
static void Light_Quick_Func(void);
static void Light_Sharp_Func(void);
static void OnLightEvent(void *context);

static void LED_Toggle_Int_Set(unsigned int);

void LED_Process(void);

#endif

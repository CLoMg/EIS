#include "led.h"
#include "fsm.h"
#include "gd32f10x_eval.h"
#include "stdlib.h"
#include "gd32f10x_timer.h"

enum led_states
{
	UnInit = 0x00, Normal, Sensor_Error, DisConnect, BT_Connect, 
};

typedef enum  
{
	LED_Init_Event = 0x00, LED_DisConn_Event, LED_BTConn_Event,
	LED_SensorErr_Event, LED_ReNormal_Event
}Events_T;
static StateTransform_T trans_table[] = 
{
	{UnInit,LED_Init_Event,Normal,LED_Init_Func},
	{Normal,LED_DisConn_Event,DisConnect,Light_Quick_Func},
	{Normal,LED_BTConn_Event,BT_Connect,Light_Inter_Func},
	{Normal,LED_SensorErr_Event,Sensor_Error,Light_Sharp_Func},
	{BT_Connect,LED_ReNormal_Event,Normal,Light_Slow_Func},
	{BT_Connect,LED_SensorErr_Event,Sensor_Error,Light_Sharp_Func},
	{BT_Connect,LED_DisConn_Event,DisConnect,Light_Quick_Func},
	{Sensor_Error,LED_ReNormal_Event,Normal,Light_Slow_Func},
	{Sensor_Error,LED_BTConn_Event,BT_Connect,Light_Inter_Func},
	{Sensor_Error,LED_DisConn_Event,DisConnect,Light_Quick_Func}
};

unsigned char LED_States_Flag = 0;

FSM_T* LED_FSM;
static EventsID_T event;
uint32_t LED_Light_PERIOD_MS=0;

//static UTIL_TIMER_Object_t timerStatesLED;
static unsigned short acq_int = 0;
static void OnAcqEvent(void *context);

void LED_FSM_Init(void)
{
	LED_FSM = (FSM_T*)malloc(sizeof(FSM_T));
	FSM_Regist(LED_FSM,trans_table);
	FSM_StateTransfer(LED_FSM,UnInit);
	LED_FSM->transform_num = 10;
	event = LED_Init_Event;
//	FSM_EventHandle(LED_FSM,event);
}

static void LED_Init_Func(void){
//	LED_Light_PERIOD_MS = 5000;
//	UTIL_TIMER_Create(&timerStatesLED, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnLightEvent, NULL);
//	UTIL_TIMER_SetPeriod(&timerStatesLED, LED_Light_PERIOD_MS);
//	UTIL_TIMER_Start(&timerStatesLED);
}
	
static void	Light_Slow_Func(void){
	LED_Toggle_Int_Set(5000);
}	
static void	Light_Inter_Func(void){
	LED_Toggle_Int_Set(2000);
}
static void Light_Quick_Func(void){
	LED_Toggle_Int_Set(1000);
}
static void Light_Sharp_Func(void){
	LED_Toggle_Int_Set(500);
}


static void OnLightEvent(void *context)
{
//	BSP_LED_Toggle(LED1);
//  UTIL_TIMER_Start(&timerStatesLED);
}

static void LED_Toggle_Int_Set(unsigned int PERIOD_MS)
{
//	UTIL_TIMER_Stop(&timerStatesLED);
//	UTIL_TIMER_SetPeriod(&timerStatesLED, PERIOD_MS);
//  UTIL_TIMER_Start(&timerStatesLED);
}

unsigned char event_index=0;
void LED_Process(void)
{
	for(;event_index <5;event_index++){
		if(LED_States_Flag & (0x01<<event_index))
				event=event_index;
	};
	FSM_EventHandle(LED_FSM,event);
}


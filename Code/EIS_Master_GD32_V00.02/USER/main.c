/*!
    \file    main.c
    \brief   led spark with systick, USART print and key example

    \version 2014-12-26, V1.0.0, firmware for GD32F10x
    \version 2017-06-20, V2.0.0, firmware for GD32F10x
    \version 2018-07-31, V2.1.0, firmware for GD32F10x
    \version 2020-09-30, V2.2.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32f10x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "queue.h"
#include "semphr.h"
#include "gd32f10x_eval.h"
#include "direct.h"
#include "fsm.h"
#include "led.h"
#include "lora_module.h"
#include "slave.h"
#include "spi.h"
#include "msg_handler.h"
#include "timer.h"
#include "rs485.h"
#include "gd32f10x_fwdgt.h"
#include "iap.h"
#include "battery.h"
#include "uart.h"
#include "rs485.h"

void IWDGT_Init(void);
void IWDGT_Feed(void);
void SystemClock_Config(void);

static void Direct_task(void *pvParameters);
static void SensorMonitor_task(void *pvParameters);
static void MSGTR_task(void *pvParameters);
static void LORACTL_task(void *pvParameters);
static void PowerMonitor_task(void *pvParameters);
static void IAP_task(void *pvParameters);
static void Timer_task(void *pvParameters);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
  /* configure systick */
		timer0_config(0,108);
    //systick_config();
  /* initilize the LEDs, USART and key */
   	gd_eval_led_init(RUN_LED);
		gd_eval_led_init(ERR_LED);
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
	
//  /* USER CODE BEGIN 2 */
		LORAControl_Queue = xQueueCreate(5,sizeof(xData));
		MSGTR_Queue = xQueueCreate(5,sizeof(xData));
		Direction_Queue = xQueueCreate(5,sizeof(xData));
		SensorMonitor_Queue = xQueueCreate(5,sizeof(xData));
		PowerMonitor_Queue = xQueueCreate(5,sizeof(xData));
		SysInfo_Queue = xQueueCreate(5,sizeof(xData));
		xStateMutex = xSemaphoreCreateMutex();
//		
		xTaskCreate(MSGTR_task,"MSGTR_Task",128,NULL,5,NULL);
		xTaskCreate(LORACTL_task,"LORACTL_Task",128,NULL,3,NULL);
  	xTaskCreate(Direct_task,"Direct_Task",128,NULL,3,NULL);
		xTaskCreate(SensorMonitor_task,"SensorMonitor_Task",128,NULL,3,NULL);
		xTaskCreate(PowerMonitor_task,"PowerMonitor_Task",64,NULL,3,NULL);
		xTaskCreate(Timer_task,"Timer_Task",64,NULL,4,NULL);
		//xTaskCreate(IAP_task,"IAP_Task",64,NULL,3,NULL);
		

  	vTaskStartScheduler();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while(1)
	{

	}
}

static void Direct_task(void *pvParameters)
{
	Direct_Init();
	portBASE_TYPE readQueue_Status;
	xData Direction_Msg;
	xData *re_code;
	while(1)
	{
		readQueue_Status = xQueueReceive(Direction_Queue,&Direction_Msg,portMAX_DELAY);
		if(readQueue_Status == pdTRUE)
		{
			re_code = (xData *)Dir_Func((int *)&Direction_Msg);
			if(re_code->iValue != NULL)
				xQueueSend(MSGTR_Queue,re_code,100/portTICK_RATE_MS);
		}
	}
}
static void SensorMonitor_task(void *pvParameters)
{
	Sensor_Init();
	xData sensor_monitor_msg;
	portBASE_TYPE readQueue_Status;
	while(1)
	{	
		readQueue_Status = xQueueReceive(SensorMonitor_Queue,&sensor_monitor_msg,portMAX_DELAY);
		if(readQueue_Status == pdTRUE)
		{
			Sensor_Process();
		}
	}
}


static void MSGTR_task(void *pvParameters)
{
	xData msg_element;
	portBASE_TYPE readQueue_Status;
	while(1)
	{
		readQueue_Status = xQueueReceive(MSGTR_Queue,&msg_element,portMAX_DELAY);
		if(readQueue_Status == pdTRUE){
			switch(msg_element.iEventType){
				case LORARxEvent:
					LORA_RX_Func(&msg_element);
					break;
				case LORATxEvent:
					LORA_TX_Func(&msg_element);
					break;
				default:
					break;
			}
		}
	}
}
size_t free_heap = 0;
static void LORACTL_task(void *pvParameters){
	const portTickType LORACTL_TaskDelay = 100 / portTICK_RATE_MS;

	portBASE_TYPE readQueue_Status;
	xData msg_element;
	LORA_Init();
	rs485_init(USART1,4800);

	while(1)
	{
		readQueue_Status =  xQueueReceive(LORAControl_Queue,&msg_element,LORACTL_TaskDelay);

		LORA_Para_Func(&msg_element);
		if(readQueue_Status == pdTRUE)
		{
			vPortFree(msg_element.iValue);
		}
		LORACTL_FMS_RUN();
		vTaskDelay(LORACTL_TaskDelay);
	}
}

static void PowerMonitor_task(void *pvParameters){
	
	const portTickType PowerMonitor_TaskDelay = 100 / portTICK_RATE_MS;
	PowerMonitor_Init();
	while(1)
	{
		PowerMonitor_Process();
		gd_eval_led_toggle(RUN_LED);
		vTaskDelay(PowerMonitor_TaskDelay);
	}
}
static void IAP_task(void *pvParameters)
{
	const portTickType IAP_TaskDelay = 100 / portTICK_RATE_MS;
	while(1)
	{
	  IAP_Process();
		
		vTaskDelay(IAP_TaskDelay);
	}
}

static void Timer_task(void *pvParameters)
{
	const portTickType Timer_TaskDelay = 100 / portTICK_RATE_MS;
	static unsigned char cnt =0 ;
	xData timer_msg;
	while(1)
	{
		cnt++;
		if(cnt%50 ==0){
			timer_msg.iMeaning = Light_State_Refresh;
			xQueueSend(Direction_Queue,&timer_msg,100/portTICK_RATE_MS);
			timer_msg.iMeaning = Light_State_Refresh;
			xQueueSend(SensorMonitor_Queue,&timer_msg,100/portTICK_RATE_MS);
		}
		if(cnt %100 ==0)
		{
			timer_msg.iMeaning = Data_Upload;
			timer_msg.iEventType = LORATxEvent;
			xQueueSend(MSGTR_Queue,&timer_msg,100/portTICK_RATE_MS);
		}
		vTaskDelay(Timer_TaskDelay);
	}
}


/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}

void IWDGT_Init(void)
{
	// ±÷”≥ı ºªØ
	rcu_osci_on(RCU_IRC40K);
	while(ERROR == rcu_osci_stab_wait(RCU_IRC40K));
	
	fwdgt_config(271,FWDGT_PSC_DIV128);
	fwdgt_enable();
}

void IWDGT_Feed(void)
{
	fwdgt_counter_reload();
}

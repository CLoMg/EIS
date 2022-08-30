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
void SystemClock_Config(void);
static void States_task(void *pvParameters);
static void Direct_task(void *pvParameters);
static void Slave_task(void *pvParameters);
static void LORA_TR_task(void *pvParameters);
static void Msg_Disp_task(void *pvParameters);

void IWDGT_Init(void);

void IWDGT_Feed(void);

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
		//rs485_init(1,9600);
		LORA_Init();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
	
//  /* USER CODE BEGIN 2 */
		xTaskCreate(LORA_TR_task,"LORA_TR_Task",128,NULL,6,NULL);
  	xTaskCreate(Direct_task,"Direct_Task",128,NULL,5,NULL);
		xTaskCreate(Msg_Disp_task,"Msg_Disp_task",128,NULL,7,NULL);
		xTaskCreate(Slave_task,"Slave_Task",128,NULL,6,NULL);
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
	const portTickType Direct_TaskDelay = 100 / portTICK_RATE_MS;
	while(1)
	{
		Dir_Func();
		vTaskDelay(Direct_TaskDelay);
	}
}
static void Slave_task(void *pvParameters)
{
	Slave_Init();
	const portTickType Sensor_TaskDelay = 1000 / portTICK_RATE_MS;
	while(1)
	{	
		gd_eval_led_toggle(RUN_LED);
		Slave_Process();
		vTaskDelay(Sensor_TaskDelay);
	}
}


static void LORA_TR_task(void *pvParameters)
{
	const portTickType LORA_TaskDelay = 100 / portTICK_RATE_MS;
	while(1)
	{
		LORA_TR_Func();
		vTaskDelay(LORA_TaskDelay);
	}
}

static void Msg_Disp_task(void *pvParameters)
{
	const portTickType MSG_DISP_TaskDelay = 100 / portTICK_RATE_MS;
	MSG_FSM_Init();
	IWDGT_Init();
	while(1)
	{
		IWDGT_Feed();
		MSG_FSM_RUN_Func();
		MSG_Handler_Func();
		vTaskDelay(MSG_DISP_TaskDelay);
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

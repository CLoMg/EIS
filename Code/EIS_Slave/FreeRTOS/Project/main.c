/*!
    \file  main.c
    \brief key demo 
    
    \version 2018-06-19, V1.0.0, firmware for GD32E230
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

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

#include "gd32e230.h"
#include "gd32e230c_eval.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "temp.h"
#include <stdio.h>
#include "ec_tb600.h"
#include "ws_gas.h"
#include "spi.h"
#include "scd4x_i2c.h"
#include "uart.h"
#include "dsl_08.h"

#define Temp_Task_PRIO   ( tskIDLE_PRIORITY + 1 )
#define Gas_Task_PRIO    ( tskIDLE_PRIORITY + 1 )
#define AQI_Task_PRIO    ( tskIDLE_PRIORITY + 1 )
#define SPI_Task_PRIO    ( tskIDLE_PRIORITY + 2 )
#define SCDxx_Task_PRIO  ( tskIDLE_PRIORITY + 1 )
 
void Temp_Task(void * pvParameters);
void Gas_Task(void * );
void AQI_Task(void * );
void SPI_Task(void * );
void SCDxx_Task(void * );



/* Binary semaphore handle definition. */
SemaphoreHandle_t binary_semaphore;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
		BSP_IIC_Init(0);
		//systick_init();
		delay_init();
		BSP_MultiPlexer_Init();
//    /* init task */

	xTaskCreate(Temp_Task, "temp",   ( ( unsigned short ) 110 ), NULL, Temp_Task_PRIO, NULL);
	xTaskCreate(SCDxx_Task, "scdxx", ( ( unsigned short ) 150 ), NULL, SCDxx_Task_PRIO, NULL);
//		//xTaskCreate(Gas_Task, "gas", configMINIMAL_STACK_SIZE, NULL, Gas_Task_PRIO, NULL);
//		xTaskCreate(AQI_Task, "aqi", ( ( unsigned short ) 100 ), NULL, AQI_Task_PRIO, NULL);
		xTaskCreate(SPI_Task, "spi",  ( ( unsigned short ) 60 ), NULL, SPI_Task_PRIO, NULL);
//    
//    /* start scheduler */
    vTaskStartScheduler();

    while (1){
			;
    }
}

/*!
    \brief      Temp task
    \param[in]  pvParameters not used
    \param[out] none
    \retval     none
*/
void Temp_Task(void * pvParameters)
{  
	  size_t free_heap_size=0, min_temp =100;
	
		Temp_Init();
    for( ;; ){
			free_heap_size = uxTaskGetStackHighWaterMark(NULL);
			min_temp = free_heap_size < min_temp? free_heap_size:min_temp;
			 // printf("Free Heap Size:%d bytes \r\n",free_heap_size);
        /* Acquire temperature data 100HZ */
			Temp_Data_Acquire();

			vTaskDelay(100/portTICK_RATE_MS);
    }
}

/*!
    \brief      Temp task
    \param[in]  pvParameters not used
    \param[out] none
    \retval     none
*/
void SCDxx_Task(void * pvParameters)
{  
		size_t free_heap_size=0, min_scd =100;
		scd4x_init();
    for( ;; ){
        /* Acquire temperature data 100HZ */
			free_heap_size = uxTaskGetStackHighWaterMark(NULL);
			min_scd = free_heap_size < min_scd? free_heap_size:min_scd;
			SCDxx_Process();

			vTaskDelay(100/portTICK_RATE_MS);
    }
}

/*!
    \brief      Gas task
    \param[in]  pvParameters not used
    \param[out] none
    \retval     none
*/
void Gas_Task(void * pvParameters)
{  
		size_t mini_ever_freesize =0 ;
		pump_io_init();
		_Bool pump_state = 0;

		WS_Sensor_Init();
		BSP_MultiPlexer_Init();
    for( ;; ){
				//mini_ever_freesize =  uxTaskGetStackHighWaterMark(NULL);
				//WS_Acquire_ALL();
			  pump_states_change(pump_state);
				pump_state=!pump_state;
        vTaskDelay(100/portTICK_RATE_MS);
    }
}

/*!
    \brief      AQI task
    \param[in]  pvParameters not used
    \param[out] none
    \retval     none
*/
void AQI_Task(void * pvParameters)
{  
		size_t free_heap_size=0, min_aqi =100;
		DSL_Sensor_Init();
    for( ;; ){
//        /* toggle LED2 each 500ms */
			free_heap_size = uxTaskGetStackHighWaterMark(NULL);
			min_aqi = free_heap_size < min_aqi? free_heap_size:min_aqi;
			 AQI_Process();
       vTaskDelay(100/portTICK_RATE_MS);
    }
}

/*!
    \brief      SPI task
    \param[in]  pvParameters not used
    \param[out] none
    \retval     none
*/
void SPI_Task(void * pvParameters)
{  
		size_t free_heap_size=0, min_spi =100;
		spi1_init();
    for( ;; ){
        /* toggle LED2 each 500ms */
			free_heap_size = uxTaskGetStackHighWaterMark(NULL);
			min_spi = free_heap_size < min_spi? free_heap_size:min_spi;
			SPI_Process();
      vTaskDelay(100/portTICK_RATE_MS);
    }
}



/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM, (uint8_t) ch);
    while (RESET == usart_flag_get(EVAL_COM, USART_FLAG_TBE));
    return ch;
}




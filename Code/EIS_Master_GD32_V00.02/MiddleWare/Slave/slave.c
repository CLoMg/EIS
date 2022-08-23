#include "slave.h"

#include "spi.h"
#include "gd32f10x_eval.h"
#include "timer.h"
#include "msg_handler.h"
#include "string.h"
#include "gd32f10x_it.h"
#include "scd4x_i2c.h"
#include "dsl_08.h"
#include "freertos.h"
#include "queue.h"

#define ReadData  0x52 
#define WriterData 0x57




void Sensor_Init(void)
{
	/*Parameter config*/
  scd4x_init();
	DSL_Sensor_Init();
	pump_io_init();
	FIRE_Monitor_Init();
	Water_Monitor_Init();
	
	/*Instance structure initialize*/

	/*communication interface initialize*/
	
	/*timer initialize*/
	
	/*FSM Initialize*/

}


void Fire_Irq_Callback(void){
  if(RESET==gpio_input_bit_get(FIRE_EXIT_GPIO_PORT, FIRE_EXIT_PIN))
			gd_eval_led_toggle(RUN_LED);
}

void Water_Irq_Callback(void){
  if(RESET==gpio_input_bit_get(FIRE_EXIT_GPIO_PORT, FIRE_EXIT_PIN))
			gd_eval_led_toggle(ERR_LED);
}
void FIRE_Monitor_Init(void){
	gd_eval_EXIT_init(EXIT_FIRE, EXIT_MODE_EXTI);
}
void Water_Monitor_Init(void){
		gd_eval_EXIT_init(EXIT_WATER, EXIT_MODE_EXTI);
}

xData sensor_data;
void Sensor_Process(){
	uint8_t sensor_data_buff[19];
	//pump_states_change(1);
	delay_ms(100);
	SCDxx_Process(sensor_data_buff);
	AQI_Process(&sensor_data_buff[13]);
	//pump_states_change(0);
	
	xSemaphoreTake(xStateMutex,portMAX_DELAY);
	memcpy((char *)&Sensor_Buff[1],sensor_data_buff,19);
	xSemaphoreGive(xStateMutex);	
}

unsigned char LRC_Value_Calc(unsigned char *data_buff,unsigned char len)
{
	uint16_t lrc  = 0;
	uint16_t i=0;
	for(;i<len;++i){
		lrc += data_buff[i];
	}
	lrc = ~lrc + 1;
	return (uint8_t)(lrc & 0xff);
}

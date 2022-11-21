#include "battery.h"
#include "gd32f10x_eval.h"
#include "gd32f10x_adc.h"
#include "freertos.h"
#include "task.h"
#include "timer.h"

static _Bool Charge_Status = 0; 
#define VOLTAGE_THRESHOLD_UPPER 83
#define VOLTAGE_THRESHOLD_LOWER 65

void PowerMonitor_Init(void){
	ADC_Init();
	Charge_Control_GPIO_Init();
}

unsigned char Voltage_Get(void){
	unsigned char i=0;
	unsigned int sum = 0;
	float  voltage = 0.0f;
	for(i=0;i<5;++i){
		sum  += ADC_Value_Get(); 
	}
	voltage = (float)sum / 5 / 4096 * 3.3 * 10 * 11; 
	return (unsigned char)voltage;
}
void PowerMonitor_Process(void){
	unsigned char voltage;
	
	voltage =  Voltage_Get();
	if((voltage >= VOLTAGE_THRESHOLD_UPPER) && (Charge_Status == 1)){
		Charge_Control(0);
		Charge_Status = 0;
	}
	if((voltage < VOLTAGE_THRESHOLD_LOWER) && (Charge_Status == 0)){
		Charge_Control(1);
		Charge_Status = 1;
	}
}
unsigned short ADC_Value_Get(void){
	unsigned short adc_value= 0;
	// 配置ADC通道转换顺序，采样时间为55.5个时钟周期
	adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_9, ADC_SAMPLETIME_55POINT5);
	// 由于没有采用外部触发，所以使用软件触发ADC转换
	adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);   
			
	while(!adc_flag_get(ADC0, ADC_FLAG_EOC));                       // 等待采样完成
	adc_flag_clear(ADC0, ADC_FLAG_EOC);                             // 清除结束标志
			
	adc_value = adc_regular_data_read(ADC0);    
	return adc_value;
}
void ADC_Init(){
	
		Battery_Voltage_Monitor_GPIO_Init();
	   /* 使能ADC时钟 */
    rcu_periph_clock_enable(RCU_ADC0);
    /* 配置ADC时钟来源 这里使用APB2时钟的六分频*/
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8); 
		/*只使用了一个adc ，配置为独立模式*/
		adc_mode_config(ADC_MODE_FREE);
		/*单通道，配置为连续转换模式*/
		adc_special_function_config(ADC0,ADC_CONTINUOUS_MODE,ENABLE);
    /* 数据对齐方式 右对齐*/
    adc_data_alignment_config(ADC0,ADC_DATAALIGN_RIGHT);
    /* 配置使用ADC规则通道,使用1个 */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1U);
    /* 配置ADC触发方式:这里使用软件触发 */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_INSERTED_NONE); 
    /*使能ADC软件触发 */
    adc_software_trigger_enable(ADC0,ADC_REGULAR_CHANNEL);
    /* ADC分辨率 12B */
//    adc_resolution_config(ADC0,ADC_RESOLUTION_12B);
    /* ADC外部触发配置 */
    adc_external_trigger_config(ADC0,ADC_REGULAR_CHANNEL, ENABLE);
    /* 使能ADC */
    adc_enable(ADC0);
    /* 这个延时是为了让ADC稳定*/
		delay_us(1000);
		//vTaskDelay(10/portTICK_RATE_MS);
    //delay_1ms(1U);
    /* 进行ADC自校准 */
    adc_calibration_enable(ADC0);
}



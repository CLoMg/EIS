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
	// ����ADCͨ��ת��˳�򣬲���ʱ��Ϊ55.5��ʱ������
	adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_9, ADC_SAMPLETIME_55POINT5);
	// ����û�в����ⲿ����������ʹ���������ADCת��
	adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);   
			
	while(!adc_flag_get(ADC0, ADC_FLAG_EOC));                       // �ȴ��������
	adc_flag_clear(ADC0, ADC_FLAG_EOC);                             // ���������־
			
	adc_value = adc_regular_data_read(ADC0);    
	return adc_value;
}
void ADC_Init(){
	
		Battery_Voltage_Monitor_GPIO_Init();
	   /* ʹ��ADCʱ�� */
    rcu_periph_clock_enable(RCU_ADC0);
    /* ����ADCʱ����Դ ����ʹ��APB2ʱ�ӵ�����Ƶ*/
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8); 
		/*ֻʹ����һ��adc ������Ϊ����ģʽ*/
		adc_mode_config(ADC_MODE_FREE);
		/*��ͨ��������Ϊ����ת��ģʽ*/
		adc_special_function_config(ADC0,ADC_CONTINUOUS_MODE,ENABLE);
    /* ���ݶ��뷽ʽ �Ҷ���*/
    adc_data_alignment_config(ADC0,ADC_DATAALIGN_RIGHT);
    /* ����ʹ��ADC����ͨ��,ʹ��1�� */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1U);
    /* ����ADC������ʽ:����ʹ��������� */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_INSERTED_NONE); 
    /*ʹ��ADC������� */
    adc_software_trigger_enable(ADC0,ADC_REGULAR_CHANNEL);
    /* ADC�ֱ��� 12B */
//    adc_resolution_config(ADC0,ADC_RESOLUTION_12B);
    /* ADC�ⲿ�������� */
    adc_external_trigger_config(ADC0,ADC_REGULAR_CHANNEL, ENABLE);
    /* ʹ��ADC */
    adc_enable(ADC0);
    /* �����ʱ��Ϊ����ADC�ȶ�*/
		delay_us(1000);
		//vTaskDelay(10/portTICK_RATE_MS);
    //delay_1ms(1U);
    /* ����ADC��У׼ */
    adc_calibration_enable(ADC0);
}



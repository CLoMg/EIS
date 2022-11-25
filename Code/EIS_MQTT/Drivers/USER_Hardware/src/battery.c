#include "battery.h"
#include "adc.h"
#include "gpio.h"

static _Bool Charge_Status = 0; 
float BAT_PCT = 0;
#define VOLTAGE_THRESHOLD_UPPER 83
#define VOLTAGE_THRESHOLD_LOWER 65

void PowerMonitor_Init(void){
	MX_ADC1_Init();
	Charge_Control_GPIO_Init();
}

float Voltage_Get(void){
	float  voltage = 0.0f;
	voltage = ADC_Value_Get();
	voltage = voltage / 4096 * 3.3 * 11; //�˴�ADC�ɼ���ѹ�ǵ���������100k��10k��ӵأ�10k�������˵�ѹ�����Ե�ص�ѹӦ����ADC���������ĵ�ѹ *��110/10��
	return voltage;
}
void PowerMonitor_Process(void){
	float voltage = 0.0f;
	
	voltage =  Voltage_Get();
	
	BAT_PCT = Battery_Percentage_Calc(voltage);
	
	if((BAT_PCT >= VOLTAGE_THRESHOLD_UPPER) && (Charge_Status == 1)){
		Charge_Control(0);
		Charge_Status = 0;
	}
	if((BAT_PCT < VOLTAGE_THRESHOLD_LOWER) && (Charge_Status == 0)){
		Charge_Control(1);
		Charge_Status = 1;
	}
}

unsigned char Battery_Percentage_Calc(float vol)
{
	unsigned char pct = 0;
	pct = (vol-6.5) / (8.4-6.5) * 100;//6.5vʱ�����ŵ�100%��������ʱΪ8.4v��������µ����ٷֱ�
	return  pct;
}



#include "temp.h"
#include "string.h"
#include "gd32e230_adc.h"
#include "gd32e230c_eval.h"
#include "math.h"



unsigned char  temp_buff_index = 0;
Temp_T temp_instance;
static uint16_t ADC_Get_Channel(uint8_t);
void Temp_Init(void)
{
	Temp_IO_Init();
	Temp_Param_Config(&temp_instance);
}

/*adc io channel init*/
static void Temp_IO_Init(void)
{
	adc_gpio_config();
	ADC_Init();
}

/*structure config*/
static void Temp_Param_Config(Temp_T *instance)
{
	instance->alar_count = 0;
	instance->cur = 0;
	instance->delta = 0;
	instance->pre = 0;
	instance->mean = 0;
	memset(instance->buff,0,5);
}

extern void Temp_Data_Acquire(void)
{
	unsigned short voltage = 0;
	voltage = Voltage_Get();
	temp_instance.cur = Vol_to_Temp(voltage);
	Temp_Data_Calc();
}

static unsigned short Voltage_Get(void)
{
	float vol_f = 0.0f;
	unsigned int AD_Value_Sum=0;
	unsigned short max=0,min=0,AD_Value[12];
	for(char i=0;i<12;i++)
	{
		AD_Value[i] = ADC_Get_Channel(ADC_CHANNEL_7);
		AD_Value_Sum += AD_Value[i];
	}
	max = AD_Value[0];
	min = AD_Value[0];
	for(char i=0;i<12;i++)
	{
		max = (AD_Value[i]<max)?max:AD_Value[i];
		min = (AD_Value[i]>min)?min:AD_Value[i];
	}
	vol_f = (float)(AD_Value_Sum - max -min)/10 * (3.3/4096);
	return (unsigned short)(vol_f*1000);
}

static signed short Vol_to_Temp(unsigned short voltage)
{
	float temp_f = 0.0f;
	signed short temp_s = 0;
	
	temp_f = (8.194 - sqrt( pow(8.194,2) + 4 * 0.00262 * (1324 - voltage))) / (2 * (-0.00262)) + 30;
	temp_s = (signed short)(temp_f * 100);
	return temp_s;
}

static void Temp_Data_Calc(void)
{
	unsigned char i = 0;
	if(temp_buff_index > 4)
		temp_buff_index=0;
	temp_instance.buff[temp_buff_index++] = temp_instance.cur;
	temp_instance.delta = temp_instance.cur - temp_instance.pre;
	temp_instance.pre = temp_instance.cur;
	if(temp_instance.delta > Delta_Max)
		temp_instance.alar_count++;
	temp_instance.mean=0;
  for(; i < 5 ; i++){
		temp_instance.mean += temp_instance.buff[i];
		temp_instance.mean /=2;
	}
}

/*!
    \brief      ADC_Get_Channel
		\param[in]  channel : ch0~chx to convert
    \param[out] none
    \retval     ad value
*/
uint16_t ADC_Get_Channel(uint8_t channel)
{

		 
			/*enable adc software trigger*/
		adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
		
		/*wait until adc convert complete*/
		while(adc_flag_get(ADC_FLAG_EOC)==RESET);
	
		/*Clear adc convert complete flag*/
		adc_flag_clear(ADC_FLAG_EOC);
		
		/*retunr ad value*/
		return(adc_regular_data_read()&0xfff);
}
/* USER CODE BEGIN 4 */
/**
  * @brief ADC Initialization Function
  * @retval None
  */
static void ADC_Init(void)
{
	  /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
	
		/* ADCCLK = PCLK2/6 */
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);
	
	  /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
	
		/* ADC channel length config */
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1);
	
	  /* ADC external trigger source config */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);
	
		  /* ADC regular channel config */
    adc_regular_channel_config(0, ADC_CHANNEL_7, ADC_SAMPLETIME_239POINT5);
	
		/*ADC resolution config*/
		adc_resolution_config(ADC_RESOLUTION_12B);
	
	    /* ADC external trigger enable */
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);

    /* enable ADC interface */
    adc_enable();
		
    /* ADC calibration and reset calibration */
    adc_calibration_enable();
}
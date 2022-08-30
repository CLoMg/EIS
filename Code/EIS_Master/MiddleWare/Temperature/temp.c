
#include "temp.h"
#include "string.h"
#include "stm32wlxx_hal.h"
#include "math.h"



unsigned char  temp_buff_index = 0;
ADC_HandleTypeDef temp_adc;
static Temp_T temp_instance;
void Temp_Init(void)
{
	Temp_IO_Init();
	Temp_Param_Config(&temp_instance);
}

/*adc io channel init*/
static void Temp_IO_Init(void)
{
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
		HAL_ADC_Start(&temp_adc);
		HAL_ADC_PollForConversion(&temp_adc,100);
		if(HAL_IS_BIT_SET(HAL_ADC_GetState(&temp_adc),HAL_ADC_STATE_REG_EOC))
		{
				AD_Value[i] = HAL_ADC_GetValue(&temp_adc);
				AD_Value_Sum += AD_Value[i];
		}
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
		temp_instance.mean += temp_instance.buff[temp_buff_index];
		temp_instance.mean /=2;
	}
}

/* USER CODE BEGIN 4 */
/**
  * @brief ADC Initialization Function
  * @retval None
  */
static void ADC_Init(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
 
	/*Configure the global features of the ADC(Clock.Resolution,Data Alignment and number of conversion)*/
	temp_adc.Instance = ADC;
	temp_adc.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
	temp_adc.Init.Resolution = ADC_RESOLUTION12b;
	temp_adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	temp_adc.Init.ScanConvMode = ADC_SCAN_DISABLE;
	temp_adc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	temp_adc.Init.LowPowerAutoWait = DISABLE;
	temp_adc.Init.LowPowerAutoPowerOff = DISABLE;
	temp_adc.Init.ContinuousConvMode = DISABLE;
	temp_adc.Init.NbrOfConversion = 1;
	temp_adc.Init.DiscontinuousConvMode = DISABLE;
	temp_adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	temp_adc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	temp_adc.Init.DMAContinuousRequests = DISABLE;
	temp_adc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	temp_adc.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_1CYCLE_5;
	temp_adc.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_1CYCLE_5;
	temp_adc.Init.OversamplingMode = DISABLE;
	temp_adc.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
	if(HAL_ADC_Init(&temp_adc)!= HAL_OK)
	{
		//Error_Handler();
	}
	sConfig.Channel = ADC_CHANNEL_8;
	sConfig.Channel = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	if(HAL_ADC_ConfigChannel(&temp_adc,&sConfig)!= HAL_OK )
	{
		//Error_Handler();
	}
	
	HAL_ADCEx_Calibration_Start(&temp_adc);
}
#include "max30105.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "myiic.h"
#include "LORA.h"
#define WAIT_XFER_DONE_OFF
#define WAIT_TWI_DONE_TIME 200
uint32_t Led_data_buff[3];
#define hardwareiic_2 0
uint8_t max_iic_num=2;

//定义设备结构体指针
Device *firstDevice=NULL;

//I2C 初始化 
void twi_max30105_init(void)
{
	#if hardwareiic_2
	I2Cx_Init(I2C2);
	#else
	IIC_Init(max_iic_num);
	#endif
}

//MAX30105 供电引脚控制 旧版本PB9--MAX30105 VCC
void max30105_en_init(void){
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB总线时钟

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //GPIOB6与GPIOB7
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用功能
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //开漏模式
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //无上下拉
//	
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_SetBits(GPIOB,GPIO_Pin_9);
}

//max30105   断电重启
void max30105_Power_Reset(void){
	Max30105_Power=0;
	delay_ms(1000);
	Max30105_Power=1;
	delay_ms(1000);
}


//i2c单字节写
static Error_Code_t write_reg_8(uint8_t slaveaddr, uint8_t reg, uint8_t data)
{
		Error_Code_t err_code;
		#if hardwareiic_2
		err_code=I2Cx_Write_Single(I2C2,slaveaddr,reg,&data);
	  #else
		err_code=I2C_Write_Single(max_iic_num,slaveaddr,reg,&data);
		#endif
	
    return err_code;
}

//I2C单字节读
static Error_Code_t read_reg(uint8_t slaveaddr, uint8_t reg, uint8_t *data)
{

    Error_Code_t err_code;
    #if hardwareiic_2
		err_code=I2Cx_Read_Single(I2C2,slaveaddr,reg,data);
		#else
		err_code=I2C_Read_Single(max_iic_num,slaveaddr,reg,data);
		#endif
    return err_code;
}

//Max30105 InterruptSetting 初始化
static Error_Code_t set_int_reg(uint8_t slaveaddr, InterruptSettings *intSettings)
{

    Error_Code_t err_code;
    uint8_t data;

    data = 0xF0 & ((intSettings->almostFullFIFO << 7) | (intSettings->newFIFOdata << 6) | (intSettings->ambientOverflow << 5) | (intSettings->proximityThReached << 4));
    err_code = write_reg_8(slaveaddr, INT_ENABLE1, data);
    data = 0x02 & (intSettings->dieTempRdy << 1);
    err_code = write_reg_8(slaveaddr, INT_ENABLE2, data);

    return err_code;
}
////Max30105 FIFO  设置 0x05--0x01--0x00
static Error_Code_t set_fifo_reg(uint8_t slaveaddr, FIFOSettings *fifoSettings)
{

    Error_Code_t err_code;
    uint8_t data;

    data = 0xFF & ((fifoSettings->sampleAverage << 5) | (fifoSettings->rollOver << 4) | fifoSettings->fifoAlmostFull);
    err_code = write_reg_8(slaveaddr, CONF_FIFO, data);

    return err_code;
}

//Max30105 Register 0x0A~0x12 设置 
static Error_Code_t set_op_reg(uint8_t slaveaddr, OperationalSettings *opSettings)
{

    Error_Code_t err_code;
    uint8_t data;

    data = 0x7F & ((opSettings->adcRange << 5) | (opSettings->sampleRate << 2) | (opSettings->ledPulseWidth));
    err_code = write_reg_8(slaveaddr, CONF_SPO2, data);

    data = opSettings->ledRedAmp;
    err_code = write_reg_8(slaveaddr, CONF_LED_PULSE_RED, data);
    data = opSettings->ledIrAmp;
    err_code = write_reg_8(slaveaddr, CONF_LED_PULSE_IR, data);
    data = opSettings->ledGreenAmp;
    err_code = write_reg_8(slaveaddr, CONF_LED_PULSE_GREEN, data);
    data = opSettings->ledPilotAmp;
    err_code = write_reg_8(slaveaddr, CONF_LED_PULSE_PILOT, data);

    data = 0x77 & ((opSettings->timeSlot2 << 4) | opSettings->timeSlot1);
    err_code = write_reg_8(slaveaddr, CONF_SLOT1, data);
    data = 0x77 & ((opSettings->timeSlot4 << 4) | opSettings->timeSlot3);
    err_code = write_reg_8(slaveaddr, CONF_SLOT2, data);

    return err_code;
}

//Max30105 Register 0x09--Mode 0x30--Proximity Interrupt Threshold设置 
static Error_Code_t set_dev_reg(uint8_t slaveaddr, Device *device)
{

    Error_Code_t err_code;
    uint8_t data;

    data = 0x07 & (device->mode);
    err_code = write_reg_8(slaveaddr, CONF_MODE, data);
    data = 0xFF & (device->proximity);
    err_code = write_reg_8(slaveaddr, PROXIMITY_INT_TH, data);

    return err_code;
}

//Max30105 Interrupt 参数设置对应Register 0x00~0x01
extern InterruptSettings *max30105_setup_interrupt(bool almostFullFIFO, bool newFIFOdata, bool ambientOverflow, bool proximityThReached, bool dieTempRdy)
{

    InterruptSettings *intSettings = malloc(sizeof(InterruptSettings));
    intSettings->almostFullFIFO = almostFullFIFO;
    intSettings->newFIFOdata = newFIFOdata;
    intSettings->ambientOverflow = ambientOverflow;
    intSettings->proximityThReached = proximityThReached;
    intSettings->dieTempRdy = dieTempRdy;

    return intSettings;
}

//Max30105 FIFO 参数设置对应Register 0x08
extern FIFOSettings *max30105_setup_fifo(uint8_t sampleAverage, bool rollOver, uint8_t fifoAlmostFull)
{

    FIFOSettings *fifoSettings = malloc(sizeof(FIFOSettings));
    fifoSettings->sampleAverage = sampleAverage;
    fifoSettings->rollOver = rollOver;
    fifoSettings->fifoAlmostFull = fifoAlmostFull;

    return fifoSettings;
}

//Max30105 operational 参数设置对应Register 0x0A--ADCRange/SampleRate/Led_PulseWidth 0x0C~0x0E--LED_Pulse_Amplitude  0x10--Proximity_Mode_LED_Pulse_Amplitude 0x11~0x12 Muti_LED_Mode_Time_Slots
extern OperationalSettings *max30105_setup_operational(uint8_t adcRange, uint8_t sampleRate, uint8_t ledPulseWidth, uint8_t ledRedAmp, uint8_t ledIrAmp, uint8_t ledGreenAmp, uint8_t ledPilotAmp, uint8_t timeSlot1, uint8_t timeSlot2, uint8_t timeSlot3, uint8_t timeSlot4)
{

    OperationalSettings *opSettings = malloc(sizeof(OperationalSettings));
    opSettings->adcRange = adcRange;
    opSettings->sampleRate = sampleRate;
    opSettings->ledPulseWidth = ledPulseWidth;
    opSettings->ledRedAmp = ledRedAmp;
    opSettings->ledIrAmp = ledIrAmp;
    opSettings->ledGreenAmp = ledGreenAmp;
    opSettings->ledPilotAmp = ledPilotAmp;
    opSettings->timeSlot1 = timeSlot1;
    opSettings->timeSlot2 = timeSlot2;
    opSettings->timeSlot3 = timeSlot3;
    opSettings->timeSlot4 = timeSlot4;

    return opSettings;
}

//Max30105设备参数设置
extern Device *max30105_setup_device(uint8_t mode, uint8_t proximity, InterruptSettings *intSettings, FIFOSettings *fifoSettings, OperationalSettings *opSettings, MeasurementBuffer *meaBuf)
{

    Device *device = malloc(sizeof(Device));
    device->mode = mode;
    device->proximity = proximity;
    device->intSettings = intSettings;
    device->fifoSettings = fifoSettings;
    device->opSettings = opSettings;
    device->meaBuf = meaBuf;

    return device;
}

//Max30105 测量数据缓冲区初始化
extern MeasurementBuffer *max30105_setup_measurement_buffer(void)
{

    MeasurementBuffer *meaBuff = malloc(sizeof(MeasurementBuffer));
    meaBuff->head = 0;
    meaBuff->tail = 0;

    return meaBuff;
}

//Max30105设备初始化
extern Error_Code_t max30105_configure_device(uint8_t slaveaddr, Device *device)
{
    Error_Code_t err_code;

    CHECK_ERROR(set_int_reg(slaveaddr, device->intSettings));

    err_code = set_fifo_reg(slaveaddr, device->fifoSettings);
    CHECK_ERROR(err_code);

    err_code = set_op_reg(slaveaddr, device->opSettings);
    CHECK_ERROR(err_code);

    err_code = set_dev_reg(slaveaddr, device);
    CHECK_ERROR(err_code);

    return err_code;
}

//Max30105 寄存器值输出
extern Error_Code_t max30105_print_reg_map(uint8_t slaveaddr)
{

    Error_Code_t err_code;
    uint8_t reg[1] = {0};
    uint8_t data[1] = {0};


//    printf("\n\n\rMAP:");
    for(uint16_t i = 0; i <= 0xFF; i++)
    {
        if((i >= 0x31 && i <= 0xFD) || (i >= 0x13 && i <= 0x1E) || (i >= 0x22 && i <= 0x2F) || i == 0x0B || i == 0x0F)
            continue;

        reg[0] = i;
				read_reg(slaveaddr,reg[0],data);
        delay_us(WAIT_TWI_DONE_TIME);

        if((i % 4) == 0)
            printf("\n\r");
        printf("%02x: %02x\t\t", i, data[0]);
    }
    return err_code;
}

#define DIE_TEMP_RDY_STATUS 0X02
//Max30105 温度读取
extern Error_Code_t max30105_read_temperature(uint8_t slaveaddr, float *data)
{
    Error_Code_t err_code;
    err_code = write_reg_8(slaveaddr, TEMP_CONFIG, 0x01);
    CHECK_ERROR(err_code);
    uint8_t response;

    while(1)
    {
        err_code = read_reg(slaveaddr, TEMP_CONFIG, &response);
        CHECK_ERROR(err_code);

        if ((response & 0x01) == 0) break;
        delay_ms(1);
    }

    while(1)
    {
        err_code = read_reg(slaveaddr, INT_STATUS2, &response);
				CHECK_ERROR(err_code);

    	if ((response & DIE_TEMP_RDY_STATUS) == 0) 
    	{
//    		printf("temp ready+++:%02X\r\n", response);
    		break;
    	}
    	else
    	{
//    		printf("not ready---:%02X\r\n", response);
    	}
        delay_ms(1);
    }

    err_code = read_reg(slaveaddr, TEMP_INTEGER, &response);
    CHECK_ERROR(err_code);
    int8_t tempInt = (int8_t)response;
    //printf("tempInt =%d\n", tempInt);
    err_code = read_reg(slaveaddr, TEMP_FRACTION, &response);
    CHECK_ERROR(err_code);
    uint8_t tempFrac = response;
    float *result_p = data;
    *result_p = (float)tempInt + ((float)tempFrac * 0.0625f);

    return err_code;
}

//Max30105 FIFO_Data读取
extern Error_Code_t max30105_read_fifo(uint8_t slaveaddr, Device *device)
{

    Error_Code_t err_code;
    uint8_t rdPtr;
    uint8_t wrPtr;
    uint8_t numOfChannels;
    uint32_t adcResolution;

    err_code = read_reg(slaveaddr, FIFO_RD_PTR, &rdPtr);
    CHECK_ERROR(err_code);
    err_code = read_reg(slaveaddr, FIFO_WR_PTR, &wrPtr);
    CHECK_ERROR(err_code);

    if(rdPtr != wrPtr)
    {
        if(device->mode == MODE_RED)
            numOfChannels = 1;
        else if(device->mode == MODE_RED_IR)
            numOfChannels = 2;
        else if(device->mode == MODE_RED_IR_GREEN)
            numOfChannels = 3;
        else
            numOfChannels = 0;

        if(device->opSettings->ledPulseWidth == LED_PULSE_WIDTH_411)
            adcResolution = 0x3FFFF;
        else if(device->opSettings->ledPulseWidth == LED_PULSE_WIDTH_215)
            adcResolution = 0x1FFFF;
        else if(device->opSettings->ledPulseWidth == LED_PULSE_WIDTH_118)
            adcResolution = 0xFFFF;
        else
            adcResolution = 0x7FFF;

        int8_t numOfSamples = wrPtr - rdPtr;
        if(numOfSamples < 0)
            numOfSamples += 32;
        uint8_t bytesToRead = numOfSamples * numOfChannels * 3;
        //uint8_t bytesToRead = numOfSamples * numOfChannels ;
        //printf("FIFO:%d--%d\n", numOfSamples, bytesToRead);

        uint8_t reg_p = FIFO_DATA;
//				I2C1_Read_Single(slaveaddr,reg_p,&reg_p);
				uint8_t result[bytesToRead];
				#if hardwareiic_2
				err_code = I2Cx_Read_Multiple(I2C2,slaveaddr,reg_p,result,bytesToRead);
				#else
				err_code = I2C_Read_Multiple(max_iic_num,slaveaddr,reg_p,result,bytesToRead);				
				#endif
				CHECK_ERROR(err_code);

//#ifdef WAIT_XFER_DONE
//			while (m_xfer_done == false) {}
//#else
//			nrf_delay_us(WAIT_TWI_DONE_TIME);
//#endif

//			if (NRF_SUCCESS != err_code)
//			{
//					return false;
//			}
//			CHECK_ERROR(err_code);
				delay_us(WAIT_TWI_DONE_TIME);
				
        uint32_t tempLong;
        uint8_t temp[4];

        for(int i = 0; i < numOfSamples; i++)
        {
            device->meaBuf->head++;
            device->meaBuf->head %= 32;
            for(int k = 0; k < numOfChannels; k++)
            {

                temp[3] = 0;
                temp[2] = result[(i*3*numOfChannels)+(k*3)];
                temp[1] = result[(i*3*numOfChannels)+(k*3)+1];
                temp[0] = result[(i*3*numOfChannels)+(k*3)+2];

                memcpy(&tempLong, temp, sizeof(tempLong));
                tempLong &= adcResolution;

                if(k == 0)
                    device->meaBuf->redLed[device->meaBuf->head] = tempLong;
                if(k == 1)
                    device->meaBuf->irLed[device->meaBuf->head] = tempLong;
                if(k == 2)
                    device->meaBuf->greenLed[device->meaBuf->head] = tempLong;
            }
        }
    }
    return err_code;
}

//Max30105 Sleep Mode
extern Error_Code_t max30105_sleep(nrf_drv_twi_t slaveaddr, Device *device)
{

    Error_Code_t err_code;

    uint8_t sleep = device->mode | 0x80;
    uint8_t reg_p[2] = {CONF_MODE, sleep};
		err_code = write_reg_8(slaveaddr,reg_p[0],reg_p[1]);
		CHECK_ERROR(err_code);
		delay_ms(10);
		err_code = read_reg(slaveaddr,reg_p[0],&reg_p[1]);
		CHECK_ERROR(err_code);
		if((0x80&reg_p[1])!=0x80) 
			err_code=0;
		
    return err_code;
}

////Max30105 Wake From Sleep Mode
extern Error_Code_t max30105_wake_up(nrf_drv_twi_t slaveaddr, Device *device)
{

    Error_Code_t err_code;

    uint8_t wakeUp = device->mode & 0x07;
    uint8_t reg_p[2] = {CONF_MODE, wakeUp};

		write_reg_8(slaveaddr,reg_p[0],reg_p[1]);
	  delay_ms(10);
		err_code = read_reg(slaveaddr,reg_p[0],&reg_p[1]);
		CHECK_ERROR(err_code);
		if((0x80&reg_p[1])!=0x00) 
			err_code=0;
		
    return err_code;
}

//Max30105 reset
extern Error_Code_t max30105_reset(nrf_drv_twi_t slaveaddr)
{

    Error_Code_t err_code;
    uint8_t reset = 0x40;

    uint8_t reg_p[2] = {CONF_MODE, reset};

		err_code = write_reg_8(slaveaddr,reg_p[0],reg_p[1]);
		CHECK_ERROR(err_code);

    uint8_t reg_p1[2] = {FIFO_WR_PTR, 0x00};
		err_code = write_reg_8(slaveaddr,reg_p1[0],reg_p1[1]);
		CHECK_ERROR(err_code);

    uint8_t reg_p2[2] = {FIFO_RD_PTR, 0x00};
 		err_code = write_reg_8(slaveaddr,reg_p2[0],reg_p2[1]);
		CHECK_ERROR(err_code);

    return err_code;
}

#define A_FULL_EN			0
#define DATA_RDY_EN			0
#define ALC_OVF_EN			0
#define PROX_INT_EN			0
#define DIE_TEMP_RDY_EN		1

//Max30105 设备初始化
Device *init_device(uint8_t mode, uint8_t proxy)
{
	//clear all interrupt flag when initiation
    InterruptSettings *intSettings = max30105_setup_interrupt(A_FULL_EN, DATA_RDY_EN, ALC_OVF_EN, PROX_INT_EN, DIE_TEMP_RDY_EN);
    //FIFO setting para: 
    //average=32
    //32 samples data full then triger FIFO_FULL interrupt
    //FIFO full then fill new data
    FIFOSettings *fifoSettings = max30105_setup_fifo(SAMPLE_AVG_32, FIFO_ROLLOVER_ON, FIFO_A_FULL_OFF);
    OperationalSettings *opSettings = max30105_setup_operational( ADC_RANGE_MIN, SAMPLE_RATE_1000, LED_PULSE_WIDTH_411,
                                      LED_PULSE_AMP_LOW, LED_PULSE_AMP_LOW, LED_PULSE_AMP_LOW, LED_PULSE_AMP_MEDIUM,
                                      LED_SLOT_RED_PILOT, LED_SLOT_IR_PILOT, LED_SLOT_GREEN_PILOT, LED_SLOT_OFF);

    MeasurementBuffer *meaBuf = max30105_setup_measurement_buffer();

    Device *device = max30105_setup_device(mode, proxy, intSettings, fifoSettings, opSettings, meaBuf);

    return device;
}

//Max30105 采集count次 Sample
uint32_t * measurement_circle(nrf_drv_twi_t slaveaddr, Device *dev, uint32_t count, uint8_t sampleAvg, uint8_t adcRange, uint8_t sampleRate, uint8_t pulseWidth, uint8_t amplitude)
{
	  Error_Code_t err_code=0;
			//float redled;
		uint64_t redLed=0,irLed=0,greenLed=0;
    Device *device = dev;
    device->fifoSettings->sampleAverage = sampleAvg;
    device->opSettings->adcRange = adcRange;
    device->opSettings->sampleRate = sampleRate;
    device->opSettings->ledPulseWidth = pulseWidth;
    device->opSettings->ledPilotAmp = amplitude;
	  static uint8_t error_times=0;
    //max30105_reset(instance);
    //nrf_delay_ms(20);
    
    err_code=max30105_configure_device(slaveaddr, device);
		if(!err_code){
			Led_data_buff[0]=0xffffffff;
			Led_data_buff[1]=0xffffffff;
			Led_data_buff[2]=0xffffffff;
			error_times++;
			if(error_times>10){
				max30105_Power_Reset();
				error_times=0;
			}
			return Led_data_buff;
		}
    delay_ms(50);

    uint32_t i = 0;
		uint16_t cnt=0;
		uint8_t success_times=0;
    //printf("\tRED\tIR\tGRE\r\n");
	
			while(i < count)
			{
					cnt++;
					uint16_t times=0x20;
//				while(!max30105_read_fifo(slaveaddr, device));
					while((!max30105_read_fifo(slaveaddr, device))&&(times>0))
					{
						times--;
						delay_ms(10);
					}

					if(device->meaBuf->tail != device->meaBuf->head)
					{
							redLed+=device->meaBuf->redLed[device->meaBuf->tail];
							irLed+=device->meaBuf->irLed[device->meaBuf->tail];
							greenLed+=device->meaBuf->greenLed[device->meaBuf->tail];
							printf("%d->%d\t%d\t%d\t%d\r\n", (i+1),cnt, device->meaBuf->redLed[device->meaBuf->tail], device->meaBuf->irLed[device->meaBuf->tail], device->meaBuf->greenLed[device->meaBuf->tail]);
							//printf("%d ", device->meaBuf->irLed[device->meaBuf->tail]);
							delay_us(50);
							device->meaBuf->tail++;
							device->meaBuf->tail %= 32;
							i++;
							success_times++;
							cnt=0;
					}

					else{
						if(cnt>300){
							cnt=0;
							i++;
						}
					}
					delay_ms(300);
			}
		if(success_times==0)
			success_times=1;
		redLed/=success_times;
		Led_data_buff[0]=(uint32_t)redLed;
		irLed/=success_times;
		Led_data_buff[1]=(uint32_t)irLed;
		greenLed/=success_times;
		Led_data_buff[2]=(uint32_t)greenLed;
		printf("average->\t%d\t%d\t%d\r\n", (uint32_t)redLed,(uint32_t)irLed,(uint32_t)greenLed);
		//redled=(float)redLed;
		return Led_data_buff;
}

//Max30105 读取设备ID
Error_Code_t max30105_read_id(void)
{
    Error_Code_t err_code;
    uint8_t chip_id;
    err_code = read_reg(Slave, DEV_PART_ID, &chip_id);
		CHECK_ERROR(err_code);
    printf("read reg:%02X, chip_id=%02X\n", DEV_PART_ID, chip_id);
    return err_code;
		
}

//Max30105测试--采集16个Sample
void max30105_test(void)
{
//    float temperature;
    //max30105_read_temperature(m_twi, &temperature);
    //printf("TEMPERATURE: %f\r\n", temperature);
    measurement_circle(Slave, firstDevice, 10, SAMPLE_AVG_32, ADC_RANGE_MIN, SAMPLE_RATE_100, LED_PULSE_WIDTH_69, LED_PULSE_AMP_LOW);
}

//Max30105测试--采集16个Sample
Error_Code_t max30105_Acquire(uint8_t *data_buff)
{ 
	  Error_Code_t err_code=0;

		uint16_t visual_grade;
    measurement_circle(Slave, firstDevice, 5, SAMPLE_AVG_32, ADC_RANGE_MIN, SAMPLE_RATE_100, LED_PULSE_WIDTH_69, LED_PULSE_AMP_LOW);
	 	if(Led_data_buff[0]==0xFFFFFFFF)
			visual_grade=0xFFFF;
		else{
			visual_grade=Led_data_buff[0]/500;
			err_code=1;
		}
	  
		*(data_buff++)=(uint8_t)(visual_grade>>8);
		*(data_buff)=(uint8_t)(visual_grade);

		return err_code;
}

//#define PROXIMITY_MODE	0X0F
#define PROXIMITY_MODE	0XAA

//Max30105上电初始化
void max30105_init(void)
{
		uint8_t dummy_buff[2],i=0;
		char Init_Info[]="IIC_2 MAX30105 Init Sucess\r\n";
		Error_Code_t err_code;
	  max30105_en_init();
	  max30105_Power_Reset();
    twi_max30105_init();
		delay_ms(10);

    firstDevice = init_device(MODE_RED_IR_GREEN, PROXIMITY_MODE);
   // printf("\r\n<><><><><>max30105 Start!<><><><><>\r\n");

    err_code=max30105_reset(Slave);
    delay_ms(20);
    //printf("\r\n<><><><><>max30105 reset!<><><><><>\r\n");

    err_code=max30105_configure_device(Slave, firstDevice);
    delay_ms(20);
    printf("\r\n<><><><><>max30105 initialize success!<><><><><>\r\n");
	
		if(!max30105_Acquire(dummy_buff))
			memcpy(Init_Info,"IIC_2 MAX30105 Init Failed\r\n",sizeof("IIC_2 MAX30105 Init Failed\r\n"));
		
		while(Init_Info[i++]!='\0'){
			USART_SendData(USART3,Init_Info[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
	}	
		LORA_send_data(Init_Info,sizeof(Init_Info));
	 
}

//Max30105重新初始化
void max30105_reinit(void)
{
    //twi_max30105_init();

    firstDevice = init_device(MODE_RED_IR, PROXIMITY_MODE);
    delay_ms(20);

    max30105_reset(Slave);
    delay_ms(20);
    
    max30105_configure_device(Slave, firstDevice);
    delay_ms(20);
    printf("max30105 reInitiate!\r\n");
}

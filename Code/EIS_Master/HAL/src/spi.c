
#include "gd32e230_spi.h"
#include "spi.h"
#include "dsl_08.h"
#include "ws_gas.h"
#include "temp.h"
#include "ec_tb600.h"
#include "string.h"

uint8_t spi1_receive_array[20];
uint8_t spi1_transmit_array[20] = {0x21,0x52,0x04,0x00,};
uint32_t receive_n;
/*!
    \brief      init SPI1
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void spi1_init(void)
{
    spi_parameter_struct spi_init_struct;
		

    /* enable the gpio clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI1);

    /* GPIOB config, PB13(SPI1_SCK), PB14(SPI1_MISO), PB15(LCD_SPI1_MOSI) */
    gpio_af_set(GPIOB, GPIO_AF_0, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

    /* SPI1 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_SLAVE;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_HARD;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_init_struct);

    /* set crc polynomial */
    spi_enable(SPI1);
		
		spi_fifo_access_size_config(SPI1,SPI_BYTE_ACCESS);
		
		nvic_irq_enable(SPI1_IRQn,3);
		spi_i2s_interrupt_enable(SPI1,SPI_I2S_INT_RBNE);
		
}

void SPI1_IRQHandler(void)
{	
	unsigned char rec,len=0;
	if(spi_i2s_interrupt_flag_get(SPI1,SPI_I2S_INT_FLAG_RBNE) != RESET){
		rec = spi_i2s_data_receive(SPI1);
		if((receive_n==0)&&(rec==0x21))
			spi1_receive_array[receive_n++] = rec;
		if(receive_n == 1){
			if((rec == 0x57)||(rec == 0x52))
				spi1_receive_array[receive_n++] = rec;
			else
				receive_n=0;
		}
		
		if(receive_n ==2){
			spi1_receive_array[receive_n++] = rec;
			len = rec;
		}
		if((receive_n>2)&&(receive_n<len-3))
			spi1_receive_array[receive_n++] = rec;
		else
			receive_n = 0;
		if((receive_n==4)&&(spi1_receive_array[1] = 0x52))
			if(memcmp(spi1_receive_array,spi1_transmit_array,4)!=0)
				memset(spi1_transmit_array,0xff,20);
		if(receive_n>=4)
			spi_i2s_data_transmit(SPI1,spi1_transmit_array[receive_n]);
		
	}
}

void SPI_Process(void){
	unsigned char lrc_calc,tx_index = 4;
	if(receive_n>5){
		//lrc_calc = LRC_Value_Calc(spi1_receive_array,spi1_receive_array[2]);
		if(LRC_Value_Calc(spi1_receive_array,spi1_receive_array[2]) == spi1_receive_array[receive_n-1]){
			if((spi1_receive_array[1] == 0x57)&&(spi1_receive_array[3] == 0x00)){
				spi1_transmit_array[tx_index++] = (uint8_t)(dsl_instance->Mean>>8);
				spi1_transmit_array[tx_index++] = (uint8_t)dsl_instance->Mean;
				spi1_transmit_array[tx_index++] = (uint8_t)(CO2_instance->Mean>>8);
				spi1_transmit_array[tx_index++]  =(uint8_t)CO2_instance->Mean;
				spi1_transmit_array[tx_index++]  = (uint8_t)(temp_instance.mean>>8);
				spi1_transmit_array[tx_index++]  = (uint8_t)temp_instance.mean;
				spi1_transmit_array[tx_index++]  = (uint8_t)(instance_co.mean>>8);
			  spi1_transmit_array[tx_index++]  = (uint8_t)instance_co.mean;
				spi1_transmit_array[tx_index]  = LRC_Value_Calc(spi1_transmit_array,tx_index);
			}
		}
	}
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
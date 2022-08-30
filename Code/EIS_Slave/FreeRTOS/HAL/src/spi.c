
#include "gd32e230_spi.h"
#include "spi.h"
#include "dsl_08.h"
#include "ws_gas.h"
#include "temp.h"
#include "ec_tb600.h"
#include "string.h"
#include "temp.h"
#include "scd4x_i2c.h"
#include "dsl_08.h"

uint8_t cpm_array[20] ={0x52,0xAB,};
uint8_t spi1_receive_array[20];
uint8_t data_rec[20];
uint8_t data_rec_n = 0;
uint8_t spi1_transmit_array[20] = {0xFF,};
uint8_t data_tx[20];
uint32_t receive_n;

/*!
    \brief      init SPI1
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi1_init(void)
{
    spi_parameter_struct spi_init_struct;
		 
    /* enable the gpio clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_SPI1);

    /* GPIOB config, PB13(SPI1_SCK), PB14(SPI1_MISO), PB15(LCD_SPI1_MOSI) */
    gpio_af_set(GPIOB, GPIO_AF_0, GPIO_PIN_12|GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);
		
		gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_1);
		
    spi_i2s_deinit(SPI1);
		spi_struct_para_init(&spi_init_struct);
    /* SPI1 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_SLAVE;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_init_struct);

    /* set crc polynomial */
    spi_enable(SPI1);
		
		spi_fifo_access_size_config(SPI1,SPI_BYTE_ACCESS);
		
		nvic_irq_enable(SPI1_IRQn,1);
		spi_i2s_interrupt_enable(SPI1,SPI_I2S_INT_RBNE);
		
		memset(spi1_transmit_array,0xFF,20);
		spi_i2s_data_transmit(SPI1, 0x4A);
		spi_i2s_data_transmit(SPI1, 0x54);
}

void SPI1_IRQHandler(void)
{	
	unsigned char rec,len ,cnt=0;
	unsigned char *p;
	if(spi_i2s_interrupt_flag_get(SPI1,SPI_I2S_INT_FLAG_RBNE) != RESET){
//		rec = spi_i2s_data_receive(SPI1);
//		spi1_receive_array[receive_n++] = rec;
//		if((spi1_receive_array[0]!= 0x52)&&(spi1_receive_array[0]!= 0x57))
//			receive_n = 0;

//				
//		if((spi1_receive_array[receive_n-2] == 0x0D ) &&(spi1_receive_array[receive_n-1] == 0x0A)){
//			memcpy(data_rec,spi1_receive_array,receive_n);
//			memset(spi1_receive_array,0xFF,receive_n);
//			data_rec_n = receive_n;
//			receive_n = 0;
//		}
//		else{
//			if(spi1_receive_array[0] == 0x52)
//				spi_i2s_data_transmit(SPI1, spi1_transmit_array[receive_n]);
//			else 
//				spi_i2s_data_transmit(SPI1,0xFF);
//		}
		rec = spi_i2s_data_receive(SPI1);
//		if(cnt == 0){
//			if(rec == 0xFF)
//				spi1_receive_array[cnt++] = rec;	
//		}
//		else{
//			if(cnt == 1){
//				if((rec == 0x57)||(rec == 0x52))
//					spi1_receive_array[cnt++] = rec;
//				else
//					cnt =0;
//			}
//			else{
//				if(spi1_receive_array[1] == 0x52){
//					if(cnt == 2){
//						if(rec == spi1_transmit_array[2]){
//							spi_i2s_data_transmit(SPI1,spi1_transmit_array[cnt+1]);
//							spi1_receive_array[cnt++] = rec;
//						}
//						else
//							cnt = 0;
//					}
//					else{
//						spi_i2s_data_transmit(SPI1,spi1_transmit_array[cnt+1]);
//						spi1_receive_array[cnt++] = rec;
//					}
//				
//				}
//				else{
//					if(spi1_receive_array[1] == 0x57){
//						if(cnt == 4){
//							len = (uint16_t)spi1_receive_array[3]<<8 | rec;
//							spi1_receive_array[cnt++] = rec;
//						}
//						else{
//							if(cnt < len+6)
//								spi1_receive_array[cnt++] = rec;
//							else if(cnt == len+6){
//								spi1_receive_array[cnt] = rec;
//								memcpy(data_rec,spi1_receive_array,cnt);
//								data_rec_n = cnt;
//								cnt = 0;
//							}
//						}
//					}
//					else
//						cnt = 0;
//				}
//			}
//		}
	if((data_rec_n==0)&&((rec == 0x52)||(rec == 0x57)))
		spi1_receive_array[data_rec_n] = rec;
	else {
		if(spi1_receive_array[0] == 0x52){
			if(data_rec_n ==1){
				if(rec < REG_NUM)
					memcpy(&spi1_transmit_array[1],data_tx,REG_NUM - rec);
			}
			
		}
		else
		{
			spi1_receive_array[data_rec_n] = rec;
		}
	}
		data_rec_n++;
		spi_i2s_data_transmit(SPI1,spi1_transmit_array[data_rec_n]);
		if(data_rec_n > 9)
			data_rec_n = 0;
	}
}

void SPI_Process(void){
	unsigned char lrc_calc,tx_index = 0;
//	if(data_rec_n>7){
//		data_rec_n =0;
//	}
	data_tx[0] = (uint8_t)(temp_instance.mean>>8);
	data_tx[1] = (uint8_t)temp_instance.mean;
	
	data_tx[2] = (uint8_t)(dsl_instance->Mean>>8);
	data_tx[3] = (uint8_t)dsl_instance->Mean;
	
	data_tx[4] = (uint8_t)(scd_co2_instance->mean>>8);
	data_tx[5] = (uint8_t)scd_co2_instance->mean;
	
	data_tx[6] = 0;
	data_tx[7] = LRC_Value_Calc(data_tx,7);
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
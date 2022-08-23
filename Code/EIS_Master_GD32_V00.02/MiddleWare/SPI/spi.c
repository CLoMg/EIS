#include "spi.h"
#include "gd32f10x_spi.h"
#include "gd32f10x_eval.h"
#include "timer.h"

#define Spi_Buff_Size 255
uint8_t spi1_rx_buff[Spi_Buff_Size]={0,};

void SPI_Init(void){
	#if defined SPI_HARD
	spi_gpio_init();
	spi_config();
	spi_dma_config();
	#elif defined SPI_SOFT
		rcu_periph_clock_enable(SPI_GPIO_CLK);
		rcu_periph_clock_enable(SLAVE_RESET_GPIO_CLK);
		
		gpio_init(SPI1_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPI1_MOSI_Pin);
		gpio_init(SPI1_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPI1_SCLK_Pin);
		gpio_init(SPI1_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPI1_CS_Pin);
		gpio_init(SLAVE_RESET_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SLAVE_RESET_Pin);
	
		gpio_init(SPI1_GPIO_Port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, SPI1_MISO_Pin);
	
		
		GPIO_BOP(SPI1_GPIO_Port)  = SPI1_CS_Pin|SPI1_SCLK_Pin|SPI1_MOSI_Pin;
		Slave_Reset();
	#endif
}

void Slave_Reset(void){
		GPIO_BOP(SLAVE_RESET_GPIO_Port)  = SLAVE_RESET_Pin;
		delay_us(500);
		GPIO_BC(SLAVE_RESET_GPIO_Port)  = SLAVE_RESET_Pin;
		delay_us(500);
		GPIO_BOP(SLAVE_RESET_GPIO_Port)  = SLAVE_RESET_Pin;
}
#if defined SPI_HARD
void spi_config(void)
{
	spi_parameter_struct spi_init_struct;
	
	rcu_periph_clock_enable(RCU_SPI1);
	spi_i2s_deinit(SPI1);
	spi_struct_para_init(&spi_init_struct);
	
	spi_init_struct.trans_mode  = SPI_TRANSMODE_FULLDUPLEX;
	spi_init_struct.device_mode = SPI_MASTER;
	spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
	spi_init_struct.nss	= SPI_NSS_SOFT;
	spi_init_struct.prescale = SPI_PSC_8;
	spi_init_struct.endian = SPI_ENDIAN_MSB;
	
	spi_init(SPI1,&spi_init_struct);
}


void spi_dma_config(void)
{
	dma_parameter_struct dma_init_struct;
	dma_struct_para_init(&dma_init_struct);
	
	rcu_periph_clock_enable(RCU_DMA0);
	dma_deinit(DMA0,DMA_CH4);
	dma_init_struct.periph_addr = (uint32_t)&(SPI_DATA(SPI1));
	dma_init_struct.memory_addr = (uint32_t)spi1_rx_buff;
	dma_init_struct.direction  = DMA_MEMORY_TO_PERIPHERAL;
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
	dma_init_struct.priority = DMA_PRIORITY_HIGH;
	dma_init_struct.number = Spi_Buff_Size;
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init(DMA0,DMA_CH4, &dma_init_struct);
	
	dma_deinit(DMA0,DMA_CH3);
	dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
	dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
	dma_init(DMA0,DMA_CH3,&dma_init_struct);
}

unsigned char SPI_ReadWriteByte(unsigned char TxData)
{
		spi_ss_control(0);
	
		SPI_CTL1(SPI1) |= (uint32_t)SPI_CTL1_DMATEN;
		SPI_CTL1(SPI1) |= (uint32_t)SPI_CTL1_DMAREN;
	
		DMA_CHCTL(DMA0,DMA_CH4) &= ~DMA_CHXCTL_CHEN;
		dma_memory_address_config(DMA0,DMA_CH4,(uint32_t)&TxData);
		DMA_CHCNT(DMA0,DMA_CH4) = 1;
		DMA_CHCTL(DMA0,DMA_CH4) |= DMA_CHXCTL_CHEN;
	
		dma_channel_disable(DMA0,DMA_CH3);
		dma_transfer_number_config(DMA0,DMA_CH3,1);
		dma_channel_enable(DMA0,DMA_CH3);
		
		while(RESET == dma_flag_get(DMA0,DMA_CH4,DMA_FLAG_FTF));
		//while(RESET == dma_flag_get(DMA0,DMA_CH3,DMA_FLAG_FTF));
		
		spi_ss_control(1);
		return (spi1_rx_buff[0]);
}

#elif defined SPI_SOFT
/* CPOL = 0, CPHA = 0, MSB first */
uint8_t SOFT_SPI_RW_MODE0( uint8_t write_dat )
{
    uint8_t i, read_dat;
    for( i = 0; i < 8; i++ )
    {
        if( write_dat & 0x80 )
            MOSI_H;  
        else                    
            MOSI_L;  
        write_dat <<= 1;
        delay_us(1);	
        SCK_H; 
        read_dat <<= 1;  
        if( MISO == 1 ) 
            read_dat++; 
		delay_us(1);
        SCK_L; 
        __nop();
    }
	
    return read_dat;
}
 
 
/* CPOL=0，CPHA=1, MSB first */
uint8_t SOFT_SPI_RW_MODE1(uint8_t byte) 
{
    uint8_t i,Temp=0;
 
	for(i=0;i<8;i++)     // 循环8次
	{
		SCK_H;     //拉高时钟
		if(byte&0x80)
        {
			MOSI_H;  //若最到位为高，则输出高
        }
		else      
		{
			MOSI_L;   //若最到位为低，则输出低
		}
		byte <<= 1;     // 低一位移位到最高位
		delay_us(1);
		SCK_L;     //拉低时钟
		Temp <<= 1;     //数据左移
 
		if(MISO == 1)
			Temp++;     //若从从机接收到高电平，数据自加一
		delay_us(1);
 
	}
	return (Temp);     //返回数据
}
 
/* CPOL=1，CPHA=0, MSB first */
uint8_t SOFT_SPI_RW_MODE2(uint8_t byte) 
{
    uint8_t i,Temp=0;
 
	for(i=0;i<8;i++)     // 循环8次
	{
		if(byte&0x80)
        {
			MOSI_H;  //若最到位为高，则输出高
        }
		else      
		{
			MOSI_L;   //若最到位为低，则输出低
		}
		byte <<= 1;     // 低一位移位到最高位
		delay_us(1);
		SCK_L;     //拉低时钟
		Temp <<= 1;     //数据左移
 
		if(MISO==1)
			Temp++;     //若从从机接收到高电平，数据自加一
		delay_us(1);
		SCK_H;     //拉高时钟
		
	}
	return (Temp);     //返回数据
}
 
 
/* CPOL = 1, CPHA = 1, MSB first */
uint8_t SOFT_SPI_RW_MODE3( uint8_t write_dat )
{
    uint8_t i, read_dat;
    for( i = 0; i < 8; i++ )
    {
		    SCK_L; 
        if( write_dat & 0x80 )
            MOSI_H;  
        else                    
            MOSI_L;  
        write_dat <<= 1;
        delay_us(1);	
        SCK_H; 
        read_dat <<= 1;  
        if( MISO==1 ) 
            read_dat++; 
		delay_us(1);
        __nop();
    }
    return read_dat;
}
unsigned char SPI_ReadWriteByte(unsigned char TxData)
{
	uint8_t RxData = 0; 
		RxData = SOFT_SPI_RW_MODE1(TxData) ;
	return RxData;
}
 #endif

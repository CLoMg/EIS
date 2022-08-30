#ifndef SPI_H
#define SPI_H

//#define SPI_HARD 1
#define SPI_SOFT 1
#if defined SPI_HARD
	#define SPI1_SCLK_Pin GPIO_PIN_2
	#define SPI1_SCLK_Port GPIOA

	#define SPI1_CS_Pin GPIO_PIN_1
	#define SPI1_CS_Port GPIOA

	#define SPI1_MISO_Pin GPIO_PIN_3
	#define SPI1_MISO_Port GPIOA

	#define SPI1_MOSI_Pin GPIO_PIN_4
	#define SPI1_MOSI_Port GPIOA
	
	void spi_config(void);
	void spi_dma_config(void);
	unsigned char SPI_ReadWriteByte(unsigned char TxData);
	void SPI_ReadWriteBytes(unsigned char *TxData,unsigned char *RxData, unsigned char len);

#elif  defined SPI_SOFT 
	#define SPI1_MOSI_Pin   GPIO_PIN_15
	#define SPI1_MISO_Pin   GPIO_PIN_14
	#define SPI1_SCLK_Pin   GPIO_PIN_13
	#define SPI1_CS_Pin     GPIO_PIN_12
	#define SLAVE_RESET_Pin GPIO_PIN_8
	
	#define SPI1_GPIO_Port  GPIOB
	#define SPI_GPIO_CLK    RCU_GPIOB

	#define SLAVE_RESET_GPIO_Port  GPIOA
	#define SLAVE_RESET_GPIO_CLK  RCU_GPIOA
	
	#define MOSI_H          (GPIO_BOP(SPI1_GPIO_Port) = SPI1_MOSI_Pin)
	#define MOSI_L          (GPIO_BC(SPI1_GPIO_Port)  = SPI1_MOSI_Pin)
	#define SCK_H           (GPIO_BOP(SPI1_GPIO_Port) = SPI1_SCLK_Pin)
	#define SCK_L           (GPIO_BC(SPI1_GPIO_Port)  = SPI1_SCLK_Pin)
	#define NSS_H           (GPIO_BOP(SPI1_GPIO_Port)  = SPI1_CS_Pin)
	#define NSS_L           (GPIO_BC(SPI1_GPIO_Port)  = SPI1_CS_Pin)
	#define MISO            (gpio_input_bit_get(SPI1_GPIO_Port,SPI1_MISO_Pin))
	//#define MISO            (GPIO_ISTAT(SPI1_GPIO_Port)&SPI1_MISO_Pin)
	/* CPOL=0£¬CPHA=0, MSB first */
unsigned char SOFT_SPI_RW_MODE0( unsigned char write_dat );
/* CPOL=0£¬CPHA=1, MSB first */
unsigned char  SOFT_SPI_RW_MODE1(unsigned char byte);
/* CPOL=1£¬CPHA=0, MSB first */
unsigned char SOFT_SPI_RW_MODE2(unsigned char byte);
/* CPOL = 1, CPHA = 1, MSB first */
unsigned char SOFT_SPI_RW_MODE3( unsigned char write_dat );

#endif
void SPI_Init(void);
void Slave_Reset(void);
unsigned char SPI_ReadWriteByte(unsigned char TxData);



#endif

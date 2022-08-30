#ifndef SPI_H
#define SPI_H


typedef enum {
 write = 0x01, read
}func_code_t;

typedef struct spi_protocol
{
	unsigned char header;
	func_code_t func_code;
	unsigned char len;
	unsigned char offset;
	unsigned char *data_buffer;
  unsigned char check_sum;
}spi_protocol_t;
void spi1_init(void);
void SPI_Process(void);
unsigned char LRC_Value_Calc(unsigned char *data_buff,unsigned char len);

#endif
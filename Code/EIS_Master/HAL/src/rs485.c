#include "rs485.h"
#include "uart.h"
#include "gd32f10x_eval.h"
#include "string.h"
#include "timer.h"


/*!
    \brief      initilize the USART configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rs485_init(uint32_t usart_periph ,uint32_t baudrate)
{
	usart_init(usart_periph, baudrate);
	gd_rs485_en_init();	
}

void rs485_Data_Send(uint32_t usart_periph,uint8_t *data_point,uint8_t len)
{
	gd_rs485_en_control(1);
	//delay_ms(5000);
	USART_Data_DMA_Send(usart_periph,data_point,len);
//	delay_ms(50);
//	gd_rs485_en_control(0);
}






#include "uart.h"
#include "gd32e230_usart.h"
#include "gd32e230_dma.h"
#include "gd32e230c_eval.h"
#include "gd32e230_dma.h"
#include "string.h"

#define USART0_TDATA_ADDRESS      ((uint32_t)0x40013828)  
#define USART0_RDATA_ADDRESS      ((uint32_t)0x40013824)

#define USART1_TDATA_ADDRESS      ((uint32_t)0x40004428)  
#define USART1_RDATA_ADDRESS      ((uint32_t)0x40004424)

/*DMA接收缓冲区*/
uint8_t usart0_dma_buffer[256];
/*待处理串口数据缓冲区*/
uint8_t rx0_data_buff[256];
/*待处理数据个数*/
uint32_t USART0_RX_NUM = 0;

/*DMA接收缓冲区*/
uint8_t usart1_dma_buffer[256];
/*待处理串口数据缓冲区*/
uint8_t rx1_data_buff[256];
/*待处理数据个数*/
uint32_t USART1_RX_NUM = 0;

/*!
    \brief      initilize the USART configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart_init(uint32_t usart_periph ,uint32_t baudrate)
{
		dma_parameter_struct dma_init_struct;
    switch(usart_periph){
		case USART0:
			usart_gpio_init(0);
		/* enable USART clock */
			rcu_periph_clock_enable(RCU_USART0);

			/* USART configure */
			usart_deinit(USART0);
			usart_baudrate_set(USART0, baudrate);
			usart_receive_config(USART0, USART_RECEIVE_ENABLE);
			usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
			usart_enable(USART0);
			
			rcu_periph_clock_enable(RCU_DMA);
			/* deinitialize DMA channel1 */
			dma_deinit(DMA_CH2);
			dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
			dma_init_struct.memory_addr = (uint32_t)usart0_dma_buffer;
			dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
			dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
			dma_init_struct.number = sizeof(usart0_dma_buffer);
			dma_init_struct.periph_addr = USART0_RDATA_ADDRESS;
			dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
			dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
			dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
			dma_init(DMA_CH2,&dma_init_struct);
			/* configure DMA mode */
			dma_circulation_disable(DMA_CH2);
			dma_memory_to_memory_disable(DMA_CH2);
			/* enable DMA channel1 */
			dma_channel_enable(DMA_CH2);
			
			usart_dma_receive_config(USART0,USART_DENR_ENABLE);
			nvic_irq_enable(USART0_IRQn,2);
			usart_interrupt_enable(USART0,USART_INT_IDLE);
			break;
		case USART1:
			
			usart_gpio_init(1);
			/* enable USART clock */
			rcu_periph_clock_enable(RCU_USART1);

			/* USART configure */
			usart_deinit(USART1);
			usart_baudrate_set(USART1, baudrate);
			usart_receive_config(USART1, USART_RECEIVE_ENABLE);
			usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
			usart_enable(USART1);
		
			rcu_periph_clock_enable(RCU_DMA);
			/* deinitialize DMA channel1 */
			dma_deinit(DMA_CH4);
			dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
			dma_init_struct.memory_addr = (uint32_t)usart1_dma_buffer;
			dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
			dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
			dma_init_struct.number = sizeof(usart1_dma_buffer);
			dma_init_struct.periph_addr = USART1_RDATA_ADDRESS;
			dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
			dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
			dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
			dma_init(DMA_CH4,&dma_init_struct);
			/* configure DMA mode */
			dma_circulation_disable(DMA_CH4);
			dma_memory_to_memory_disable(DMA_CH4);
			/* enable DMA channel1 */
			dma_channel_enable(DMA_CH4);
			
			usart_dma_receive_config(USART1,USART_DENR_ENABLE);
			nvic_irq_enable(USART1_IRQn,2);
			usart_interrupt_enable(USART1,USART_INT_IDLE);
			break;
		default:
			break;
		}
}



void USART0_IRQHandler(void)
{
	if(RESET != usart_interrupt_flag_get(USART0,USART_INT_FLAG_IDLE))
	{
		usart_interrupt_flag_clear(USART0,USART_INT_FLAG_IDLE);
		usart_data_receive(USART0);
		dma_channel_disable(DMA_CH2);
		
		USART0_RX_NUM = sizeof(usart0_dma_buffer) - dma_transfer_number_get(DMA_CH2);
		memcpy(rx0_data_buff,usart0_dma_buffer,USART0_RX_NUM);
		rx0_data_buff[USART0_RX_NUM] = '\0';
		
		dma_memory_address_config(DMA_CH2,(uint32_t)usart0_dma_buffer);
		dma_transfer_number_config(DMA_CH2,sizeof(usart0_dma_buffer));
		dma_channel_enable(DMA_CH2);
	}
}

void USART1_IRQHandler(void)
{
	if(RESET != usart_interrupt_flag_get(USART1,USART_INT_FLAG_IDLE))
	{
		usart_interrupt_flag_clear(USART1,USART_INT_FLAG_IDLE);
		usart_data_receive(USART1);
		dma_channel_disable(DMA_CH4);
		
		USART1_RX_NUM = sizeof(usart1_dma_buffer) - dma_transfer_number_get(DMA_CH4);
		memcpy(rx1_data_buff,usart1_dma_buffer,USART1_RX_NUM);
		rx1_data_buff[USART1_RX_NUM] = '\0';
		
		dma_memory_address_config(DMA_CH4,(uint32_t)usart1_dma_buffer);
		dma_transfer_number_config(DMA_CH4,sizeof(usart1_dma_buffer));
		dma_channel_enable(DMA_CH4);
	}
}

void USART_Data_Send(uint32_t usart_periph,uint8_t sw_ch,uint8_t *data_point,uint8_t len)
{
	usart_ch_sw_ctrl(usart_periph,sw_ch);
	while(len--){
		usart_data_transmit(usart_periph, *(data_point++));
		while(RESET == usart_flag_get(usart_periph, USART_FLAG_TBE));
	}
}

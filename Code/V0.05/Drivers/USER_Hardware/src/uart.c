
#include "uart.h"
#include "gd32f10x_usart.h"
#include "gd32f10x_eval.h"
#include "gd32f10x_dma.h"
#include "string.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "msg_handler.h"

#define USART0_DATA_ADDRESS      ((uint32_t)&USART_DATA(USART0))

#define USART1_DATA_ADDRESS      ((uint32_t)&USART_DATA(USART1))

#define USART2_DATA_ADDRESS      ((uint32_t)&USART_DATA(USART2))

Usart_Msg_Link_T *LORA_Rec_Msg_Head = NULL;



/*DMA接收缓冲区*/
uint8_t usart0_dma_buffer[rx_buff_size];
/*待处理串口数据缓冲区*/
uint8_t rx0_data_buff[rx_buff_size];
/*待处理数据个数*/
uint32_t USART0_RX_NUM = 0;

/*DMA接收缓冲区*/
uint8_t usart1_dma_buffer[rx_buff_size];
/*待处理串口数据缓冲区*/
uint8_t rx1_data_buff[rx_buff_size];
/*待处理数据个数*/
uint32_t USART1_RX_NUM = 0;

/*DMA接收缓冲区*/
uint8_t usart2_dma_buffer[rx_buff_size];
/*待处理串口数据缓冲区*/
uint8_t rx2_data_buff[rx_buff_size];
/*待处理数据个数*/
uint32_t USART2_RX_NUM = 0;

uint8_t Lora_Queue_Front =0, Lora_Queue_Rear =0;

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
			gd_usart_gpio_init(0);
		/* enable USART clock */
			rcu_periph_clock_enable(RCU_USART0);

			/* USART configure */
			usart_deinit(USART0);
			usart_baudrate_set(USART0, baudrate);
			usart_receive_config(USART0, USART_RECEIVE_ENABLE);
			usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
			usart_enable(USART0);
			
			rcu_periph_clock_enable(RCU_DMA0);
			/* deinitialize DMA0 channel4 (USART0 RX) */
			dma_deinit(DMA0,DMA_CH4);
			dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
			dma_init_struct.memory_addr = (uint32_t)usart0_dma_buffer;
			dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
			dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
			dma_init_struct.number = sizeof(usart0_dma_buffer);
			dma_init_struct.periph_addr = USART0_DATA_ADDRESS;
			dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
			dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
			dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
			dma_init(DMA0,DMA_CH4,&dma_init_struct);
			/* configure DMA mode */
			dma_circulation_disable(DMA0,DMA_CH4);
			dma_memory_to_memory_disable(DMA0,DMA_CH4);
			/* enable DMA0 channel4 */
			dma_channel_enable(DMA0,DMA_CH4);
			
			
			/* deinitialize DMA0 channel3 (USART0 TX)*/
			dma_deinit(DMA0,DMA_CH3);
			dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
			dma_init_struct.memory_addr = (uint32_t)usart0_dma_buffer;
			dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
			dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
			dma_init_struct.number = sizeof(usart0_dma_buffer);
			dma_init_struct.periph_addr = USART0_DATA_ADDRESS;
			dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
			dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
			dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
			dma_init(DMA0,DMA_CH3,&dma_init_struct);
			/* configure DMA mode */
			dma_circulation_disable(DMA0,DMA_CH3);
			dma_memory_to_memory_disable(DMA0,DMA_CH3);
			/* enable DMA channel1 */
			dma_channel_disable(DMA0,DMA_CH3);
			
			usart_dma_transmit_config(USART0,USART_DENT_ENABLE);
			usart_dma_receive_config(USART0,USART_DENR_ENABLE);
			nvic_irq_enable(USART0_IRQn,2,0);
			usart_interrupt_enable(USART0,USART_INT_IDLE);
			
			break;
		case USART1:
			
			gd_usart_gpio_init(1);
			/* enable USART clock */
			rcu_periph_clock_enable(RCU_USART1);

			/* USART configure */
			usart_deinit(USART1);
			usart_baudrate_set(USART1, baudrate);
			usart_receive_config(USART1, USART_RECEIVE_ENABLE);
			usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
			usart_enable(USART1);
		
			rcu_periph_clock_enable(RCU_DMA0);
			/* deinitialize DMA0 channel5 (USART1 RX) */
			dma_deinit(DMA0,DMA_CH5);
			dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
			dma_init_struct.memory_addr = (uint32_t)usart1_dma_buffer;
			dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
			dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
			dma_init_struct.number = sizeof(usart1_dma_buffer);
			dma_init_struct.periph_addr = USART1_DATA_ADDRESS;
			dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
			dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
			dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
			dma_init(DMA0,DMA_CH5,&dma_init_struct);
			/* configure DMA mode */
			dma_circulation_disable(DMA0,DMA_CH5);
			dma_memory_to_memory_disable(DMA0,DMA_CH5);
			/* enable DMA channel1 */
			dma_channel_enable(DMA0,DMA_CH5);
			
			/* deinitialize DMA0 channel6 (USART1 TX) */
			dma_deinit(DMA0,DMA_CH6);
			dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
			dma_init_struct.memory_addr = (uint32_t)usart1_dma_buffer;
			dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
			dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
			dma_init_struct.number = sizeof(usart1_dma_buffer);
			dma_init_struct.periph_addr = USART1_DATA_ADDRESS;
			dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
			dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
			dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
			dma_init(DMA0,DMA_CH6,&dma_init_struct);
			/* configure DMA mode */
			dma_circulation_disable(DMA0,DMA_CH6);
			dma_memory_to_memory_disable(DMA0,DMA_CH6);
			/* enable DMA channel1 */
			dma_channel_disable(DMA0,DMA_CH6);
			
			usart_dma_transmit_config(USART1,USART_DENT_ENABLE);
			usart_dma_receive_config(USART1,USART_DENR_ENABLE);
			nvic_irq_enable(USART1_IRQn,2,0);
			usart_interrupt_enable(USART1,USART_INT_IDLE);
		 nvic_irq_enable(DMA0_Channel6_IRQn,2,0);
			dma_interrupt_enable(DMA0, DMA_CH6, DMA_INT_FTF);
			break;
		case USART2:
			gd_usart_gpio_init(2);
			/* enable USART clock */
			rcu_periph_clock_enable(RCU_USART2);

			/* USART configure */
			usart_deinit(USART2);
			usart_baudrate_set(USART2, baudrate);
			usart_receive_config(USART2, USART_RECEIVE_ENABLE);
			usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);
			usart_enable(USART2);
		
			rcu_periph_clock_enable(RCU_DMA0);
			/* deinitialize DMA0 channel2 (USART2 RX) */
			dma_deinit(DMA0,DMA_CH2);
			dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
			dma_init_struct.memory_addr = (uint32_t)usart2_dma_buffer;
			dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
			dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
			dma_init_struct.number = sizeof(usart2_dma_buffer);
			dma_init_struct.periph_addr = USART2_DATA_ADDRESS;
			dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
			dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
			dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
			dma_init(DMA0,DMA_CH2,&dma_init_struct);
			/* configure DMA mode */
			dma_circulation_disable(DMA0,DMA_CH2);
			dma_memory_to_memory_disable(DMA0,DMA_CH2);
			/* enable DMA channel1 */
			dma_channel_enable(DMA0,DMA_CH2);
			
			/* deinitialize DMA0 channel1 (USART2 TX) */
			dma_deinit(DMA0,DMA_CH1);
			dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
			dma_init_struct.memory_addr = (uint32_t)usart2_dma_buffer;
			dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
			dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
			dma_init_struct.number = sizeof(usart2_dma_buffer);
			dma_init_struct.periph_addr = USART2_DATA_ADDRESS;
			dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
			dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
			dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
			dma_init(DMA0,DMA_CH1,&dma_init_struct);
			/* configure DMA mode */
			dma_circulation_disable(DMA0,DMA_CH1);
			dma_memory_to_memory_disable(DMA0,DMA_CH1);
			/* enable DMA channel1 */
			dma_channel_disable(DMA0,DMA_CH1);
			
			usart_dma_transmit_config(USART2,USART_DENT_ENABLE);
			usart_dma_receive_config(USART2,USART_DENR_ENABLE);
			nvic_irq_enable(USART2_IRQn,2,0);
			usart_interrupt_enable(USART2,USART_INT_IDLE);
		 nvic_irq_enable(DMA0_Channel1_IRQn,2,0);
			dma_interrupt_enable(DMA0, DMA_CH1, DMA_INT_FTF);
			break;
		default:
			break;
		}
}

void DMA0_Channel6_IRQHandler(void)
{
	if(RESET != dma_interrupt_flag_get(DMA0,DMA_CH6,DMA_INT_FTF))
	{
	  
		dma_interrupt_flag_clear(DMA0,DMA_CH6,DMA_INT_FTF);
		gd_rs485_en_control(0);
	}
}


void USART0_IRQHandler(void)
{
	if(RESET != usart_interrupt_flag_get(USART0,USART_INT_FLAG_IDLE))
	{
		usart_interrupt_flag_clear(USART0,USART_INT_FLAG_IDLE);
		usart_data_receive(USART0);
		dma_channel_disable(DMA0,DMA_CH4);
		
		USART0_RX_NUM = sizeof(usart0_dma_buffer) - dma_transfer_number_get(DMA0,DMA_CH4);
		static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
		
		if((usart0_dma_buffer[0] == 0x4A)&&(usart0_dma_buffer[1] == 0x54)){
				uint8_t *data_addr = (uint8_t *)pvPortMalloc(sizeof(uint8_t)*USART0_RX_NUM);
				memcpy(data_addr,usart0_dma_buffer,USART0_RX_NUM);
				
				xData *rx_msg = (xData *)pvPortMalloc(sizeof(xData));
				
				rx_msg->iEventType = LORARxEvent;
				rx_msg->len = USART0_RX_NUM;
				rx_msg->iValue = data_addr;
				xQueueSendFromISR(MSGTR_Queue,rx_msg,&xHigherPriorityTaskWoken);
				vPortFree(rx_msg);
			}
		else{
			
				uint8_t *data_addr = (uint8_t *)pvPortMalloc(sizeof(uint8_t)*USART0_RX_NUM);
				memcpy(data_addr,usart0_dma_buffer,USART0_RX_NUM);
				size_t freeheapsize = xPortGetFreeHeapSize();
				
				xData *rx_msg = (xData *)pvPortMalloc(sizeof(xData));
			
				freeheapsize = xPortGetFreeHeapSize();
			
				rx_msg->iEventType = LORARxEvent;
				rx_msg->len = USART0_RX_NUM;
				rx_msg->iValue = data_addr;
				xQueueSendFromISR(LORAControl_Queue,rx_msg,&xHigherPriorityTaskWoken);
				vPortFree(rx_msg);
		}
		//rx0_data_buff[USART0_RX_NUM] = '\0';
		dma_memory_address_config(DMA0,DMA_CH4,(uint32_t)usart0_dma_buffer);
		dma_transfer_number_config(DMA0,DMA_CH4,sizeof(usart0_dma_buffer));
		dma_channel_enable(DMA0,DMA_CH4);
		
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
void USART1_IRQHandler(void)
{
	if(RESET != usart_interrupt_flag_get(USART1,USART_INT_FLAG_IDLE))
	{
		usart_interrupt_flag_clear(USART1,USART_INT_FLAG_IDLE);
		usart_data_receive(USART1);
		dma_channel_disable(DMA0,DMA_CH5);
		
		USART1_RX_NUM = sizeof(usart1_dma_buffer) - dma_transfer_number_get(DMA0,DMA_CH5);
		
		if((LORA_Rec_Msg_Head !=NULL)&&(usart1_dma_buffer[0] == 0x4A)&&(usart1_dma_buffer[1] == 0x54)){
				uint8_t *data_addr = (uint8_t *)malloc(sizeof(uint8_t)*USART1_RX_NUM);
				memcpy(data_addr,usart1_dma_buffer,USART1_RX_NUM);
				
				Usart_Msg_Link_T *node = (Usart_Msg_Link_T *)malloc(sizeof(Usart_Msg_Link_T));

				uint32_t heap_sp = __get_MSP();

				
				node->Data.len  = USART1_RX_NUM;
				node->Data.addr = data_addr;
				node->Next = NULL;
				
				Usart_Msg_Link_T *tail = LORA_Rec_Msg_Head;
				while(tail->Next != NULL)
					tail = tail->Next;
				tail->Next = node;
			}
//	  if((Lora_Queue_Rear+USART0_RX_NUM) % QueueMAX == Lora_Queue_Front)
//			;
//		else{
//			if((Lora_Queue_Rear + USART0_RX_NUM) < QueueMAX){
//				memcpy(&rx0_data_buff[Lora_Queue_Rear],usart0_dma_buffer,USART0_RX_NUM);
//				Lora_Queue_Rear += USART0_RX_NUM;
//			}
//			else{
//				uint8_t first_cp_num = QueueMAX-1-Lora_Queue_Rear;
//				uint8_t second_cp_num = USART0_RX_NUM  - first_cp_num;
//				memcpy(&rx0_data_buff[Lora_Queue_Rear],usart0_dma_buffer,first_cp_num);
//				
//				memcpy(rx0_data_buff,&usart0_dma_buffer[first_cp_num],second_cp_num);
//				Lora_Queue_Rear = second_cp_num;
//			}
//		}
		else
			memcpy(rx1_data_buff,usart1_dma_buffer,USART1_RX_NUM);
		//rx0_data_buff[USART0_RX_NUM] = '\0';
		dma_memory_address_config(DMA0,DMA_CH5,(uint32_t)usart1_dma_buffer);
		dma_transfer_number_config(DMA0,DMA_CH5,sizeof(usart1_dma_buffer));
		dma_channel_enable(DMA0,DMA_CH5);
	}
}

void USART2_IRQHandler(void)
{
	if(RESET != usart_interrupt_flag_get(USART2,USART_INT_FLAG_IDLE))
	{
		usart_interrupt_flag_clear(USART2,USART_INT_FLAG_IDLE);
		usart_data_receive(USART2);
		dma_channel_disable(DMA0,DMA_CH2);
		
		USART2_RX_NUM = sizeof(usart2_dma_buffer) - dma_transfer_number_get(DMA0,DMA_CH2);
		
		if((LORA_Rec_Msg_Head !=NULL)&&(usart2_dma_buffer[0] == 0x4A)&&(usart2_dma_buffer[1] == 0x54)){
				uint8_t *data_addr = (uint8_t *)malloc(sizeof(uint8_t)*USART1_RX_NUM);
				memcpy(data_addr,usart2_dma_buffer,USART1_RX_NUM);
				
				Usart_Msg_Link_T *node = (Usart_Msg_Link_T *)malloc(sizeof(Usart_Msg_Link_T));

				uint32_t heap_sp = __get_MSP();

				
				node->Data.len  = USART2_RX_NUM;
				node->Data.addr = data_addr;
				node->Next = NULL;
				
				Usart_Msg_Link_T *tail = LORA_Rec_Msg_Head;
				while(tail->Next != NULL)
					tail = tail->Next;
				tail->Next = node;
			}
//	  if((Lora_Queue_Rear+USART0_RX_NUM) % QueueMAX == Lora_Queue_Front)
//			;
//		else{
//			if((Lora_Queue_Rear + USART0_RX_NUM) < QueueMAX){
//				memcpy(&rx0_data_buff[Lora_Queue_Rear],usart0_dma_buffer,USART0_RX_NUM);
//				Lora_Queue_Rear += USART0_RX_NUM;
//			}
//			else{
//				uint8_t first_cp_num = QueueMAX-1-Lora_Queue_Rear;
//				uint8_t second_cp_num = USART0_RX_NUM  - first_cp_num;
//				memcpy(&rx0_data_buff[Lora_Queue_Rear],usart0_dma_buffer,first_cp_num);
//				
//				memcpy(rx0_data_buff,&usart0_dma_buffer[first_cp_num],second_cp_num);
//				Lora_Queue_Rear = second_cp_num;
//			}
//		}
		else
			memcpy(rx2_data_buff,usart2_dma_buffer,USART2_RX_NUM);
		//rx0_data_buff[USART0_RX_NUM] = '\0';
		dma_memory_address_config(DMA0,DMA_CH2,(uint32_t)usart2_dma_buffer);
		dma_transfer_number_config(DMA0,DMA_CH2,sizeof(usart2_dma_buffer));
		dma_channel_enable(DMA0,DMA_CH2);
	}
}

//void USART1_IRQHandler(void)
//{
//	if(RESET != usart_interrupt_flag_get(USART1,USART_INT_FLAG_IDLE))
//	{
//		usart_interrupt_flag_clear(USART1,USART_INT_FLAG_IDLE);
//		usart_data_receive(USART1);
//		dma_channel_disable(DMA0,DMA_CH5);
//		
//		USART1_RX_NUM = sizeof(usart1_dma_buffer) - dma_transfer_number_get(DMA0,DMA_CH5);

//		memcpy(rx1_data_buff,usart1_dma_buffer,USART1_RX_NUM);
//		//rx1_data_buff[USART1_RX_NUM] = '\0';
//		
//		dma_memory_address_config(DMA0,DMA_CH5,(uint32_t)usart1_dma_buffer);
//		dma_transfer_number_config(DMA0,DMA_CH5,sizeof(usart1_dma_buffer));
//		dma_channel_enable(DMA0,DMA_CH5);
//	}
//}

void USART_Data_Send(uint32_t usart_periph,uint8_t sw_ch,uint8_t *data_point,uint8_t len)
{
	while(len--){
		usart_data_transmit(usart_periph, *(data_point++));
		while(RESET == usart_flag_get(usart_periph, USART_FLAG_TBE));
	}
}

void USART_Data_DMA_Send(uint32_t usart_periph,uint8_t *data_point,uint8_t len){
	if(usart_periph == USART0)
	{
		dma_channel_disable(DMA0,DMA_CH3);
		
		dma_memory_address_config(DMA0,DMA_CH3,(uint32_t)data_point);
		dma_transfer_number_config(DMA0,DMA_CH3,len);
		dma_channel_enable(DMA0,DMA_CH3);
		while(RESET	== dma_flag_get(DMA0,DMA_CH3,DMA_FLAG_FTF));
		dma_flag_clear(DMA0,DMA_CH3,DMA_FLAG_FTF);
	}
	else if(usart_periph == USART1)
	{
		dma_channel_disable(DMA0,DMA_CH6);
		
		dma_memory_address_config(DMA0,DMA_CH6,(uint32_t)data_point);
		dma_transfer_number_config(DMA0,DMA_CH6,len);
		dma_channel_enable(DMA0,DMA_CH6);
	}
		else if(usart_periph == USART2)
	{
		dma_channel_disable(DMA0,DMA_CH1);
		
		dma_memory_address_config(DMA0,DMA_CH1,(uint32_t)data_point);
		dma_transfer_number_config(DMA0,DMA_CH1,len);
		dma_channel_enable(DMA0,DMA_CH1);
	}
	  else;
}



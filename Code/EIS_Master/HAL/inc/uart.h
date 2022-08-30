#ifndef UART_H
#define UART_H

#include <stdint.h>
#define rx_buff_size 255
#define QueueMAX 256
/*待处理串口数据缓冲区*/
extern uint8_t rx0_data_buff[rx_buff_size];
/*待处理数据个数*/
extern  uint32_t USART0_RX_NUM;

/*待处理串口数据缓冲区*/
extern uint8_t rx1_data_buff[rx_buff_size];
/*待处理数据个数*/
extern  uint32_t USART1_RX_NUM;
extern  uint8_t Lora_Queue_Front , Lora_Queue_Rear;

typedef struct{
	uint8_t len;
	uint8_t *addr;
}Usart_Msg_T;

typedef struct Usart_Msg_Link{
	Usart_Msg_T Data;
	struct Usart_Msg_Link  *Next;
}Usart_Msg_Link_T;

extern Usart_Msg_Link_T *LORA_Rec_Msg_Head;
void usart_init(uint32_t usart_periph ,uint32_t baudrate);
void DMA0_Channel6_IRQHandler(void);
void USART0_IRQHandler(void);
void USART1_IRQHandler(void);
void USART_Data_Send(uint32_t usart_periph,uint8_t ,uint8_t *data_point,uint8_t len);
void USART_Data_DMA_Send(uint32_t usart_periph,uint8_t *data_point,uint8_t len);

#endif

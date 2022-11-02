#ifndef __USR_K7_H
#define __USR_K7_H
#include "sys.h"

void usr_k7_gpio_config(void);
void usr_k7_init(void);
Error_Code_t usr_reset(void);
Error_Code_t usr_Mac_query (void);

int8_t usr_echo_query(void);
Error_Code_t usr_echo_set(int8_t newstate);
Error_Code_t usr_transparent_mode (void);

Error_Code_t usr_wann_query (char *info_buff);
Error_Code_t usr_wann_set (void);

Error_Code_t usr_dns_query (void);
Error_Code_t usr_dns_set (void);

Error_Code_t usr_Mac_query (void);

uint8_t usr_uartclbuf_query (void);
Error_Code_t usr_uartclbuf_set (int8_t newstate);


char *usr_write_read(char *command, char *read_buffer);
void usr_data_send(char *sendbuff,uint16_t length);
void usr_data_receive(char *data_read);


#endif

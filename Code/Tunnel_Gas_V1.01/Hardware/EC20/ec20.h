#ifndef __EC20_H
#define __EC20_H
#include "sys.h"

//����EC20_PERSTNʹ�����ţ��ɰ�PG11 �°�PC1
#define ec20_PERSTN PCout(1)
//#define ec20_PERSTN PGout(11)

//����EC20_PERSTNʹ�����ţ��ɰ�PA15 �°�PA3
#define ec20_PowerEN PAout(3)
//#define ec20_PowerEN PAout(15)

extern _Bool ec20_reconnect_flag;

void ec20_get_server_addr(uint32_t *addr_read);
Error_Code_t ec20_modify_server_addr(uint32_t *addr_to_write);

void ec20_init(void);
void ec20_gpio_config(void);
void ec20_Software_Reset(void);
Error_Code_t ec20_test(void);
void ec20_send_data(char *sendbuff,uint16_t length);
Error_Code_t ec20_connnect(void);
void ec20_Hardware_Reset(void);
void ec20_reconnnect(void);

#endif

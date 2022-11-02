#ifndef __EC20_H
#define __EC20_H
#include "sys.h"

//定义EC20_PERSTN使能引脚，旧版PG11 新版PC1
#define ec20_PERSTN PCout(1)
//#define ec20_PERSTN PGout(11)

//定义EC20_PERSTN使能引脚，旧版PA15 新版PA3
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

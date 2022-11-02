#ifndef __UPGRADE_H
#define __UPGRADE_H
#include "sys.h"

#define  faile  0x00
#define  succe  0x01


#define  gateway_to_node  										0x01
#define  node_to_gateway  										0x02
#define  node_send_to_gateway  								0x03

#define  upgrade_inform_cmd_high   						0x20
#define  upgrade_inform_cmd_low    						0x03
#define  upgrade_request_cmd_high  						0x20
#define  upgrade_request_cmd_low   						0x04

#define  gateway_upgrade 								      0x00
#define  node_upgrade   											0x01
#define  upgrade_able  												0x01
#define  upgrade_disable   										0x00
#define  gateway_soft_version         				0x01
#define  gateway_soft_version_new         		0x02
#define  node_soft_version         						0x01
#define  node_soft_version_new         				0x02
#define  upgrade_request_num_of_package_mode   0x01 // 01/02/03/04/05/06, 128/256/512/1024/2048/4096
#define  upgrade_request_num_of_package_size   128 //128/256/512/1024/2048/4096

void firmware_send_process(void);
int8_t node_upgrade_rev_process(void);

#endif

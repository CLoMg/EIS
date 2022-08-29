#ifndef __HAL_OTA_H_
#define __HAL_OTA_H_

#include "stdbool.h"

#define App_A_Address 0x08020000
#define App_B_Address 0x08080000

#define NV_OTA_INFORMATION_OFFSET 1024
#define NV_OTA_APP_STATUS_OFFSET 0
#define NV_OTA_APP_STATUS_LEN 1
#define NV_OTA_IMAGE_NOW_FW_VERSION_OFFSET (NV_OTA_APP_STATUS_OFFSET + NV_OTA_APP_STATUS_LEN)
#define NV_OTA_IMAGE_NOW_FW_VERSION_LEN 1
#define NV_OTA_IMAGE_NEW_FW_VERSION_OFFSET (NV_OTA_IMAGE_NOW_FW_VERSION_OFFSET + NV_OTA_IMAGE_NOW_FW_VERSION_LEN)
#define NV_OTA_IMAGE_NEW_FW_VERSION_LEN 1
#define NV_OTA_NEW_IMAGE_SIZE_OFFSET (NV_OTA_IMAGE_NEW_FW_VERSION_OFFSET + NV_OTA_IMAGE_NEW_FW_VERSION_LEN)
#define NV_OTA_NEW_IMAGE_SIZE_LEN 4
#define NV_OTA_APP_ENTRY_OFFSET (NV_OTA_NEW_IMAGE_SIZE_OFFSET + NV_OTA_NEW_IMAGE_SIZE_LEN)
#define NV_OTA_APP_ENTRY_LEN 1
#define NV_OTA_MD5_OFFSET (NV_OTA_APP_ENTRY_OFFSET + NV_OTA_APP_ENTRY_LEN)
#define NV_OTA_MD5_LEN 16
#define NV_OTA_INFORMATION_LEN (NV_OTA_MD5_OFFSET+NV_OTA_MD5_LEN)

//#define UPGRADE_SUCCESS 0x00U
//#define DOWNLOAD_START 0x01U
//#define VERIFY_START 0x02U
//#define VERIFY_OK 0x03U

/***** THREAD_OTA_FLAG ****/
#define OTA_Thread_Flag_Mask (OTA_DOWNLOAD_START|OTA_VERIFY_START)
#define OTA_DOWNLOAD_START 0x00000001U
#define OTA_VERIFY_START   0x00000002U

#define DEFAULT_PACKET_LENGTH 200 //长度必须4字节对齐
#define CODEENCRPT 0x55
#define IDENTIFY_CODE 0x56
#define EXTFLASH_FW_VERSION_OFFSET 0
#define EXTFLASH_FW_VERSION_LEN 1
#define EXTFLASH_FW_SIZE_OFFSET (EXTFLASH_FW_VERSION_OFFSET + EXTFLASH_FW_VERSION_LEN)
#define EXTFLASH_FW_SIZE_LEN 3
#define EXTFLASH_MD5_OFFSET (EXTFLASH_FW_SIZE_OFFSET + EXTFLASH_FW_SIZE_LEN)
#define EXTFLASH_MD5_LEN 16
#define EXTFLASH_IDENTIFY_OFFSET (EXTFLASH_MD5_OFFSET + EXTFLASH_MD5_LEN)
#define EXTFLASH_IDENTIFY_LEN 1
#define EXTFLASH_CODE_ERA_OFFSET (EXTFLASH_IDENTIFY_OFFSET + EXTFLASH_IDENTIFY_LEN)
#define EXTFLASH_CODE_ERA_LEN 2
#define EXTFLASH_IMAGE_START_OFFSET 32

typedef enum
{
    NONE_IMAGE = 0U,
    DOWNLOAD_START,
    VERIFY_START,
    VERIFY_OK,
    UPGRATE_FAIL,
} FW_status;



bool OTA_image_verify(unsigned int initial_len);

void OTA_init(void);

bool OTA_powerdown_request(void);

void OTA_packet_flag_set(unsigned short SN);

unsigned short OTA_packet_miss_SN_get(void);

void OTA_packet_flag_init(void);

void OTA_packet_flag_deinit(void);

bool OTA_packet_integrality_check(void);

bool OTA_packet_is_exist(unsigned short SN);

void OTA_new_fw_version_set(unsigned char version);

unsigned char OTA_new_fw_version_get(void);
static void Flash_Read_To_Bytes(unsigned int Flash_addr, unsigned char *data_buff,unsigned char len);

#endif //__HAL_OTA_H_
#include "hal_ota.h"
#include "stdint.h"
#include "string.h"
#include "systick.h"
#include "gd32f10x.h"
#include "gd32f10x_fmc.h"


// leftrotate function definition
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))
extern void ANP_GatewaySendFWRequest2Server(void);
#define OTA_RequestFWPacketFromServer() ANP_GatewaySendFWRequest2Server()

static bool OTA_decrypt(uint8_t *code_buf, uint16_t len);
static uint32_t OTA_New_FW_Entry_get(void);
/******  ANP任务定义 ******/


// r specifies the per-round shift amounts
const uint32_t r[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                      5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
                      4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                      6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
// Constants are the integer part of the sines of integers (in radians) * 2^32.
const uint32_t k[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

static uint32_t OTA_Thread_flags = 0;

extern uint8_t machine_MAC[8];

extern void HardFault_Handler(void);

extern void ANP_GatewaySendFWUpgrateResult2Server(FW_status status);

static bool OTA_decrypt(uint8_t *code_buf, uint16_t len)
{
    uint8_t *decode_buf = NULL;
    decode_buf = malloc(len);
    if (decode_buf == NULL)
        return false;
    for (uint16_t i = 0; i < len; i++)
    {
        decode_buf[i] = ~(code_buf[i] ^ CODEENCRPT);
    }
    memcpy(code_buf, decode_buf, len);
    free(decode_buf);
    return true; 
}

static void to_bytes(uint32_t val, uint8_t *bytes)
{
    bytes[0] = (uint8_t)val;
    bytes[1] = (uint8_t)(val >> 8);
    bytes[2] = (uint8_t)(val >> 16);
    bytes[3] = (uint8_t)(val >> 24);
}

static uint32_t to_int32(const uint8_t *bytes)
{
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) | ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}

uint32_t CRC32BUG=0;
bool OTA_image_verify(uint32_t initial_len)
{
 
    //__disable_irq();
    //SysTick->CTRL  &=~ SysTick_CTRL_TICKINT_Msk;

    uint8_t md5_inflash[16] = {0};
    uint8_t md5_calculated[16] = {0};

    // These vars will contain the hash
    uint32_t h0, h1, h2, h3;

    // Message (to prepare)
    uint8_t *msg = NULL;

    size_t new_len, offset_tmp;
    uint32_t w[16];
    uint32_t a, b, c, d, i, f, g, temp;

    // Initialize variables - simple count in nibbles:
    h0 = 0x67452301;
    h1 = 0xefcdab89;
    h2 = 0x98badcfe;
    h3 = 0x10325476;
    initial_len -= (EXTFLASH_IMAGE_START_OFFSET);
    //NV_read(NV_OTA_INFORMATION_OFFSET + NV_OTA_MD5_OFFSET, md5_inflash, NV_OTA_MD5_LEN);
    //extFlashRead(EXTFLASH_MD5_OFFSET, EXTFLASH_MD5_LEN, md5_inflash);
    //drv_mx25_read(EXTFLASH_MD5_OFFSET, md5_inflash, EXTFLASH_MD5_LEN);
		/*从FLASH中读取接收到的MD5校验值到缓存中*/
		Flash_Read_To_Bytes(NEW_FW_CHECKSUM, md5_inflash,EXTFLASH_MD5_LEN);
		
    //Pre-processing:
    //append "1" bit to message
    //append "0" bits until message length in bits ≡ 448 (mod 512)
    //append length mod (2^64) to message

    for (new_len = initial_len + 1; new_len % (512/8) != (448/8); new_len++)
        ;

    msg = (uint8_t *)malloc(new_len + 8 - initial_len);
    msg[0] = 0x80; // append the "1" bit; most significant bit is "first"
    for (offset_tmp = 1; offset_tmp < new_len - initial_len; offset_tmp++)
        msg[offset_tmp] = 0; // append "0" bits

    // append the len in bits at the end of the buffer.
    to_bytes(initial_len * 8, msg + new_len - initial_len);
    // initial_len>>29 == initial_len*8>>32, but avoids overflow.
    to_bytes(initial_len >> 29, msg + new_len + 4 - initial_len);

    // Process the message in successive 512-bit chunks:
    //for each 512-bit chunk of message:
    for (offset_tmp = 0; offset_tmp < new_len; offset_tmp += 64)
    {

        //autoCtrlDelayMs(2);
				delay_1ms(2);
        if ((offset_tmp + (512 / 8)) < initial_len)
        {
            uint8_t data[64] = {0};
           // drv_mx25_read(offset_tmp + 32, data, 64);
						Flash_Read_To_Bytes(offset_tmp + 32, data, 64);
            for(uint8_t i=0;i<64;i++)
                CRC32BUG+=data[i];
            // break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
            for (i = 0; i < 16; i++)
                w[i] = to_int32(data + i * 4);
        }
        else if ((offset_tmp < initial_len) && ((offset_tmp + (512 / 8)) >= initial_len))
        {
            uint8_t c = initial_len - offset_tmp;
            uint8_t data[64] = {0};
           // drv_mx25_read(offset_tmp + 32, data, c);
						Flash_Read_To_Bytes(offset_tmp + 32, data , c);
            memcpy(data + c, msg, 64 - c);
                        for(uint8_t i=0;i<64;i++)
                CRC32BUG+=data[i];
            // break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
            for (i = 0; i < 16; i++)
                w[i] = to_int32(data + i * 4);
        }
        else
        {
            uint8_t data[64] = {0};
            memcpy(data, msg + offset_tmp - initial_len, 64);
                        for(uint8_t i=0;i<64;i++)
                CRC32BUG+=data[i];
            // break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
            for (i = 0; i < 16; i++)
                w[i] = to_int32(data + i * 4);
        }

        // Initialize hash value for this chunk:
        a = h0;
        b = h1;
        c = h2;
        d = h3;

        // Main loop:
        for (i = 0; i < 64; i++)
        {

            if (i < 16)
            {
                f = (b & c) | ((~b) & d);
                g = i;
            }
            else if (i < 32)
            {
                f = (d & b) | ((~d) & c);
                g = (5 * i + 1) % 16;
            }
            else if (i < 48)
            {
                f = b ^ c ^ d;
                g = (3 * i + 5) % 16;
            }
            else
            {
                f = c ^ (b | (~d));
                g = (7 * i) % 16;
            }

            temp = d;
            d = c;
            c = b;
            b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
            a = temp;
        }

        // Add this chunk's hash to result so far:
        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
    }

    //var char md5_calculated[16] := h0 append h1 append h2 append h3 //(Output is in little-endian)
    to_bytes(h0, md5_calculated);
    to_bytes(h1, md5_calculated + 4);
    to_bytes(h2, md5_calculated + 8);
    to_bytes(h3, md5_calculated + 12);
    //OS_task_INT_enable();
    //__enable_irq();
    //HAL_ResumeTick();
    //SysTick->CTRL  |= SysTick_CTRL_TICKINT_Msk;
    // cleanup
    free(msg);
    if (0 == memcmp(md5_inflash, md5_calculated, 16))
        return true;
    return false;
}


static uint32_t OTA_New_FW_Entry_get(void)
{
    uint32_t p_hardfault = (uint32_t)HardFault_Handler;
    if ((p_hardfault & (App_A_Address | App_B_Address)) == App_A_Address)
        return App_B_Address;
    return App_A_Address;
}

static void Flash_Read_To_Bytes(uint32_t Flash_addr, uint8_t *data_buff,uint8_t len){
	uint8_t i = 0,j = 0;
	uint32_t flash_data_tmp = 0;
	for(i=0;i<len;++i){
		flash_data_tmp = (*(__IO uint32_t *)(Flash_addr + i*4));
		for(j=0;j<4;++j)
			data_buff[i*4+j] = (uint8_t)(flash_data_tmp>>((3-j)*8));
	}
}

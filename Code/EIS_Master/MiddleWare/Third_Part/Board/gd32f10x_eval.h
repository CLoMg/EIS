/*!
    \file    gd32f10x_eval.h
    \brief   definitions for GD32F10x_EVAL's leds, keys and COM ports hardware resources

    \version 2014-12-26, V1.0.0, firmware for GD32F10x
    \version 2017-06-20, V2.0.0, firmware for GD32F10x
    \version 2018-07-31, V2.1.0, firmware for GD32F10x
    \version 2020-09-30, V2.2.0, firmware for GD32F10x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef GD32F10X_EVAL_H
#define GD32F10X_EVAL_H

#ifdef cplusplus
 extern "C" {
#endif

#include "gd32f10x.h"
     
/* exported types */
typedef enum 
{
    RUN_LED = 0,
    ERR_LED = 1,
} led_typedef_enum;

typedef enum 
{
    KEY_WAKEUP = 0,
    KEY_TAMPER = 1,
    KEY_USER   = 2
} key_typedef_enum;

typedef enum 
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
} keymode_typedef_enum;

typedef enum 
{
  All_OFF = 0x00,
	Left,
	Right,
	Both,
	Error,
	Dir_Eof
}BSP_Direction_TypeDef;

/* eval board low layer led */
#define LEDn                             2U

#define RUN_LED_PIN                         GPIO_PIN_11
#define RUN_LED_GPIO_PORT                   GPIOA
#define RUN_LED_GPIO_CLK                    RCU_GPIOA
  
#define ERR_LED_PIN                         GPIO_PIN_10
#define ERR_LED_GPIO_PORT                   GPIOA
#define ERR_LED_GPIO_CLK                    RCU_GPIOA

#define COMn                             2U

#define USART0_TX_PIN                 GPIO_PIN_6
#define USART0_RX_PIN                 GPIO_PIN_7
#define USART0_GPIO_PORT              GPIOB
#define USART0_GPIO_CLK               RCU_GPIOB
#define USART0_GPIO_REMAP             GPIO_USART0_REMAP

#define RS485_EN_PIN									GPIO_PIN_4
#define RS485_EN_GPIO_PORT            GPIOA
#define RS485_EN_GPIO_CLK             RCU_GPIOA


#define USART1_TX_PIN                 GPIO_PIN_2
#define USART1_RX_PIN                 GPIO_PIN_3
#define USART1_GPIO_PORT              GPIOA
#define USART1_GPIO_CLK               RCU_GPIOA

#define KEYn                             3U

/* wakeup push-button */
#define WAKEUP_KEY_PIN                   GPIO_PIN_0
#define WAKEUP_KEY_GPIO_PORT             GPIOA
#define WAKEUP_KEY_GPIO_CLK              RCU_GPIOA
#define WAKEUP_KEY_EXTI_LINE             EXTI_0
#define WAKEUP_KEY_EXTI_PORT_SOURCE      GPIO_PORT_SOURCE_GPIOA
#define WAKEUP_KEY_EXTI_PIN_SOURCE       GPIO_PIN_SOURCE_0
#define WAKEUP_KEY_EXTI_IRQn             EXTI0_IRQn  

/* tamper push-button */
#define TAMPER_KEY_PIN                   GPIO_PIN_13
#define TAMPER_KEY_GPIO_PORT             GPIOC
#define TAMPER_KEY_GPIO_CLK              RCU_GPIOC
#define TAMPER_KEY_EXTI_LINE             EXTI_13
#define TAMPER_KEY_EXTI_PORT_SOURCE      GPIO_PORT_SOURCE_GPIOC
#define TAMPER_KEY_EXTI_PIN_SOURCE       GPIO_PIN_SOURCE_13
#define TAMPER_KEY_EXTI_IRQn             EXTI10_15_IRQn

/* user push-button */
#define USER_KEY_PIN                     GPIO_PIN_14
#define USER_KEY_GPIO_PORT               GPIOB
#define USER_KEY_GPIO_CLK                RCU_GPIOB
#define USER_KEY_EXTI_LINE               EXTI_14
#define USER_KEY_EXTI_PORT_SOURCE        GPIO_PORT_SOURCE_GPIOB
#define USER_KEY_EXTI_PIN_SOURCE         GPIO_PIN_SOURCE_14
#define USER_KEY_EXTI_IRQn               EXTI10_15_IRQn

/** @defgroup Direction_Light LOW LEVEL Direction_Light Constants
  * @{
  */
#define DIR_Ctrln                                    4
#define DIR_LEFT1_PIN                                GPIO_PIN_3
 #define DIR_LEFT1_GPIO_PORT                          GPIOB
#define DIR_LEFT1_GPIO_CLK               						 RCU_GPIOB

#define DIR_LEFT2_PIN                                GPIO_PIN_1
#define DIR_LEFT2_GPIO_PORT                          GPIOA
#define DIR_LEFT2_GPIO_CLK               						 RCU_GPIOA


#define DIR_RIGHT1_PIN                               GPIO_PIN_4
#define DIR_RIGHT1_GPIO_PORT                         GPIOB
#define DIR_RIGHT1_GPIO_CLK               					 RCU_GPIOB

#define DIR_RIGHT2_PIN                               GPIO_PIN_13
#define DIR_RIGHT2_GPIO_PORT                         GPIOC
#define DIR_RIGHT2_GPIO_CLK               					 RCU_GPIOC


/** @defgroup LORA Module LOW LEVEL Direction_Light Constants
  * @{
  */

#define LORA_RESET_PIN                                GPIO_PIN_8
#define LORA_RESET_GPIO_PORT                          GPIOB
#define LORA_RESET_CLK               						      RCU_GPIOB

#define LORA_WAKE_PIN                                GPIO_PIN_12
#define LORA_WAKE_GPIO_PORT                          GPIOA
#define LORA_WAKE_CLK               						     RCU_GPIOA

/** @defgroup SPI LOW LEVEL SPI Constants
  * @{
  */
//#define SPI1_GPIO_PORT                               GPIOB
//#define SPI1_GPIO_CLK                                RCU_GPIOB
//#define SPI1_GPIO_REMAP                              GPIO_USART0_REMAP

//#define SPI1_MOSI_PIN                                GPIO_PIN_15
//#define SPI1_MISO_PIN                                GPIO_PIN_14
//#define SPI1_SCK_PIN                                 GPIO_PIN_13
//#define SPI1_NSS_PIN                                 GPIO_PIN_12




/* function declarations */
/* configure led GPIO */
void gd_eval_led_init(led_typedef_enum lednum);
/* turn on selected led */
void gd_eval_led_on(led_typedef_enum lednum);
/* turn off selected led */
void gd_eval_led_off(led_typedef_enum lednum);
/* toggle the selected led */
void gd_eval_led_toggle(led_typedef_enum lednum);
/* configure key */
void gd_eval_key_init(key_typedef_enum key_num, keymode_typedef_enum key_mode);
/* return the selected key state */
uint8_t gd_eval_key_state_get(key_typedef_enum key);
/* configure COM port */
void gd_usart_gpio_init(uint8_t usart_num);
/*! configure RS485_EN port*/
void gd_rs485_en_init(void);
/*configure RS485_EN port*/
void gd_rs485_en_control(_Bool states);

/*configure LORA module control port*/
void LORA_Module_GPIO_Init(void);
/*control the LORA module reset pin*/
void LORA_Module_Reset_Control(_Bool state);
/*comntrol the LORA module wake pin*/
void LORA_Module_Wake_Control(_Bool state);
/*comntrol the SPI1  pin*/
void spi_gpio_init(void);
void  spi_ss_control(_Bool state);
/** @defgroup STM32WLXX_NUCLEO_LOW_LEVEL_DIRECTION_LIGHT_Functions 
  * @{
  */
int32_t BSP_Dir_Light_Init(void);
int32_t BSP_Dir_Light_DeInit(void);
int32_t BSP_Light_Control(BSP_Direction_TypeDef dir);
BSP_Direction_TypeDef BSP_Light_Get(void);

#ifdef cplusplus
}
#endif

#endif /* GD32F10X_EVAL_H */

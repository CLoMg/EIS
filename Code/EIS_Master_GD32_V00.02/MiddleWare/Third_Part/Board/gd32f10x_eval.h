/*!
    \file    gd32f10x_eval.h
    \brief   definitions for GD32F10x_EVAL's leds, EXITs and COM ports hardware resources

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

#define CHECK_ERROR(e) if(e != 0) return e;
     
/* exported types */
typedef enum 
{
    RUN_LED = 0,
    ERR_LED = 1,
} led_typedef_enum;

typedef enum 
{
    EXIT_WATER = 0,
    EXIT_FIRE = 1,
 
} exit_typedef_enum;

typedef enum 
{
    EXIT_MODE_GPIO = 0,
    EXIT_MODE_EXTI = 1
} exit_mode_typedef_enum;

typedef enum 
{
  IIC1 = 0,
  IIC2 = 1,
  IIC3 = 2,
}IIC_TypeDef;

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

#define RUN_LED_PIN                         GPIO_PIN_13
#define RUN_LED_GPIO_PORT                   GPIOB
#define RUN_LED_GPIO_CLK                    RCU_GPIOB
  
#define ERR_LED_PIN                         GPIO_PIN_14
#define ERR_LED_GPIO_PORT                   GPIOB
#define ERR_LED_GPIO_CLK                    RCU_GPIOB

#define COMn                             3U

#define USART0_TX_PIN                 GPIO_PIN_9
#define USART0_RX_PIN                 GPIO_PIN_10
#define USART0_GPIO_PORT              GPIOA
#define USART0_GPIO_CLK               RCU_GPIOA
#define USART0_GPIO_REMAP             GPIO_USART0_REMAP

#define RS485_EN_PIN									GPIO_PIN_4
#define RS485_EN_GPIO_PORT            GPIOA
#define RS485_EN_GPIO_CLK             RCU_GPIOA


#define USART1_TX_PIN                 GPIO_PIN_2
#define USART1_RX_PIN                 GPIO_PIN_3
#define USART1_GPIO_PORT              GPIOA
#define USART1_GPIO_CLK               RCU_GPIOA

#define USART2_TX_PIN                 GPIO_PIN_10
#define USART2_RX_PIN                 GPIO_PIN_11
#define USART2_GPIO_PORT              GPIOB
#define USART2_GPIO_CLK               RCU_GPIOB

#define EXITn                             2U

/* WATER push-button */
#define WATER_EXIT_PIN                GPIO_PIN_6
#define WATER_EXIT_GPIO_PORT          GPIOA
#define WATER_EXIT_GPIO_CLK           RCU_GPIOA
#define WATER_EXTI_LINE            	  EXTI_6
#define WATER_EXTI_PORT_SOURCE        GPIO_PORT_SOURCE_GPIOA
#define WATER_EXTI_PIN_SOURCE         GPIO_PIN_SOURCE_6
#define WATER_EXTI_IRQn             	EXTI5_9_IRQn  

/* FIRE push-button */
#define FIRE_EXIT_PIN                 GPIO_PIN_12
#define FIRE_EXIT_GPIO_PORT           GPIOB
#define FIRE_EXIT_GPIO_CLK            RCU_GPIOB
#define FIRE_EXTI_LINE                EXTI_12
#define FIRE_EXTI_PORT_SOURCE         GPIO_PORT_SOURCE_GPIOB
#define FIRE_EXTI_PIN_SOURCE          GPIO_PIN_SOURCE_12
#define FIRE_EXTI_IRQn                EXTI10_15_IRQn

/* user push-button */
#define USER_EXIT_PIN                   GPIO_PIN_14
#define USER_EXIT_GPIO_PORT             GPIOB
#define USER_EXIT_GPIO_CLK              RCU_GPIOB
#define USER_EXTI_LINE               		EXTI_14
#define USER_EXTI_PORT_SOURCE        		GPIO_PORT_SOURCE_GPIOB
#define USER_EXTI_PIN_SOURCE         		GPIO_PIN_SOURCE_14
#define USER_EXTI_IRQn              		EXTI10_15_IRQn

/** @defgroup Direction_Light LOW LEVEL Direction_Light Constants
  * @{
  */
#define DIR_Ctrln                                    4
#define DIR_LEFT1_PIN                                GPIO_PIN_15
 #define DIR_LEFT1_GPIO_PORT                         GPIOA
#define DIR_LEFT1_GPIO_CLK               						 RCU_GPIOA

#define DIR_LEFT2_PIN                                GPIO_PIN_1
#define DIR_LEFT2_GPIO_PORT                          GPIOA
#define DIR_LEFT2_GPIO_CLK               						 RCU_GPIOA


#define DIR_RIGHT1_PIN                               GPIO_PIN_3
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

/** @defgroup Battery charge control pin
  * @{
  */

#define BATTERY_CHARGE_CONTROL_PIN                   GPIO_PIN_0
#define BATTERY_CHARGE_CONTROL_GPIO_PORT             GPIOA
#define BATTERY_CHARGE_CONTROL_CLK               		 RCU_GPIOA

#define BATTERY_VOLTAGE_MONITOR_PIN                  GPIO_PIN_1
#define BATTERY_VOLTAGE_MONITOR_GPIO_PORT            GPIOB
#define BATTERY_VOLTAGE_MONITOR_CLK               	 RCU_GPIOB

/* eval board dsl_08 control pin */
#define DSL_SET_PIN                      GPIO_PIN_8
#define DSL_SET_GPIO_PORT                GPIOA
#define DSL_SET_GPIO_CLK                 RCU_GPIOA

#define DSL_RESET_PIN                    GPIO_PIN_2
#define DSL_RESET_GPIO_PORT              GPIOB
#define DSL_RESET_GPIO_CLK               RCU_GPIOB

/* eval board usart channel select port */
#define USART2_S0_PIN                   GPIO_PIN_0
#define USART2_S0_PORT                  GPIOB
#define USART2_S0_CLK                   RCU_GPIOB

#define USART2_S1_PIN                   GPIO_PIN_7
#define USART2_S1_PORT                  GPIOA
#define USART2_S1_CLK                   RCU_GPIOA

/** @defgroup pump control pin */
#define Pump_Pin                        GPIO_PIN_8
#define Pump_Port												GPIOA
#define Pump_CLK												RCU_GPIOA



/** @defgroup STM32WLXX_NUCLEO_LOW_LEVEL_I2C LOW LEVEL I2C Constants
  * @{
  */
#define IICn                                    3

#define IIC1_SCL_PIN                           GPIO_PIN_6
#define IIC1_SCL_GPIO_PORT                     GPIOB
#define IIC1_SCL_GPIO_CLK                      RCU_GPIOB

#define IIC1_SDA_PIN                           GPIO_PIN_7
#define IIC1_SDA_GPIO_PORT                     GPIOB
#define IIC1_SDA_GPIO_CLK                      RCU_GPIOB


////IO方向设置
//#define SDA_IN_1()  {GPIOB->MODER&=~(3<<(8*2));GPIOB->MODER|=0<<8*2;}	//PB8输入模式
//#define SDA_OUT_1() {GPIOB->MODER&=~(3<<(8*2));GPIOB->MODER|=1<<8*2;} //PB8输出模式
////IO操作函数	 
//#define IIC_SCL_1    PAout(0) //SCL
//#define IIC_SDA_1    PBout(8) //SDA	 
//#define READ_SDA_1   PBin(8)  //输入SDA 

//IO方向设置
#define SDA_IN_1()  BSP_IIC_SDA_IN()	//PB8输入模式
#define SDA_OUT_1() BSP_IIC_SDA_OUT()//PB8输出模式
//IO操作函数	 
#define IIC1_SCL_HIGH()   gpio_bit_set(IIC1_SCL_GPIO_PORT,IIC1_SCL_PIN)
#define IIC1_SCL_LOW()    gpio_bit_reset(IIC1_SCL_GPIO_PORT,IIC1_SCL_PIN)
#define IIC1_SDA_HIGH()   gpio_bit_set(IIC1_SDA_GPIO_PORT,IIC1_SDA_PIN)
#define IIC1_SDA_LOW()    gpio_bit_reset(IIC1_SDA_GPIO_PORT,IIC1_SDA_PIN)

#define READ_IIC1_SDA   	gpio_input_bit_get(IIC1_SDA_GPIO_PORT,IIC1_SDA_PIN)  //输入SDA 

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
/* configure EXIT */
void gd_eval_EXIT_init(exit_typedef_enum EXIT_num, exit_mode_typedef_enum EXIT_mode);
/* return the selected EXIT state */
uint8_t gd_eval_EXIT_state_get(exit_typedef_enum EXIT);
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
/**initialize the battery charge controling pin .*/
void Charge_Control_GPIO_Init(void);
void Charge_Control(_Bool status);
void Battery_Voltage_Monitor_GPIO_Init(void);
/* @brief  Configures I2C GPIO.*/
void BSP_IIC_Init(IIC_TypeDef i2c);
/*@brief  Configures I2C SDA IO as Input Mode.*/
void BSP_IIC_SDA_IN(void);
/*@brief  Configures I2C SDA IO as Output Mode.. */
void BSP_IIC_SDA_OUT(void);
/* @brief  DeInit I2Cs. */
void BSP_IIC_DeInit(IIC_TypeDef i2c);
/* @brief configure control the port  of DSL_08*/
void dsl_board_init(void);
/* @brief      configure control the port  of DSL_08 */
void dsl_board_init(void);
/* @brief      configure USART CHANNEL SWITCH GPIO*/
void usart_ch_sw_init(void);
/* @brief      configure USART CHANNEL Select */
void usart_ch_sw_ctrl(uint32_t usart_periph,uint8_t ch);
/* @brief      configure Pump Control  GPIO*/
void pump_io_init(void);
/* @brief      configure Pump States Change  Function*/
void pump_states_change(_Bool state);
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

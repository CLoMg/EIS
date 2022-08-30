/*!
    \file  gd32e230c_eval.h
    \brief definitions for GD32E230C_EVAL's leds, keys and COM ports hardware resources
    
    \version 2018-06-19, V1.0.0, firmware for GD32E230
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

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

#ifndef GD32E230C_EVAL_H
#define GD32E230C_EVAL_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "gd32e230.h"

#define CHECK_ERROR(e) if(e != 0) return e;

/* exported types */
typedef enum 
{
    LED1 = 0,
    LED2 = 1,
    LED3 = 2,
    LED4 = 3
}led_typedef_enum;

typedef enum 
{
    KEY_WAKEUP = 0,
    KEY_TAMPER = 1,
}key_typedef_enum;

typedef enum 
{
    KEY_MODE_GPIO = 0,
    KEY_MODE_EXTI = 1
}keymode_typedef_enum;

typedef enum 
{
  IIC1 = 0,
  IIC2 = 1,
  IIC3 = 2,
}IIC_TypeDef;

typedef enum 
{
  MP_SW0 = 0,
  MP_SW1 = 1,
}MultiPlex_SW_TypeDef;

typedef enum 
{
  MP_CH0 = 0,
	MP_CH1,
	MP_CH2,
	MP_CH3 
}MultiPlex_CH_TypeDef;

//#define EVAL_Board 1
#define JT_Board 1

/* eval board low layer led */
#define LEDn                             4U

#define LED1_PIN                         GPIO_PIN_8
#define LED1_GPIO_PORT                   GPIOA
#define LED1_GPIO_CLK                    RCU_GPIOA

#define LED2_PIN                         GPIO_PIN_11
#define LED2_GPIO_PORT                   GPIOA
#define LED2_GPIO_CLK                    RCU_GPIOA

#define LED3_PIN                         GPIO_PIN_12
#define LED3_GPIO_PORT                   GPIOA
#define LED3_GPIO_CLK                    RCU_GPIOA

#define LED4_PIN                         GPIO_PIN_15
#define LED4_GPIO_PORT                   GPIOA
#define LED4_GPIO_CLK                    RCU_GPIOA

/* eval board low layer button */  
#define KEYn                             2U

/* wakeup push-button */
#define WAKEUP_KEY_PIN                   GPIO_PIN_0
#define WAKEUP_KEY_GPIO_PORT             GPIOA
#define WAKEUP_KEY_GPIO_CLK              RCU_GPIOA
#define WAKEUP_KEY_EXTI_LINE             EXTI_0
#define WAKEUP_KEY_EXTI_PORT_SOURCE      EXTI_SOURCE_GPIOA
#define WAKEUP_KEY_EXTI_PIN_SOURCE       EXTI_SOURCE_PIN0
#define WAKEUP_KEY_EXTI_IRQn             EXTI0_1_IRQn

/* tamper push-button */
#define TAMPER_KEY_PIN                   GPIO_PIN_13
#define TAMPER_KEY_GPIO_PORT             GPIOC
#define TAMPER_KEY_GPIO_CLK              RCU_GPIOC
#define TAMPER_KEY_EXTI_LINE             EXTI_13
#define TAMPER_KEY_EXTI_PORT_SOURCE      EXTI_SOURCE_GPIOC
#define TAMPER_KEY_EXTI_PIN_SOURCE       EXTI_SOURCE_PIN13
#define TAMPER_KEY_EXTI_IRQn             EXTI4_15_IRQn

/* eval board low layer COM */
#define COMn                             1U

/* definition for COM, connected to USART0 */
#define EVAL_COM                         USART0
#define EVAL_COM_CLK                     RCU_USART0

#define EVAL_COM_TX_PIN                  GPIO_PIN_2
#define EVAL_COM_RX_PIN                  GPIO_PIN_3

#define EVAL_COM_GPIO_PORT               GPIOA
#define EVAL_COM_GPIO_CLK                RCU_GPIOA
#define EVAL_COM_AF                      GPIO_AF_1

/* eval board dsl_08 control pin */
#define DSL_SET_PIN                      GPIO_PIN_8
#define DSL_SET_GPIO_PORT                GPIOA
#define DSL_SET_GPIO_CLK                 RCU_GPIOA

#define DSL_RESET_PIN                    GPIO_PIN_11
#define DSL_RESET_GPIO_PORT              GPIOA
#define DSL_RESET_GPIO_CLK               RCU_GPIOA

/* eval board usart channel select port */
#define USART0_S0_PIN                   GPIO_PIN_4
#define USART0_S0_PORT                  GPIOA
#define USART0_S0_CLK                   RCU_GPIOA

#define USART0_S1_PIN                   GPIO_PIN_1
#define USART0_S1_PORT                  GPIOA
#define USART0_S1_CLK                   RCU_GPIOA

#define USART1_S0_PIN                   GPIO_PIN_1
#define USART1_S0_PORT                  GPIOA
#define USART1_S0_CLK                   RCU_GPIOA

#define USART1_S1_PIN                   GPIO_PIN_4
#define USART1_S1_PORT                  GPIOA
#define USART1_S1_CLK                   RCU_GPIOA

/** @defgroup STM32WLXX_NUCLEO_LOW_LEVEL_I2C LOW LEVEL I2C Constants
  * @{
  */
#define IICn                                    3

#define IIC1_SCL_PIN                           GPIO_PIN_5
#define IIC1_SCL_GPIO_PORT                     GPIOB
#define IIC1_SCL_GPIO_CLK                      RCU_GPIOB

#define IIC1_SDA_PIN                           GPIO_PIN_4
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
#define SDA_IN_1()  BSP_IIC_SDA_IN(0)	//PB8输入模式
#define SDA_OUT_1() BSP_IIC_SDA_OUT(0)//PB8输出模式
//IO操作函数	 
#define IIC1_SCL_HIGH()   gpio_bit_set(IIC1_SCL_GPIO_PORT,IIC1_SCL_PIN)
#define IIC1_SCL_LOW()    gpio_bit_reset(IIC1_SCL_GPIO_PORT,IIC1_SCL_PIN)
#define IIC1_SDA_HIGH()   gpio_bit_set(IIC1_SDA_GPIO_PORT,IIC1_SDA_PIN)
#define IIC1_SDA_LOW()    gpio_bit_reset(IIC1_SDA_GPIO_PORT,IIC1_SDA_PIN)

#define READ_IIC1_SDA   	gpio_input_bit_get(IIC1_SDA_GPIO_PORT,IIC1_SDA_PIN)  //输入SDA 

/** @defgroup STM32WLXX_NUCLEO_LOW_LEVEL_Multi-Plexer LOW LEVEL Multi-Plexer Constants
  * @{
  */
#define MP_SWn                                   2
#define MP_S0_PIN                                GPIO_PIN_1
#define MP_S0_GPIO_PORT                          GPIOA
#define MP_S0_GPIO_CLK                 					 RCU_GPIOA

#define MP_S1_PIN                                GPIO_PIN_4
#define MP_S1_GPIO_PORT                          GPIOA
#define MP_S1_GPIO_CLK                					 RCU_GPIOA




#ifdef JT_Board
#define Pump_Pin                        GPIO_PIN_15
#define Pump_Port												GPIOA
#define Pump_CLK												RCU_GPIOA
#endif

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
void gd_eval_key_init(key_typedef_enum keynum, keymode_typedef_enum keymode);
/* return the selected key state */
uint8_t gd_eval_key_state_get(key_typedef_enum keynum);
/* configure COM port */
void gd_eval_com_init(uint32_t com);
/*configure ADC PORT*/
void adc_gpio_config(void);
/*config usart port */
void usart_gpio_init(uint8_t usart_num);
/*config DSL_08 Control pin*/
void dsl_board_init();
/*config timer0 parameter*/
void timer0_config(uint16_t prescaler,uint16_t period);
void delay_init(void);
/*declare delay_nus function*/
void delay_us(uint32_t n_us);
/*declare delay_nms function*/
void delay_ms(uint16_t n_ms);
/*configure USART CHANNEL SWITCH GPIO*/
void usart_ch_sw_init(void);
/*declare usart channel select function*/
void usart_ch_sw_ctrl(uint32_t usart_periph,uint8_t ch);
/*configure Pump Control  GPIO*/
void pump_io_init(void);
/*configure Pump States Change  Function*/
void pump_states_change(_Bool state);
/* @brief  Configures I2C GPIO.*/
void BSP_IIC_Init(IIC_TypeDef i2c);
/*@brief  Configures I2C SDA IO as Input Mode.*/
void BSP_IIC_SDA_IN(IIC_TypeDef i2c);
/*@brief  Configures I2C SDA IO as Output Mode.. */
void BSP_IIC_SDA_OUT(IIC_TypeDef i2c);
/* @brief  DeInit I2Cs. */
void BSP_IIC_DeInit(IIC_TypeDef i2c);
/** @defgroup STM32WLXX_NUCLEO_LOW_LEVEL_LED_Functions LOW LEVEL LED Functions
  * @{
  */
void BSP_MultiPlexer_Init(void);
void BSP_MultiPlexer_DeInit(void);
void BSP_MP_SW_On(uint32_t sw);
void BSP_MP_SW_Off(uint32_t sw);
void BSP_MP_Chnnel_Set(MultiPlex_CH_TypeDef ch);

/*
 * Max 4294967295(us)=4294967(us)=4294(s)=71(min)
 */
uint32_t get_systick_us(void);
/*
 * Max 4294967295(ms)=4294967(s)=71582(min)=1193(hour)=49(day)
 */
uint32_t get_systick_ms(void);
void systick_init(void);
 
#ifdef __cplusplus
}
#endif

#endif /* GD32E230C_EVAL_H */


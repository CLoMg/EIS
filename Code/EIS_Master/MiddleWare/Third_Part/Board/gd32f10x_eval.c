/*!
    \file    gd32f10x_eval.c
    \brief   firmware functions to manage leds, keys, COM ports

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

#include "gd32f10x_eval.h"

/* private variables */
static uint32_t LED_CLK[LEDn] = {RUN_LED_GPIO_CLK, ERR_LED_GPIO_CLK};
static uint32_t LED_PORT[LEDn] = {RUN_LED_GPIO_PORT, ERR_LED_GPIO_PORT};
static uint32_t LED_PIN[LEDn] = {RUN_LED_PIN,RUN_LED_PIN};

static uint32_t DIR_LIGHT_CLK[DIR_Ctrln] = {DIR_LEFT1_GPIO_CLK, DIR_LEFT2_GPIO_CLK,
																		DIR_RIGHT1_GPIO_CLK, DIR_RIGHT2_GPIO_CLK};
static uint32_t DIR_LIGHT_PORT[DIR_Ctrln] = {DIR_LEFT1_GPIO_PORT, DIR_LEFT2_GPIO_PORT,
																		DIR_RIGHT1_GPIO_PORT, DIR_RIGHT2_GPIO_PORT};
static uint32_t DIR_LIGHT_PIN[DIR_Ctrln] = {DIR_LEFT1_PIN, DIR_LEFT2_PIN, DIR_RIGHT1_PIN, DIR_RIGHT2_PIN};


static uint32_t KEY_PORT[KEYn] = {WAKEUP_KEY_GPIO_PORT, 
                                  TAMPER_KEY_GPIO_PORT,
                                  USER_KEY_GPIO_PORT};
static uint32_t KEY_PIN[KEYn] = {WAKEUP_KEY_PIN,
                                 TAMPER_KEY_PIN,
                                 USER_KEY_PIN};
static rcu_periph_enum KEY_CLK[KEYn] = {WAKEUP_KEY_GPIO_CLK, 
                                        TAMPER_KEY_GPIO_CLK,
                                        USER_KEY_GPIO_CLK};
static exti_line_enum KEY_EXTI_LINE[KEYn] = {WAKEUP_KEY_EXTI_LINE,
                                             TAMPER_KEY_EXTI_LINE,
                                             USER_KEY_EXTI_LINE};
static uint8_t KEY_PORT_SOURCE[KEYn] = {WAKEUP_KEY_EXTI_PORT_SOURCE,
                                        TAMPER_KEY_EXTI_PORT_SOURCE,
                                        USER_KEY_EXTI_PORT_SOURCE};
static uint8_t KEY_PIN_SOURCE[KEYn] = {WAKEUP_KEY_EXTI_PIN_SOURCE,
                                       TAMPER_KEY_EXTI_PIN_SOURCE,
                                       USER_KEY_EXTI_PIN_SOURCE};
static uint8_t KEY_IRQn[KEYn] = {WAKEUP_KEY_EXTI_IRQn, 
                                 TAMPER_KEY_EXTI_IRQn,
                                 USER_KEY_EXTI_IRQn};

/*!
    \brief      configure led GPIO
    \param[in]  lednum: specify the led to be configured
      \arg        LED2
      \arg        LED3
      \arg        LED4
      \arg        LED5
    \param[out] none
    \retval     none
*/
void gd_eval_led_init(led_typedef_enum lednum)
{
    /* enable the led clock */
    rcu_periph_clock_enable((rcu_periph_enum)LED_CLK[lednum]);
    /* configure led GPIO port */ 
    gpio_init(LED_PORT[lednum], GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED_PIN[lednum]);

    GPIO_BC(LED_PORT[lednum]) = LED_PIN[lednum];
}

/*!
    \brief      turn on selected led
    \param[in]  lednum: specify the led to be turned on
      \arg        LED2
      \arg        LED3
      \arg        LED4
      \arg        LED5
    \param[out] none
    \retval     none
*/
void gd_eval_led_on(led_typedef_enum lednum)
{
    GPIO_BOP(LED_PORT[lednum]) = LED_PIN[lednum];
}

/*!
    \brief      turn off selected led
    \param[in]  lednum: specify the led to be turned off
      \arg        LED2
      \arg        LED3
      \arg        LED4
      \arg        LED5
    \param[out] none
    \retval     none
*/
void gd_eval_led_off(led_typedef_enum lednum)
{
    GPIO_BC(LED_PORT[lednum]) = LED_PIN[lednum];
}

/*!
    \brief      toggle selected led
    \param[in]  lednum: specify the led to be toggled
      \arg        LED2
      \arg        LED3
      \arg        LED4
      \arg        LED5
    \param[out] none
    \retval     none
*/
void gd_eval_led_toggle(led_typedef_enum lednum)
{
    gpio_bit_write(LED_PORT[lednum], LED_PIN[lednum], 
    (bit_status)(1-gpio_input_bit_get(LED_PORT[lednum], LED_PIN[lednum])));
}

/*!
    \brief      configure key
    \param[in]  key_num: specify the key to be configured
      \arg        KEY_TAMPER: tamper key
      \arg        KEY_WAKEUP: wakeup key
      \arg        KEY_USER: user key
    \param[in]  key_mode: specify button mode
      \arg        KEY_MODE_GPIO: key will be used as simple IO
      \arg        KEY_MODE_EXTI: key will be connected to EXTI line with interrupt
    \param[out] none
    \retval     none
*/
void gd_eval_key_init(key_typedef_enum key_num, keymode_typedef_enum key_mode)
{
    /* enable the key clock */
    rcu_periph_clock_enable(KEY_CLK[key_num]);
    rcu_periph_clock_enable(RCU_AF);

    /* configure button pin as input */
    gpio_init(KEY_PORT[key_num], GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, KEY_PIN[key_num]);

    if (key_mode == KEY_MODE_EXTI) {
        /* enable and set key EXTI interrupt to the lowest priority */
        nvic_irq_enable(KEY_IRQn[key_num], 2U, 0U);

        /* connect key EXTI line to key GPIO pin */
        gpio_exti_source_select(KEY_PORT_SOURCE[key_num], KEY_PIN_SOURCE[key_num]);

        /* configure key EXTI line */
        exti_init(KEY_EXTI_LINE[key_num], EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        exti_interrupt_flag_clear(KEY_EXTI_LINE[key_num]);
    }
}

/*!
    \brief      return the selected key state
    \param[in]  key: specify the key to be checked
      \arg        KEY_TAMPER: tamper key
      \arg        KEY_WAKEUP: wakeup key
      \arg        KEY_USER: user key
    \param[out] none
    \retval     the key's GPIO pin value
*/
uint8_t gd_eval_key_state_get(key_typedef_enum key)
{
    return gpio_input_bit_get(KEY_PORT[key], KEY_PIN[key]);
}

/*!
    \brief      configure COM port
    \param[in]  com: COM on the board
      \arg        EVAL_COM0: COM0 on the board
      \arg        EVAL_COM1: COM1 on the board
    \param[out] none
    \retval     none
*/
void gd_usart_gpio_init(uint8_t usart_num)
{

	switch(usart_num){
		case 0:
			rcu_periph_clock_enable(RCU_AF);
	
			rcu_periph_clock_enable(USART0_GPIO_CLK);
	
			gpio_pin_remap_config(USART0_GPIO_REMAP,ENABLE);
		
			//rcu_periph_clock_enable(RCU_USART0);
	
			gpio_init(USART0_GPIO_PORT,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ, USART0_TX_PIN);
	
			gpio_init(USART0_GPIO_PORT,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,USART0_RX_PIN);
		
			//gpio_pin_remap_config(GPIO_USART0_REMAP,ENABLE);
		break;
		case 1:
			gpio_init(USART1_GPIO_PORT,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ, USART1_TX_PIN);
	
			gpio_init(USART1_GPIO_PORT,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,USART1_RX_PIN);
		break;
			default:
				break;
			
		}
}

/*!
    \brief      configure RS485_EN port
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gd_rs485_en_init(void)
{
			rcu_periph_clock_enable(RS485_EN_GPIO_CLK);
			/* configure led GPIO port */ 
			gpio_init(RS485_EN_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RS485_EN_PIN);

			GPIO_BC(RS485_EN_GPIO_PORT) = RS485_EN_PIN;
}

/*!
    \brief      configure RS485_EN port
		\param[in]  states: 0-disable 1-enable 
    \param[out] none
    \retval     none
*/
void gd_rs485_en_control(_Bool states)
{
//	uint8_t timeout = 200;
	if(states == 1){
		GPIO_BOP(RS485_EN_GPIO_PORT) = RS485_EN_PIN;
	}
	else
		GPIO_BC(RS485_EN_GPIO_PORT) = RS485_EN_PIN;
}


/**
  * @}
  */ 
/** @addtogroup STM32WLXX_NUCLEO_LOW_LEVEL_Direction_Light_Functions
  * @{
  */ 

/**
  * @brief  Configures Direction_Light Control Ports.
  * @param  void
  * @retval Direction Light initialize status
  */
int32_t BSP_Dir_Light_Init(void)
{
		uint8_t Light_Num = 0;
		
		for(;Light_Num< 4;Light_Num++){
			  /* enable the led clock */
			rcu_periph_clock_enable((rcu_periph_enum)DIR_LIGHT_CLK[Light_Num]);
			if(Light_Num ==0){
				rcu_periph_clock_enable(RCU_AF);
				gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP,ENABLE);
			
			}
			/* configure led GPIO port */ 
			gpio_init(DIR_LIGHT_PORT[Light_Num], GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, DIR_LIGHT_PIN[Light_Num]);

			GPIO_BOP(DIR_LIGHT_PORT[Light_Num]) = DIR_LIGHT_PIN[Light_Num];
		}
  return 1;
}

/**
  * @brief  DeInit Direction Light.
  * @param  void
  * @note Direction Light DeInit does not disable the GPIO clock nor disable the Direction Light 
  * @retval BSP status
  */
int32_t BSP_Dir_Light_DeInit(void)
{
 
  return 1;
}

/**
  * @brief  control the direction lights status.
  * @param  dir: the direction which will be lighten
  *            @arg both
  *            @arg left
  *            @arg right
  * @retval BSP status
  */
int32_t BSP_Light_Control(BSP_Direction_TypeDef dir)
{
  switch(dir){
		case All_OFF:
		{
			GPIO_BC(DIR_LEFT1_GPIO_PORT) = DIR_LEFT1_PIN;
			GPIO_BC(DIR_LEFT2_GPIO_PORT) = DIR_LEFT2_PIN;
			GPIO_BC(DIR_RIGHT1_GPIO_PORT) = DIR_RIGHT1_PIN;
			GPIO_BC(DIR_RIGHT2_GPIO_PORT) = DIR_RIGHT2_PIN;
		}
		break;
		case Left:
		{
			GPIO_BOP(DIR_LEFT1_GPIO_PORT) = DIR_LEFT1_PIN;
			GPIO_BOP(DIR_LEFT2_GPIO_PORT) = DIR_LEFT2_PIN;
			GPIO_BC(DIR_RIGHT1_GPIO_PORT) = DIR_RIGHT1_PIN;
			GPIO_BC(DIR_RIGHT2_GPIO_PORT) = DIR_RIGHT2_PIN;
		}
		break;
		case Right:
		{
			GPIO_BC(DIR_LEFT1_GPIO_PORT) = DIR_LEFT1_PIN;
			GPIO_BC(DIR_LEFT2_GPIO_PORT) = DIR_LEFT2_PIN;
			GPIO_BOP(DIR_RIGHT1_GPIO_PORT) = DIR_RIGHT1_PIN;
			GPIO_BOP(DIR_RIGHT2_GPIO_PORT) = DIR_RIGHT2_PIN;
		}
		break;
	  case Both:
		{
			GPIO_BOP(DIR_LEFT1_GPIO_PORT) = DIR_LEFT1_PIN;
			GPIO_BOP(DIR_LEFT2_GPIO_PORT) = DIR_LEFT2_PIN;
			GPIO_BOP(DIR_RIGHT1_GPIO_PORT) = DIR_RIGHT1_PIN;
			GPIO_BOP(DIR_RIGHT2_GPIO_PORT) = DIR_RIGHT2_PIN;
		}
		break;
		default:
		break;
	}
  return 1;
}


/**
  * @brief  get the direction lights status.
  * @param  none
  * @retval dir: the direction which will be lighten(off/left/right/both)
  */
BSP_Direction_TypeDef BSP_Light_Get(void)
{
	BSP_Direction_TypeDef dir;
	FlagStatus light_status[4];
	uint8_t i=0;
	for(;i<4;++i)
	 light_status[i] = gpio_output_bit_get(DIR_LIGHT_PORT[i], DIR_LIGHT_PIN[i]);
	
	if((light_status[0] == 0) &&(light_status[2] == 0))
		dir = All_OFF;
	else if((light_status[0] == 1) &&(light_status[2] == 0))
		dir = Left;
	else if((light_status[0] == 0) &&(light_status[2] == 1))
		dir = Right;
	else if((light_status[0] == 1) &&(light_status[2] == 1))
		dir = Both;
	else
		dir = Error;
  return dir;
}


/**
  * @brief  control the direction lights status.
  * @param  dir: the direction which will be lighten
  *            @arg both
  *            @arg left
  *            @arg right
  * @retval BSP status
  */
void LORA_Module_GPIO_Init(void)
{
			rcu_periph_clock_enable(LORA_RESET_CLK);
			/* configure RS485_EN port */ 
			gpio_init(LORA_RESET_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LORA_RESET_PIN);

			GPIO_BOP(LORA_RESET_GPIO_PORT) =LORA_RESET_PIN;
	
}

void LORA_Module_Reset_Control(_Bool state){
	if(state)
		GPIO_BOP(LORA_RESET_GPIO_PORT) = LORA_RESET_PIN;
	else
		GPIO_BC(LORA_RESET_GPIO_PORT)  = LORA_RESET_PIN;
}

void LORA_Module_Wake_Control(_Bool state){
	if(state)
		GPIO_BOP(LORA_WAKE_GPIO_PORT) = LORA_WAKE_PIN;
	else
		GPIO_BC(LORA_WAKE_GPIO_PORT)  = LORA_WAKE_PIN;
}


/*!
    \brief      configure SPI port
    \param[in]  none
    \param[out] none
    \retval     none
*/
//void spi_gpio_init(void)
//{
//			rcu_periph_clock_enable(SPI1_GPIO_CLK);
//		    
//			
//			gpio_init(SPI1_GPIO_PORT,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ, SPI1_MOSI_PIN|SPI1_MISO_PIN|SPI1_SCK_PIN);		
//			gpio_bit_reset(SPI1_GPIO_PORT,SPI1_MOSI_PIN|SPI1_MISO_PIN|SPI1_SCK_PIN);
//	
//			gpio_init(SPI1_GPIO_PORT,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,SPI1_NSS_PIN);
//			gpio_bit_set(SPI1_GPIO_PORT,SPI1_NSS_PIN);
//}

//void  spi_ss_control(_Bool state)
//{
//	if(state)
//		GPIO_BC(SPI1_GPIO_PORT) = SPI1_NSS_PIN;
//	else
//		GPIO_BOP(SPI1_GPIO_PORT) = SPI1_NSS_PIN;

//}


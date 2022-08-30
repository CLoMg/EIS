/*!
    \file  gd32e230c_eval.c
    \brief firmware functions to manage leds, keys, COM ports
    
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

#include "gd32e230c_eval.h"
#include "FreeRTOS.h"
#include "task.h"

/* private variables */
static const uint32_t GPIO_PORT[LEDn]       = {LED1_GPIO_PORT,
                                               LED2_GPIO_PORT,
                                               LED3_GPIO_PORT,
                                               LED4_GPIO_PORT};

static const uint32_t GPIO_PIN[LEDn]        = {LED1_PIN,
                                               LED2_PIN,
                                               LED3_PIN,
                                               LED4_PIN};

static const rcu_periph_enum COM_CLK[COMn]  = {EVAL_COM_CLK};

static const uint32_t COM_TX_PIN[COMn]      = {EVAL_COM_TX_PIN};

static const uint32_t COM_RX_PIN[COMn]      = {EVAL_COM_RX_PIN};

static const rcu_periph_enum GPIO_CLK[LEDn] = {LED1_GPIO_CLK,
                                               LED2_GPIO_CLK,
                                               LED3_GPIO_CLK,
                                               LED4_GPIO_CLK};

static const uint32_t KEY_PORT[KEYn]        = {WAKEUP_KEY_GPIO_PORT, 
                                               TAMPER_KEY_GPIO_PORT
                                               };

static const uint32_t KEY_PIN[KEYn]         = {WAKEUP_KEY_PIN, 
                                               TAMPER_KEY_PIN
                                               };

static const rcu_periph_enum KEY_CLK[KEYn]  = {WAKEUP_KEY_GPIO_CLK, 
                                               TAMPER_KEY_GPIO_CLK
                                               };

static const exti_line_enum KEY_EXTI_LINE[KEYn] = {WAKEUP_KEY_EXTI_LINE,
                                                   TAMPER_KEY_EXTI_LINE
                                                   };

static const uint8_t KEY_PORT_SOURCE[KEYn]      = {WAKEUP_KEY_EXTI_PORT_SOURCE,
                                                   TAMPER_KEY_EXTI_PORT_SOURCE
                                                   };

static const uint8_t KEY_PIN_SOURCE[KEYn]       = {WAKEUP_KEY_EXTI_PIN_SOURCE,
                                                   TAMPER_KEY_EXTI_PIN_SOURCE
                                                   };

static const uint8_t KEY_IRQn[KEYn]             = {WAKEUP_KEY_EXTI_IRQn, 
                                                   TAMPER_KEY_EXTI_IRQn
                                                   };

static const uint32_t  MultiPlex_SW_PORT[MP_SWn] = {MP_S0_GPIO_PORT, MP_S1_GPIO_PORT,};

static const uint32_t  MultiPlex_SW_PIN[MP_SWn] = {MP_S0_PIN,MP_S1_PIN};

/* eval board low layer private functions */
/*!
    \brief      configure led GPIO
    \param[in]  lednum: specify the led to be configured
      \arg        LED1
      \arg        LED2
      \arg        LED3
      \arg        LED4
    \param[out] none
    \retval     none
*/
void gd_eval_led_init(led_typedef_enum lednum)
{
    /* enable the led clock */
    rcu_periph_clock_enable(GPIO_CLK[lednum]);
    /* configure led GPIO port */ 
    gpio_mode_set(GPIO_PORT[lednum], GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN[lednum]);
    gpio_output_options_set(GPIO_PORT[lednum], GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN[lednum]);

    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      turn on selected led
    \param[in]  lednum: specify the led to be turned on
      \arg        LED1
      \arg        LED2
      \arg        LED3
      \arg        LED4
    \param[out] none
    \retval     none
*/
void gd_eval_led_on(led_typedef_enum lednum)
{
    GPIO_BOP(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      turn off selected led
    \param[in]  lednum: specify the led to be turned off
      \arg        LED1
      \arg        LED2
      \arg        LED3
      \arg        LED4
    \param[out] none
    \retval     none
*/
void gd_eval_led_off(led_typedef_enum lednum)
{
    GPIO_BC(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      toggle selected led
    \param[in]  lednum: specify the led to be toggled
      \arg        LED1
      \arg        LED2
      \arg        LED3
      \arg        LED4
    \param[out] none
    \retval     none
*/
void gd_eval_led_toggle(led_typedef_enum lednum)
{
    GPIO_TG(GPIO_PORT[lednum]) = GPIO_PIN[lednum];
}

/*!
    \brief      configure key
    \param[in]  keynum: specify the key to be configured
      \arg        KEY_TAMPER: tamper key
      \arg        KEY_WAKEUP: wakeup key
    \param[in]  keymode: specify button mode
      \arg        KEY_MODE_GPIO: key will be used as simple IO
      \arg        KEY_MODE_EXTI: key will be connected to EXTI line with interrupt
    \param[out] none
    \retval     none
*/
void gd_eval_key_init(key_typedef_enum keynum, keymode_typedef_enum keymode)
{
    /* enable the key clock */
    rcu_periph_clock_enable(KEY_CLK[keynum]);
    rcu_periph_clock_enable(RCU_CFGCMP);

    /* configure button pin as input */
    gpio_mode_set(KEY_PORT[keynum], GPIO_MODE_INPUT, GPIO_PUPD_NONE, KEY_PIN[keynum]);

    if (keymode == KEY_MODE_EXTI) {
        /* enable and set key EXTI interrupt to the lowest priority */
        nvic_irq_enable(KEY_IRQn[keynum], 2U);

        /* connect key EXTI line to key GPIO pin */
        syscfg_exti_line_config(KEY_PORT_SOURCE[keynum], KEY_PIN_SOURCE[keynum]);

        /* configure key EXTI line */
        exti_init(KEY_EXTI_LINE[keynum], EXTI_INTERRUPT, EXTI_TRIG_FALLING);
        exti_interrupt_flag_clear(KEY_EXTI_LINE[keynum]);
    }
}

/*!
    \brief      return the selected key state
    \param[in]  keynum: specify the key to be checked
      \arg        KEY_TAMPER: tamper key
      \arg        KEY_WAKEUP: wakeup key
    \param[out] none
    \retval     the key's GPIO pin value
*/
uint8_t gd_eval_key_state_get(key_typedef_enum keynum)
{
    return gpio_input_bit_get(KEY_PORT[keynum], KEY_PIN[keynum]);
}

/*!
    \brief      configure COM port
    \param[in]  com: COM on the board
      \arg        EVAL_COM: COM on the board
    \param[out] none
    \retval     none
*/
void gd_eval_com_init(uint32_t com)
{
    uint32_t COM_ID;
    
    COM_ID = 0U;

    /* enable COM GPIO clock */
    rcu_periph_clock_enable(EVAL_COM_GPIO_CLK);

    /* enable USART clock */
    rcu_periph_clock_enable(COM_CLK[COM_ID]);

    /* connect port to USARTx_Tx */
    gpio_af_set(EVAL_COM_GPIO_PORT, EVAL_COM_AF, COM_TX_PIN[COM_ID]);

    /* connect port to USARTx_Rx */
    gpio_af_set(EVAL_COM_GPIO_PORT, EVAL_COM_AF, COM_RX_PIN[COM_ID]);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(EVAL_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_TX_PIN[COM_ID]);
    gpio_output_options_set(EVAL_COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, COM_TX_PIN[COM_ID]);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(EVAL_COM_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_RX_PIN[COM_ID]);
    gpio_output_options_set(EVAL_COM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, COM_RX_PIN[COM_ID]);

    /* USART configure */
    usart_deinit(com);
    usart_baudrate_set(com, 115200U);
    usart_receive_config(com, USART_RECEIVE_ENABLE);
    usart_transmit_config(com, USART_TRANSMIT_ENABLE);

    usart_enable(com);
}
/*!
    \brief      ADC GPIO configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_gpio_config(void)
{
	  /* enable the led clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* configure PA4(ADC channel4) as analog input */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_7);
}

/*!
    \brief      initilize the Usart GPIO
    \param[in]  usart_num 0-Usart0 1-Usart1
    \param[out] none
    \retval     none
*/
void usart_gpio_init(uint8_t usart_num)
{
    switch(usart_num){
			case 0:
				/* enable COM GPIO clock */
				rcu_periph_clock_enable(RCU_GPIOA);

				/* connect port to USARTx_Tx */
				gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);

				/* connect port to USARTx_Rx */
				gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);

				/* configure USART Tx as alternate function push-pull */
				gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
				gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9);

				/* configure USART Rx as alternate function push-pull */
				gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
				gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_10);
				break;
			case 1:
				/* enable COM GPIO clock */
				rcu_periph_clock_enable(RCU_GPIOA);

				/* connect port to USARTx_Tx */
				gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2);

				/* connect port to USARTx_Rx */
				gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_3);

				/* configure USART Tx as alternate function push-pull */
				gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
				gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_2);

				/* configure USART Rx as alternate function push-pull */
				gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
				gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_3);
				break;
			default:
				break;
			
		}
}

/*!
    \brief      configure control the port  of DSL_08
		\brief      SET : 0 - PowerOff  / 1 - Work
		\brief      RESET: 0 - Valid 
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dsl_board_init(void)
{
//    /* enable the dsl_08 set pin clock */
//    rcu_periph_clock_enable(DSL_SET_GPIO_CLK);
//    /* configure led GPIO port */ 
//    gpio_mode_set(DSL_SET_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, DSL_SET_PIN);
//    gpio_output_options_set(DSL_SET_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,DSL_SET_PIN);

//    GPIO_TG(DSL_SET_GPIO_PORT) = DSL_SET_PIN;
	
	    /* enable the dsl_08 set pin clock */
    rcu_periph_clock_enable(DSL_RESET_GPIO_CLK);
    /* configure led GPIO port */ 
    gpio_mode_set(DSL_RESET_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, DSL_RESET_PIN);
    gpio_output_options_set(DSL_RESET_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,DSL_RESET_PIN);

    GPIO_TG(DSL_RESET_GPIO_PORT) = DSL_RESET_PIN;
}

/*!
    \brief      configure timer0
    \param[in]  prescaler,period
    \  update_time = period / (timer0_clk/(prescaler+1))
    \  prescaler =0 period =72 update_time = 1us
    \param[out] none
    \retval     none
*/
void timer0_config(uint16_t prescaler,uint16_t period)
{
	timer_parameter_struct timer_initpara;
	rcu_periph_clock_enable(RCU_TIMER14);
	
	timer_deinit(TIMER14);
	timer_initpara.prescaler = prescaler;
	timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection = TIMER_COUNTER_UP;
	timer_initpara.period = period;
	timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_init(TIMER14,&timer_initpara);
	
	timer_auto_reload_shadow_enable(TIMER14);
	timer_flag_clear(TIMER14,TIMER_FLAG_UP);
}



void delay_init(void)
{
	timer0_config(0,72);
}

/*!
    \brief     delay_nus
    \param[in]  n_us -- counts of us while should be waited 
    \param[out] none
    \retval     none
*/
void delay_us(uint32_t n_us)
{
	timer_enable(TIMER14);
	while(n_us--)
	{
		while(timer_flag_get(TIMER14,TIMER_FLAG_UP) == RESET)
		{
			;
		}
		timer_flag_clear(TIMER14,TIMER_FLAG_UP);
	}
	timer_disable(TIMER14);

}
/*利用systick 计数器延时*/
//void delay_us(uint32_t us)
//{
//	uint32_t temp;
//	SysTick->LOAD = 72/8*us;
//  SysTick->VAL=0X00;//清空计数器
//  SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
//  do
//  {
//    temp=SysTick->CTRL;//读取当前倒计数值
//  }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
//  SysTick->CTRL=0x00; //关闭计数器
//  SysTick->VAL =0X00; //清空计数器
//}
/*!
    \brief     delay_ms
    \param[in]  n_ms -- counts of ms while should be waited 
    \param[out] none
    \retval     none
*/
void delay_ms(uint16_t n_ms)
{
//		while(n_ms--)
//			delay_us(1000);
	vTaskDelay(200/portTICK_RATE_MS);
}

/*!
    \brief      configure USART CHANNEL SWITCH GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usart_ch_sw_init(void)
{
    /* enable the USART Channel Switch clock */
    rcu_periph_clock_enable(USART0_S0_CLK);
		rcu_periph_clock_enable(USART0_S1_CLK);
	
//	  rcu_periph_clock_enable(USART1_S0_CLK);
//		rcu_periph_clock_enable(USART1_S1_CLK);
    /* configure USART Channel Switch GPIO port */ 
    gpio_mode_set(USART0_S0_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,USART0_S0_PIN);
    gpio_output_options_set(USART0_S0_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,USART0_S0_PIN);
		gpio_mode_set(USART0_S1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,USART0_S1_PIN);
    gpio_output_options_set(USART0_S1_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,USART0_S1_PIN);
	
//		gpio_mode_set(USART1_S0_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,USART1_S0_PIN);
//    gpio_output_options_set(USART1_S0_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,USART1_S0_PIN);
//		gpio_mode_set(USART1_S1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,USART1_S1_PIN);
//    gpio_output_options_set(USART1_S1_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,USART1_S1_PIN);

    GPIO_BC(USART0_S0_PORT) =USART0_S0_PIN;
		GPIO_BC(USART0_S1_PORT) =USART0_S1_PIN;
	
//		GPIO_BC(USART1_S0_PORT) =USART1_S0_PIN;
//		GPIO_BC(USART1_S1_PORT) =USART1_S1_PIN;
}

/*!
    \brief      configure USART CHANNEL Select
		\param[in]  usart_periph : usart to use  [USART0/USART1]
		\param[in]  ch :channel to select [0~3]
    \retval     none
*/
void usart_ch_sw_ctrl(uint32_t usart_periph,uint8_t ch)
{
	switch(usart_periph){
		case USART0:
			switch(ch){
				case 0:
					GPIO_BC(USART0_S0_PORT) = USART0_S0_PIN;
					GPIO_BC(USART0_S1_PORT) = USART0_S1_PIN;
					break;
				 case 1:
					GPIO_BOP(USART0_S0_PORT) = USART0_S0_PIN;
					GPIO_BC(USART0_S1_PORT) = USART0_S1_PIN;
					break;
				 case 2:
					GPIO_BC(USART0_S0_PORT) = USART0_S0_PIN;
					GPIO_BOP(USART0_S1_PORT) = USART0_S1_PIN;
					break;
				 case 3:
					GPIO_BOP(USART0_S0_PORT) = USART0_S0_PIN;
					GPIO_BOP(USART0_S1_PORT) = USART0_S1_PIN;
					break;
				 default:
					 break;
			}
		break;
		case USART1:
			switch(ch){
				case 0:
					GPIO_BC(USART1_S0_PORT) = USART1_S0_PIN;
					GPIO_BC(USART1_S1_PORT) = USART1_S1_PIN;
					break;
				 case 1:
					GPIO_BC(USART1_S0_PORT) = USART1_S0_PIN;
					GPIO_BOP(USART1_S1_PORT) = USART1_S1_PIN;
					break;
				 case 2:
					GPIO_BOP(USART1_S0_PORT) = USART1_S0_PIN;
					GPIO_BC(USART1_S1_PORT) = USART1_S1_PIN;
					break;
				 case 3:
					GPIO_BOP(USART1_S0_PORT) = USART1_S0_PIN;
					GPIO_BOP(USART1_S1_PORT) = USART1_S1_PIN;
					break;
				 default:
					 break;
			}
		break;
		default:
		break;
	}
}

/*!
    \brief      configure Pump Control  GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void pump_io_init(void)
{
    /* enable the Pump Control GPIO clock */
    rcu_periph_clock_enable(Pump_CLK);

    /* configure Pump Control GPIO port */ 
    gpio_mode_set(Pump_Port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,Pump_Pin);
    gpio_output_options_set(Pump_Port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,Pump_Pin);

    GPIO_BC(Pump_Port) =Pump_Pin;
}

/*!
    \brief      configure Pump States Change  Function
    \param[in]  state 0--off / 1--on
    \param[out] none
    \retval     none
*/
void pump_states_change(_Bool state)
{
	if(state==1)
    GPIO_TG(Pump_Port) = Pump_Pin;
	else
		GPIO_BC(Pump_Port) = Pump_Pin;
}

/**
  * @}
  */ 
/** @addtogroup STM32WLXX_NUCLEO_LOW_LEVEL_LED_Functions
  * @{
  */ 

/**
  * @brief  Configures I2C GPIO.
  * @param  i2c: I2C to be configured. 
  *         This parameter can be one of the following values:
  *            @arg I2C1
  *            @arg I2C2
  *            @arg I2C3
  * @retval None
  */
void BSP_IIC_Init(IIC_TypeDef i2c)
{
	rcu_periph_clock_enable(IIC1_SCL_GPIO_CLK);
  gpio_mode_set(IIC1_SCL_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, IIC1_SCL_PIN);
  gpio_output_options_set(IIC1_SCL_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,IIC1_SCL_PIN);
	
  rcu_periph_clock_enable(IIC1_SDA_GPIO_CLK);
  gpio_mode_set(IIC1_SDA_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, IIC1_SDA_PIN);
  gpio_output_options_set(IIC1_SDA_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,IIC1_SDA_PIN);
	
	gpio_bit_set(IIC1_SCL_GPIO_PORT,IIC1_SCL_PIN);
	gpio_bit_set(IIC1_SDA_GPIO_PORT,IIC1_SDA_PIN);
}

/**
  * @brief  Configures I2C GPIO.
  * @param  i2c: I2C to be configured. 
  *         This parameter can be one of the following values:
  *            @arg I2C1
  *            @arg I2C2
  *            @arg I2C3
  * @retval None
  */
void BSP_IIC_SDA_IN(IIC_TypeDef i2c)
{
  gpio_mode_set(IIC1_SDA_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, IIC1_SDA_PIN);

}

/**
  * @brief  Configures I2C GPIO.
  * @param  i2c: I2C to be configured. 
  *         This parameter can be one of the following values:
  *            @arg I2C1
  *            @arg I2C2
  *            @arg I2C3
  * @retval None
  */
void BSP_IIC_SDA_OUT(IIC_TypeDef i2c)
{
  gpio_mode_set(IIC1_SDA_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, IIC1_SDA_PIN);
  gpio_output_options_set(IIC1_SDA_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,IIC1_SDA_PIN);
}

/**
  * @brief  DeInit I2Cs.
  * @param  i2c: I2C to be de-init. 
  *         This parameter can be one of the following values:
  *            @arg i2c1
  *            @arg i2c2
  *            @arg i2c3
  * @note I2C DeInit does not disable the GPIO clock nor disable the Mfx 
  * @retval None
  */
void BSP_IIC_DeInit(IIC_TypeDef i2c)
{
	

}


/**
  * @}
  */ 
/** @addtogroup STM32WLXX_NUCLEO_LOW_LEVEL_Multi-Plexer_Functions
  * @{
  */ 

/**
  * @brief  Configures Multi-Plexer Control Ports.
  * @param  void
  * @retval MultiPlexer SW status
  */
void BSP_MultiPlexer_Init(void)
{

	rcu_periph_clock_enable(MP_S0_GPIO_CLK);
  gpio_mode_set(MP_S0_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, MP_S0_PIN);
  gpio_output_options_set(MP_S0_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,MP_S0_PIN);
	
	rcu_periph_clock_enable(MP_S1_GPIO_CLK);
	gpio_mode_set(MP_S1_GPIO_PORT,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,MP_S1_PIN);
	gpio_output_options_set(MP_S1_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,MP_S1_PIN);
	
	BSP_MP_Chnnel_Set(MP_CH1);
}

/**
  * @brief  DeInit MultiPlexer SW.
  * @param  void
  * @note MultiPlexer SW DeInit does not disable the GPIO clock nor disable the Mfx 
  * @retval BSP status
  */
void BSP_MultiPlexer_DeInit(void)
{
  /* Turn off LED */
//  HAL_GPIO_WritePin(MP_S0_GPIO_PORT, MP_S0_PIN|MP_S1_PIN, GPIO_PIN_RESET);

//  /* DeInit the GPIO_LED pin */
//  HAL_GPIO_DeInit(MP_S0_GPIO_PORT, MP_S0_PIN|MP_S1_PIN);

//  return BSP_ERROR_NONE;
}

/**
  * @brief  Turns selected SW On.
  * @param  sw: Specifies the sw to be set On. 
  *         This parameter can be one of the following values:
  *            @arg SW0
  *            @arg SW1
  * @retval BSP status
  */
void BSP_MP_SW_On(uint32_t sw)
{
	gpio_bit_set(MultiPlex_SW_PORT[sw],MultiPlex_SW_PIN[sw]);
}

/**
  * @brief  Turns selected SW Off.
  * @param  sw: Specifies the sw to be set off. 
  *         This parameter can be one of the following values:
  *            @arg SW0
  *            @arg SW1
  * @retval BSP status
  */
void BSP_MP_SW_Off(uint32_t sw)
{
	gpio_bit_reset(MultiPlex_SW_PORT[sw],MultiPlex_SW_PIN[sw]);
}

/**
  * @brief  selected Communication Channel.
  * @param  channel: the channel which will be selected
  *            @arg MP_CH0
  *            @arg MP_CH1
  *            @arg MP_CH2
  *            @arg MP_CH3
  * @retval BSP status
  */
void BSP_MP_Chnnel_Set(MultiPlex_CH_TypeDef ch)
{
  switch(ch){
		case MP_CH0:
		{
			BSP_MP_SW_Off(MP_SW0);
			BSP_MP_SW_Off(MP_SW1);
		}
		break;
		case MP_CH1:
		{
			BSP_MP_SW_On(MP_SW0);
			BSP_MP_SW_Off(MP_SW1);
		}
		break;
		case MP_CH2:
		{
			BSP_MP_SW_Off(MP_SW0);
			BSP_MP_SW_On(MP_SW1);
		}
		break;
		case MP_CH3:
		{
			BSP_MP_SW_On(MP_SW0);
			BSP_MP_SW_On(MP_SW1);
		}
		break;
		default:
		break;
	}
}





///*
// * Call by Systick interrupt server
// */
//void systick_handler(void)
//{
//	systick_us += 500 * 1000;
//	systick_ms += 500;
//}
// 
// 
//void SysTick_Handler(void)
//{
//	systick_handler();
//}

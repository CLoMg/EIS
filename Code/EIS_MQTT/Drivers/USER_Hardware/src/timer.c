#include "timer.h"
#include "gd32f10x_timer.h"
#include "freertos.h"
#include "task.h"

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
	rcu_periph_clock_enable(RCU_TIMER0);
	
	timer_deinit(TIMER0);
	timer_initpara.prescaler = prescaler;
	timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection = TIMER_COUNTER_UP;
	timer_initpara.period = period;
	timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_init(TIMER0,&timer_initpara);
	
	timer_auto_reload_shadow_enable(TIMER0);
	timer_flag_clear(TIMER0,TIMER_FLAG_UP);
}

/*!
    \brief     delay_nus
    \param[in]  n_us -- counts of us while should be waited 
    \param[out] none
    \retval     none
*/
void delay_us(uint32_t n_us)
{
	#if TIMER_DELAY
		timer_enable(TIMER0);
		while(n_us--)
		{
			while(timer_flag_get(TIMER0,TIMER_FLAG_UP) == RESET)
			{
				;
			}
			timer_flag_clear(TIMER0,TIMER_FLAG_UP);
		}
		timer_disable(TIMER0);
	#else
		while(n_us!=0){
		n_us--;
		__NOP();
	}
	
	#endif
}
/*!
    \brief     delay_ms
    \param[in]  n_ms -- counts of ms while should be waited 
    \param[out] none
    \retval     none
*/
void delay_ms(uint16_t n_ms)
{
	#if TIMER_DELAY
		while(n_ms--)
			delay_us(500);
	#else
		vTaskDelay(n_ms/portTICK_RATE_MS);
	#endif
}

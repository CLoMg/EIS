#include "stm32f1xx_hal.h"
#include "direct.h"
#include "stdint.h"
#include "stm32f1xx_hal_flash.h"
#include "gpio.h"
#include "sysparams.h"

#define POSITION_ADDR 0x0800FC04
Position_T Dev_Position;

void Position_Parameter_Access(void){
	uint32_t Posi_Parameter_IN_Flash;
	Posi_Parameter_IN_Flash = *(__IO uint32_t *)(POSITION_ADDR);
	
  if(Posi_Parameter_IN_Flash!=0xffffffff){
		Dev_Position.KM = (uint16_t)(Posi_Parameter_IN_Flash>>16);
		Dev_Position.M = (uint16_t)(Posi_Parameter_IN_Flash);
	}
	else
	{
		Dev_Position.KM = 0;
		Dev_Position.M = 0;
	}
}


void Direct_Init(void){
	Direct_GPIO_Init();
  Direct_Light_Control(bi_dir);
	Position_Parameter_Access();
}

/* user need to fill this function according to their own board*/
void Direct_GPIO_Init(void){
	Dir_Light_GPIO_Init();
}

void Direct_Light_Control(Directions_T dir){
	Directions_T direction;
	direction  = (Directions_T)dir;
	Dir_Light_GPIO_Control(direction);
}

Directions_T Direct_Light_Get(void){
	Directions_T dir = dir_err;
	dir =(Directions_T) Dir_Light_GPIO_Status_Get();
	return dir;
}


/*异常事件发生时灯光控制函数*/
unsigned char AbneEvt_Dispose(unsigned short *abnloc)
{
	/*判断输入参数合法性*/
	if((abnloc[0]>=0)&&(abnloc[1]>=0)&&(abnloc[1]<1000))
	{
		/*比较发生异常事件位置桩号和设备桩号*/
		/*首先进行 KM 级大小比较,如果KM 信息相同，再进行 M 级大小比较*/
		if(abnloc[0] == DEV_LOC[0])
		{
			/*进行 M 级大小比较*/
			if(abnloc[1] == DEV_LOC[1])
			{
				/*如果异常事件位置和设备位置完全一致，则疏散标两侧均点亮*/
				Direct_Light_Control(bi_dir);
				return Success;
			}
				/*如果异常事件位置信息小于 设备位置信息，则右侧点亮，左侧熄灭（仅针对右侧桩号大于左侧情形）*/	
				else if(abnloc[1] < DEV_LOC[1])
			{
				Direct_Light_Control(right);
				return Success;
			}
			else
		/*如果异常事件位置信息 大于于 设备位置信息，则左侧点亮，右侧熄灭（仅针对右侧桩号大于左侧情形）*/	
			{
				Direct_Light_Control(left);
				return Success;
			}
			
		}
		/*如果异常事件位置信息小于 设备位置信息，则右侧点亮，左侧熄灭（仅针对右侧桩号大于左侧情形）*/	
		else if(abnloc[0] < DEV_LOC[0])
		{
			Direct_Light_Control(right);
			return Success;
		}
		else
		/*如果异常事件位置信息 大于于 设备位置信息，则左侧点亮，右侧熄灭（仅针对右侧桩号大于左侧情形）*/	
		{
			Direct_Light_Control(left);
			return Success;
		}
	
	}
	else
	/*返回参数不合法*/
		return Invalid_Para;
}





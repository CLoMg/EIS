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


/*�쳣�¼�����ʱ�ƹ���ƺ���*/
unsigned char AbneEvt_Dispose(unsigned short *abnloc)
{
	/*�ж���������Ϸ���*/
	if((abnloc[0]>=0)&&(abnloc[1]>=0)&&(abnloc[1]<1000))
	{
		/*�ȽϷ����쳣�¼�λ��׮�ź��豸׮��*/
		/*���Ƚ��� KM ����С�Ƚ�,���KM ��Ϣ��ͬ���ٽ��� M ����С�Ƚ�*/
		if(abnloc[0] == DEV_LOC[0])
		{
			/*���� M ����С�Ƚ�*/
			if(abnloc[1] == DEV_LOC[1])
			{
				/*����쳣�¼�λ�ú��豸λ����ȫһ�£�����ɢ�����������*/
				Direct_Light_Control(bi_dir);
				return Success;
			}
				/*����쳣�¼�λ����ϢС�� �豸λ����Ϣ�����Ҳ���������Ϩ�𣨽�����Ҳ�׮�Ŵ���������Σ�*/	
				else if(abnloc[1] < DEV_LOC[1])
			{
				Direct_Light_Control(right);
				return Success;
			}
			else
		/*����쳣�¼�λ����Ϣ ������ �豸λ����Ϣ�������������Ҳ�Ϩ�𣨽�����Ҳ�׮�Ŵ���������Σ�*/	
			{
				Direct_Light_Control(left);
				return Success;
			}
			
		}
		/*����쳣�¼�λ����ϢС�� �豸λ����Ϣ�����Ҳ���������Ϩ�𣨽�����Ҳ�׮�Ŵ���������Σ�*/	
		else if(abnloc[0] < DEV_LOC[0])
		{
			Direct_Light_Control(right);
			return Success;
		}
		else
		/*����쳣�¼�λ����Ϣ ������ �豸λ����Ϣ�������������Ҳ�Ϩ�𣨽�����Ҳ�׮�Ŵ���������Σ�*/	
		{
			Direct_Light_Control(left);
			return Success;
		}
	
	}
	else
	/*���ز������Ϸ�*/
		return Invalid_Para;
}





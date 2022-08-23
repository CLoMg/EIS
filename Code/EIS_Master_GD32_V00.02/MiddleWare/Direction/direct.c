#include "direct.h"
#include "msg_handler.h"
#include "gd32f10x_fmc.h"
#include "gd32f10x_eval.h"
#include "FreeRtos.h"
#include "queue.h"

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
	BSP_Dir_Light_Init();
}

void Direct_Light_Control(Directions_T dir){
	BSP_Direction_TypeDef direction;
	direction  = (BSP_Direction_TypeDef)dir;
	BSP_Light_Control(direction);
}

Directions_T Direct_Light_Get(void){
	Directions_T dir = dir_err;
	dir =(Directions_T) BSP_Light_Get();
	return dir;
}

void Position_Parameter_Set(uint16_t new_km, uint16_t new_m){
	if((new_km != Dev_Position.KM)||(new_m != Dev_Position.M)){	
			uint32_t new_posi = (uint32_t)new_km<<16 | new_m;
			fmc_word_write(POSITION_ADDR, 1,&new_posi);
			Dev_Position.KM = new_km;
			Dev_Position.M = new_m;
	}
}



void Direct_Process(Directions_T dir){
	Direct_Light_Control(dir);
}
xData reply;
void *Dir_Func(int *arg){
	xData msg = *((xData*)arg);

	reply.sn = msg.sn;
	reply.iEventType = LORATxEvent;
	reply.iMeaning = msg.iMeaning;
	reply.iValue = NULL;
	switch(msg.iMeaning){
			case Light_State_Refresh:
			{
				Directions_T dir = Direct_Light_Get();
				xSemaphoreTake(xStateMutex,portMAX_DELAY);
				Sensor_Buff[0] &= 0x3F;
				Sensor_Buff[0] |= (uint8_t)(dir<<6);
				xSemaphoreGive(xStateMutex);	
			}
			 break;
			case  Light_Dir_Issue:{
				reply.iValue = (unsigned char*)malloc(sizeof(unsigned char) *1 );
				reply.len = 0x01;
				Directions_T dir = (Directions_T)(*msg.iValue);
				if(dir >= dir_err)
					*(reply.iValue)= 0x02;
				else{
					Direct_Light_Control(dir);
					if(Direct_Light_Get() == dir)
						*(reply.iValue)=0x01;
					else
						*(reply.iValue)=0x00;
				}
			}
			break;
				
			case Light_Dir_Query:{
				reply.iValue = (unsigned char*)malloc(sizeof(unsigned char) *1 );
				reply.len = 0x01;
				*(reply.iValue)= Direct_Light_Get();
			}
			break;
			case DevPosi_Issue:
			{
				
				reply.iValue = (unsigned char*)malloc(sizeof(unsigned char) *1 );
				reply.len = 0x01;
				uint16_t pos_new_m = (uint16_t)(msg.iValue[2]<<8)|(msg.iValue[3]);
				if(pos_new_m >= 1000)
					*(reply.iValue)= 0x02;
				else{
					uint16_t pos_new_km = (uint16_t)(msg.iValue[0]<<8)|msg.iValue[1];
					Position_Parameter_Set(pos_new_km, pos_new_m);
					Position_Parameter_Access();
					if((pos_new_km == Dev_Position.KM)&&(pos_new_m == Dev_Position.M))
						*(reply.iValue)= 0x01;
					else
						*(reply.iValue)= 0x00;
				}
			}
			break;
			case DevPosi_Query:
			{
				reply.iValue = (unsigned char*)malloc(sizeof(unsigned char) * 4);
				reply.len = 0x04;
				reply.iValue[0] = (uint8_t)(Dev_Position.KM>>8);
				reply.iValue[1] = (uint8_t)Dev_Position.KM;
				reply.iValue[2] = (uint8_t)(Dev_Position.M>>8);
				reply.iValue[3] = (uint8_t)Dev_Position.M;
			}
			break;
			case Abnormal_Posi_Issue:
			{
				reply.iValue = (unsigned char*)malloc(sizeof(unsigned char) * 1);
				reply.len = 0x01;
				uint16_t pos_abnormal_km = (uint16_t)(msg.iValue[0]<<8)|msg.iValue[1];
				uint16_t pos_abnormal_m = (uint16_t)(msg.iValue[2]<<8)|msg.iValue[3];
				if(pos_abnormal_m >=1000)
					*(reply.iValue)= 0x03;
				else{
					Dev_Status_Struct.status = (Dev_Status_Struct.status & 0xCF) | (0x02<<4);
					if(((uint32_t)pos_abnormal_km<<16 | pos_abnormal_m) > ((uint32_t)Dev_Position.KM<<16 | Dev_Position.M)){
						Direct_Light_Control(left);
						*(reply.iValue)= 0x00;
					}
					else{
						Direct_Light_Control(right);
						*(reply.iValue)= 0x01;
					}
				}
			}
			break;
			case Abnormal_Event_CLR:
			{
				reply.iValue = (unsigned char*)malloc(sizeof(unsigned char) * 1);
				reply.len = 0x01;
				Dev_Status_Struct.status = (Dev_Status_Struct.status & 0xCF) | (0x01<<4);
				Direct_Light_Control(bi_dir);
				if((Direct_Light_Get()==bi_dir)&&((Dev_Status_Struct.status & 0x30) == 0x01))
					*(reply.iValue)= 0x01;
				else
					*(reply.iValue)= 0x00;
			}
			break;

			default:
			break;
		}
		return &reply;
}



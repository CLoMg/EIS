#include "direct.h"
#include "msg_handler.h"
#include "gd32f10x_fmc.h"
#include "gd32f10x_eval.h"

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
	Dev_Status_Struct.status = (Dev_Status_Struct.status & 0xCF) | (0x01<<4);
	Internal_MSG_T *p = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T));
	p->next = NULL;
	States_Msg_Head = p;
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
static unsigned short cnt=0;
void Dir_Func(void){
	if((cnt++)==0xFF)
		cnt = 0;
	if(cnt % 10){
		unsigned char dir = Direct_Light_Get();
		Dev_Status_Struct.status = (Dev_Status_Struct.status & 0x3F) | (dir << 6); 
	}
	if(States_Msg_Head->next != NULL){
		Internal_MSG_T *p = States_Msg_Head->next;
		States_Msg_Head->next = p->next;
		switch(p->func_code){
			case  Light_Dir_Issue:{
				Internal_MSG_T *reply = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T));
				reply->data_addr = (uint8_t *)malloc(sizeof(uint8_t));
				Directions_T dir = (Directions_T)p->data_addr[0];
				if(dir >= dir_err)
					*(reply->data_addr) = 0x02;
				else{
					Direct_Light_Control(dir);
					//delay()?
					if(Direct_Light_Get() == dir)
						*(reply->data_addr) =0x01;
					else
						*(reply->data_addr) =0x00;
				}
				reply->func_code  = p->func_code;
				reply->id = Lora;
				reply->sn = p->sn;
				reply->len = 0x01;
				reply->next = NULL;
				Internal_MSG_T * q =  Int_Msg_Head;
				while(q->next != NULL)
					q = q->next;
				q->next = reply;
				
				free(p->data_addr);
				p->data_addr = NULL;
				free(p);
				p = NULL;
			}
			break;
				
			case Light_Dir_Query:{
				Internal_MSG_T *reply = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T));
				reply->data_addr = (uint8_t *)malloc(sizeof(uint8_t));
				Directions_T dir;
				dir = Direct_Light_Get();
				*(reply->data_addr) = dir;
				reply->func_code  = p->func_code;
				reply->id = Lora;
				reply->sn = p->sn;
				reply->len = 0x01;
				reply->next = NULL;
				Internal_MSG_T * q =  Int_Msg_Head;
				while(q->next != NULL)
					q = q->next;
				q->next = reply;	
				
				free(p->data_addr);
				p->data_addr = NULL;
				free(p);
				p = NULL;
			}
			break;
			case DevPosi_Issue:
			{
				Internal_MSG_T *reply = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T));
				reply->data_addr = (uint8_t *)malloc(sizeof(uint8_t));
				uint16_t pos_new_m = (uint16_t)p->data_addr[2]<<8 | p->data_addr[3];
				if(pos_new_m >= 1000)
					*(reply->data_addr) = 0x02;
				else{
					uint16_t pos_new_km = (uint16_t)p->data_addr[0]<<8 | p->data_addr[1];
					Position_Parameter_Set(pos_new_km, pos_new_m);
					Position_Parameter_Access();
					if((pos_new_km == Dev_Position.KM)&&(pos_new_m == Dev_Position.M))
						*(reply->data_addr) =0x01;
					else
						*(reply->data_addr) =0x00;
				}
				reply->func_code  = p->func_code;
				reply->id = Lora;
				reply->sn = p->sn;
				reply->len = 0x01;
				reply->next = NULL;
				Internal_MSG_T * q =  Int_Msg_Head;
				while(q->next != NULL)
					q = q->next;
				q->next = reply;
				
				free(p->data_addr);
				p->data_addr = NULL;
				free(p);
				p = NULL;
			}
			break;
			case DevPosi_Query:
			{
				Internal_MSG_T *reply = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T));
				reply->data_addr = (uint8_t *)malloc(sizeof(uint16_t)*2);
				
				reply->data_addr[0] = (uint8_t)(Dev_Position.KM>>8);
				reply->data_addr[1] = (uint8_t)Dev_Position.KM;
				reply->data_addr[2] = (uint8_t)(Dev_Position.M>>8);
				reply->data_addr[3] = (uint8_t)Dev_Position.M;
				reply->func_code  = p->func_code;
				reply->id = Lora;
				reply->sn = p->sn;
				reply->len = 0x04;
				reply->next = NULL;
				Internal_MSG_T * q =  Int_Msg_Head;
				while(q->next != NULL)
					q = q->next;
				q->next = reply;
				
				free(p->data_addr);
				p->data_addr = NULL;
				free(p);
				p = NULL;
			}
			break;
			case Abnormal_Posi_Issue:
			{
				Internal_MSG_T *reply = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T));
				reply->data_addr = (uint8_t *)malloc(sizeof(uint8_t));
				
				uint16_t pos_abnormal_km = (uint16_t)p->data_addr[0]<<8 | p->data_addr[1];
				uint16_t pos_abnormal_m = (uint16_t)p->data_addr[2]<<8 | p->data_addr[3];
				if(pos_abnormal_m >=1000)
					reply->data_addr[0] = 0x03;
				else{
					Dev_Status_Struct.status = (Dev_Status_Struct.status & 0xCF) | (0x02<<4);
					if(((uint32_t)pos_abnormal_km<<16 | pos_abnormal_m) > ((uint32_t)Dev_Position.KM<<16 | Dev_Position.M)){
						Direct_Light_Control(left);
						reply->data_addr[0] = 0x00;
					}
					else{
						Direct_Light_Control(right);
						reply->data_addr[0] = 0x01;
					}
				}
				reply->func_code  = p->func_code;
				reply->id = Lora;
				reply->sn = p->sn;
				reply->len = 0x01;
				reply->next = NULL;
				Internal_MSG_T * q =  Int_Msg_Head;
				while(q->next != NULL)
					q = q->next;
				q->next = reply;
				
				free(p->data_addr);
				p->data_addr = NULL;
				free(p);
				p = NULL;
			}
			break;
			case Abnormal_Event_CLR:
			{
				Internal_MSG_T *reply = (Internal_MSG_T *)malloc(sizeof(Internal_MSG_T));
				reply->data_addr = (uint8_t *)malloc(sizeof(uint8_t));
				

				
				Dev_Status_Struct.status = (Dev_Status_Struct.status & 0xCF) | (0x01<<4);
				Direct_Light_Control(bi_dir);
				
				if((Direct_Light_Get()==bi_dir)&&((Dev_Status_Struct.status & 0x30) == 0x01))
					reply->data_addr[0] = 0x01;
				else
					reply->data_addr[0] = 0x00;

				reply->func_code  = p->func_code;
				reply->id = Lora;
				reply->sn = p->sn;
				reply->len = 0x01;
				reply->next = NULL;
				Internal_MSG_T * q =  Int_Msg_Head;
				while(q->next != NULL)
					q = q->next;
				q->next = reply;
				
				free(p->data_addr);
				p->data_addr = NULL;
				free(p);
				p = NULL;
			}
			break;
			default:
				free(p->data_addr);
				p->data_addr = NULL;
				free(p);
				p = NULL;
			break;
			}
		}
}



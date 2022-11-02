#include "command.h"
#include <stdio.h>
#include <string.h>
#include "mh4xx.h"
#include "timer.h"
#include "sht3x.h"
#include "stmflash.h"
#include "pms7003.h"
#include "max30105.h"
#include <stdlib.h>
#include "usart.h"
#include "wdg.h"
#include "ec20.h"
#include "uestc_gas.h"
#include "mmdxx.h"
#include "LORA.h"
#include "adc.h"


static uint16_t Serial_num=0;

#define LC_TEST_TYPE 0
u32 flash_parameter_bak[43];
Data_Packet_t  *Data_pack=NULL;
Data_Packet_t  *Terminal_Packet=NULL;
//byte0~1 高速编号 byte2 隧道编号  byte3 项目类型（对应通信协议）  byte4设备类型(A/B) byte5~6 设备编号 byte7 保留
uint8_t Device_id[8]= {0x50, 0x12, 0x00, 0x0E, 0x00, 0x00, 0x06, 0x00};

#if device_type_num
	uint8_t sensor_status[2]={0x0F,0xC0};//0000 1111 1100 0000 
#else
	uint8_t sensor_status[2]={0xF0,0xC0};//1111 0000 1100 0000 
#endif
uint8_t packet[45];

_Bool Query_flag=0;

void Command_Parse(uint8_t *data){
		Data_Unpack(data,Data_pack);
}
void Data_Unpack(uint8_t *data,Data_Packet_t *Data_package){
	uint16_t command_code;
	if(data[0]==0x4A && data[1]==0x54){
		//如果是网关，判断ID是否是本设备的ID，是则继续解析，不是则广播给其他设备
		//如果是终端，判断ID是否是本设备的ID，是则继续解析，不是则不回复消息
		if(memcmp(&data[2],Device_id,8)==0){
				memcpy(Data_package->serial_num,&data[10],2);
				uint16_t length=(uint16_t)data[17]<<8|data[18];
				uint16_t check_sum = Check_Sum_Cal(data,length+19);
				if(check_sum==((uint16_t)data[length+19]<<8|data[length+20])){
					if(data[13]==func_code_type && data[14] == platform_issue){
						memcpy(Data_package->data_length,&data[17],2);
						for(int i=0;i<length;++i){
							Data_package->data_buff[i]=data[i+19];
						}
						command_code=((uint16_t)data[15]<<8)|data[16];
						Return_Data_State(Data_package,device_return,command_code);
					}
					else{
							Return_Instru_Error(Data_package, invalid_func_code);
					}	
			}
			else{
						Return_Instru_Error(Data_package, checksum_error);
			}
		}
		#if gateway
	 //设备是网关，广播给其他设备。不是网关，不做处理
		else{
			  
				LORA_send_data((char *)data,((uint16_t)data[17]<<8|data[18])+21);
		}
		#endif
	}
	command_recieved_flag_4G=0;
	command_recieved_flag_ETH=0;
	//command_recieved_flag_LORA=0;
}

void Data_Send(Data_Packet_t *Data_package){
	uint16_t length = (uint16_t)Data_package->data_length[0]<<8 | Data_package->data_length[1];

	Data_Package(length,Data_package);
	#if gateway
	for(int cnt=0;cnt<length+21;++cnt){
			USART_SendData(USART2,packet[cnt]);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束
			USART_SendData(USART3,packet[cnt]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
	}
	#else
		LORA_send_data((char *)packet,length+21);
	#endif

}

void Data_Package(uint8_t length,Data_Packet_t *Data_package){
	
	uint8_t count;
	
	Data_package->data_length[0]=(uint8_t)(length>>8);	
  Data_package->data_length[1]=(uint8_t)length;
	memcpy(packet,Data_package,19);
	for(count=0;count<length;++count){
		packet[count+19]=Data_package->data_buff[count];
	}
	count+=19;
	uint16_t check_sum =Check_Sum_Cal(packet,count);
	packet[count++]=(uint8_t)(check_sum>>8);
	packet[count]=(uint8_t)check_sum;
	//return packet;
	
}

void Data_Package_init(){
	uint32_t Device_id_u32_init[3];
	uint8_t i=0;
	uint32_t Terminal_mac;
	char device_id_str[]="MAC_ADDR:00 00 00 00 00 00 00 00\r\n";
	Data_Packet_t *point = malloc(sizeof(Data_Packet_t));
	Data_Packet_t *terminal_point = malloc(sizeof(Data_Packet_t));
	
	Terminal_Packet=terminal_point;
	
	point->data_head[0] = (uint8_t)(Data_head>>8);
	point->data_head[1] = (uint8_t)Data_head;
	#if New_ID_Access
		Get_Device_ID(Device_id_u32_init);
		memcpy(point->device_id,(u8*)Device_id_u32_init,8); 
		memcpy(Device_id,(u8*)Device_id_u32_init,8); 
		memcpy(terminal_id,(u8*)Device_id_u32_init,8); 
	for(i=0;i<8;++i)
		sprintf(&device_id_str[9+i*3],"%02X ",point->device_id[i]);
  i=0;
	while(device_id_str[i++]!='\0'){
			USART_SendData(USART3,device_id_str[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
		}	
	 LORA_send_data(device_id_str,sizeof(device_id_str));

	 
		for(int i=0;i<8;++i){
			USART_SendData(USART3,point->device_id[i]);
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
		}
	#else
		uint32_t Device_id_u32_read[2];
		memcpy(point->device_id,Device_id,8); 
		memcpy(Device_id_u32_init,Device_id,8);
		STMFLASH_Read(device_id_addr,Device_id_u32_read,2);
		if((Device_id_u32_read[0]!=Device_id_u32_init[0])||(Device_id_u32_read[1]!=Device_id_u32_init[1]))
			STMFLASH_Write(device_id_addr,Device_id_u32_init,2);
	#endif

	#if gateway
 	  STMFLASH_Read(terminal_mac_addr,&Terminal_mac,1);
		if(Terminal_mac!=0xFFFFFFFF)
			STMFLASH_Read(terminal_mac_addr,(uint32_t*)terminal_id,18);
	#endif
	point->serial_num[0] = (uint8_t)(Serial_num>>8);
	point->serial_num[1] = (uint8_t)Serial_num;
	point->encry_mode = Encry_mode;
	point->func_code[0] =func_code_type;
//	point->data_buff[0]=0;// 0000 0000  备用
//	point->data_buff[1]=0xA6;// 1010 0110
	Data_pack=point;
}

uint16_t Check_Sum_Cal(uint8_t *data,uint8_t length){
	uint16_t sum=0;
	for(uint8_t count=0;count<length;++count)
		sum+=data[count];
	return sum;
}

void Return_Instru_Error(Data_Packet_t *p, enum instru_exe_state error_type){
	p->func_code[1]= device_return;
	p->func_code[2]=(uint8_t)(error_type>>8);
	p->func_code[3]=(uint8_t)error_type;
	p->data_length[0]=0;
	p->data_length[1]=0;
	Data_Send(p);
}

void Device_return(uint8_t instruc_func_flag,uint16_t instruction){

	Return_Data_State(Data_pack,instruc_func_flag,instruction);
}
void Return_Data_State(Data_Packet_t *p,uint8_t instruc_func_flag,uint16_t instruction){
	p->func_code[1]= instruc_func_flag;
	p->func_code[2]=(uint8_t)(instruction>>8);
	p->func_code[3]= (uint8_t)instruction;
	switch(instruction){
		//设备重启入口
		case device_init:
		{
			NVIC_SystemReset();
			
		}
		break;
		case set_clock:
			break;
		//读取版本号入口
		case read_version:{
			Read_Software_Version(p);
			
		}
		break;
		//网关升级入口
		case gateway_upgrade_cmd:{
			#if 1
     
			//将0xaa和升级所用串口写入参数存储区
			u32 flash_to_write=0xaa;
			
			STMFLASH_Read(User_Parameter_BASE,flash_parameter_bak,27);  
			flash_parameter_bak[0] = flash_to_write;
			u32 usart_num=0;
		
			if(command_recieved_flag_4G==1){
				usart_num=2;
			}
			else if(command_recieved_flag_ETH==1){
				usart_num=3;
			}
			else if(command_recieved_flag_LORA==1){
				usart_num=1;
			}
		
			flash_parameter_bak[1] = usart_num;
			STMFLASH_Write(User_Parameter_BASE,flash_parameter_bak,27);
			
			//将0x55和网关/终端升级flag写入参数存储区
			STMFLASH_Read(Upgrade_Parameter_BASE,flash_parameter_bak,43);
			flash_parameter_bak[26] = ((uint16_t)p->serial_num[0]<<8)|p->serial_num[1];
	    u32 upgrade_para[4]={0x55,};
			
			upgrade_para[2] = p->data_buff[1];
			upgrade_para[3]=((uint32_t)(p->data_buff[2]<<24))|((uint32_t)(p->data_buff[3]<<16))|((uint16_t)(p->data_buff[4]<<8))|(p->data_buff[5]);
			if(p->data_buff[0]==0x00){
				upgrade_para[1] = flash_parameter_bak[1];
				if(upgrade_para[1]==0xFFFFFFFF)
					upgrade_para[1]=0;
				//如果带升级程序版本号大于当前版本，将当前接收到升级包数清零
				if(upgrade_para[2]>upgrade_para[1])
					flash_parameter_bak[6]=0x00;
				
				memcpy((gateway_upgrade_flag_addr-Upgrade_Parameter_BASE)/4+flash_parameter_bak,upgrade_para,sizeof(upgrade_para));
				STMFLASH_Write(Upgrade_Parameter_BASE,flash_parameter_bak,43);
			}
			
			if(p->data_buff[0]==0x01){
				upgrade_para[1] = flash_parameter_bak[9];
				if(upgrade_para[1]==0xFFFFFFFF)
					upgrade_para[1]=0;
				//如果带升级程序版本号大于当前版本，将当前接收到升级包数清零
				if(upgrade_para[2]>upgrade_para[1])
					flash_parameter_bak[14]=0x00;
				
				memcpy((node_upgrade_flag_addr-Upgrade_Parameter_BASE)/4+flash_parameter_bak,upgrade_para,sizeof(upgrade_para));
				STMFLASH_Write(Upgrade_Parameter_BASE,flash_parameter_bak,43);
				
			}
			
			//关闭时钟和中断，清空flag
			TIM_Cmd(TIM3, DISABLE);
			Data_Acq_Flag=0;
			TIM_ITConfig(TIM3, TIM_IT_Update ,DISABLE);
			
			
			USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//关闭相关中断
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//关闭相关中断
			USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);//关闭相关中断
			USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);//关闭相关中断
			USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);//关闭相关中断
			USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);//关闭相关中断

			USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);//关闭相关中断
			USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);//关闭相关中断
			USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);//关闭相关中断
			USART_ITConfig(UART4, USART_IT_IDLE, DISABLE);//关闭相关中断
			USART_ITConfig(UART5, USART_IT_IDLE, DISABLE);//关闭相关中断
			USART_ITConfig(USART6, USART_IT_IDLE, DISABLE);//关闭相关中断
			
			
			for(uint8_t i=0;i<6;++i){
				memset(USART_RX_BUF[i],0,USART_REC_LEN);
				USART_RX_CNT[i]=0;
			}
			#if iwatch_dog_work
				IWDG_Feed();
			#endif
			
			__set_FAULTMASK(1);//关闭所有中断
			
			//跳转到bootloader运行
			iap_load_app(0x08000000);
			
			#endif
			
		}
			break;
		//旧版本升级接口
			case update_program:{
			#if 1

				u32 flash_to_write=0xaa;
			
				STMFLASH_Read(User_Parameter_BASE,flash_parameter_bak,27);  
				flash_parameter_bak[0] = flash_to_write;
				u32 usart_num=0;
		
				if(command_recieved_flag_4G==1){
					usart_num=2;
				}
				else if(command_recieved_flag_ETH==1){
					usart_num=3;
				}
				else if(command_recieved_flag_LORA==1){
					usart_num=1;
				}
		
				flash_parameter_bak[1] = usart_num;
				STMFLASH_Write(User_Parameter_BASE,flash_parameter_bak,27);
				
				TIM_Cmd(TIM3, DISABLE);
				Data_Acq_Flag=0;				
				TIM_ITConfig(TIM3, TIM_IT_Update ,DISABLE);
				
				
				USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//关闭相关中断
				USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//关闭相关中断
				USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);//关闭相关中断
				USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);//关闭相关中断
				USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);//关闭相关中断
				USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);//关闭相关中断

				USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);//关闭相关中断
				USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);//关闭相关中断
				USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);//关闭相关中断
				USART_ITConfig(UART4, USART_IT_IDLE, DISABLE);//关闭相关中断
				USART_ITConfig(UART5, USART_IT_IDLE, DISABLE);//关闭相关中断
				USART_ITConfig(USART6, USART_IT_IDLE, DISABLE);//关闭相关中断
				
				
				for(uint8_t i=0;i<6;++i){
					memset(USART_RX_BUF[i],0,USART_REC_LEN);
					USART_RX_CNT[i]=0;
				}
				
			  #if iwatch_dog_work
					IWDG_Feed();
				#endif
				__set_FAULTMASK(1);//关闭所有中断
				
				
				iap_load_app(0x08000000);
			
			#endif

		}
		break;			
		//数据查询
		case query_data:
		{

			Sensor_Aquire(p);
			
		}
		break;
		
		//主动上传
		case active_upload:
		{
			Serial_num++;
			Data_pack->serial_num[0]=(uint8_t)(Serial_num>>8);
			Data_pack->serial_num[1]=(uint8_t)Serial_num;
		  memcpy(&p->data_buff[0],&sensor_status[0],2);
			p->func_code[1]= device_upload;
			Sensor_Aquire(p);
		}
		break;
		//主动上传频率
		case set_upload_sequence:
		{
			p->data_buff[0]=Update_time_Change(((uint16_t)p->data_buff[0]<<8)|p->data_buff[1]);
			p->data_length[0]=0;
			p->data_length[1]=1;
			
		}
		break;
		//设置终端mac地址
		case set_terminal_mac_addr:
		{

			uint16_t total_num=0,cur_num;
			total_num = (((uint16_t)p->data_length[0]<<8)|p->data_length[1])/9;
			for(cur_num=0;cur_num<total_num;cur_num++){
			  memcpy(terminal_id[p->data_buff[9*cur_num]-1],&p->data_buff[9*cur_num+1],8);
			}
			STMFLASH_Read(User_Parameter_BASE,flash_parameter_bak,27); 
			memcpy((uint8_t*)&flash_parameter_bak[7],terminal_id,72);
			STMFLASH_Write(User_Parameter_BASE,flash_parameter_bak,27);
			
			delay_ms(100);
			
			STMFLASH_Read(terminal_mac_addr,flash_parameter_bak,18);
			if(memcmp(terminal_id,(uint8_t*)flash_parameter_bak,72)==0)
				p->data_buff[0]=0x01;
			else
				p->data_buff[0]=0x00;
				p->data_length[0]=0;
				p->data_length[1]=1;
		}
		break;
		//查询终端MAC地址
		case query_terminal_mac_addr:
		{
      uint16_t data_length=0,query_index;	
			query_index = p->data_buff[0];
			if(query_index<9){
				memcpy(p->data_buff,&terminal_id[query_index],8);
			  data_length+=8;
			}
			else if(query_index==9){
				memcpy(p->data_buff,terminal_id,sizeof(terminal_id));
			   data_length+=sizeof(terminal_id);
			}
	    else
				data_length=0x00;
			
				p->data_length[0]=(uint8_t)(data_length>>8);
				p->data_length[1]=(uint8_t)data_length;
		}
		break;
		
		//设置所在组速率等级
		case set_g_lora_speed:
		{
			uint8_t lora_speed_set=p->data_buff[0];
      #if gateway
				uint8_t node_serial=0;
				uint8_t len=(uint16_t)p->data_length[0]|p->data_length[1];
				for(node_serial=1;node_serial<9;++node_serial){
					uint8_t i=0;
					memcpy(p->device_id,terminal_id[node_serial],8);
					p->func_code[1]=platform_issue;
				  Data_Package(len,p);
					for(i=0;i<5;++i){
						LORA_send_data((char*)packet,len+21);
						delay_ms(100);
					}
					delay_ms(100);
				}
				memcpy(p->device_id,terminal_id[0],8);
			#endif
			
		  p->data_buff[0]=LoRA_Speed_Grade_Set(lora_speed_set,1,1);
						
			p->data_length[0]=0x00;
			p->data_length[1]=0x01;
			
		}
		break;
		
		//平台查询所在组速率等级
		case platform_request_g_lora_speed:
		{
			uint32_t u32_lora_spd_in_flash=0;
			STMFLASH_Read(Group_LORA_Speed_Grade,&u32_lora_spd_in_flash,1);
			p->data_buff[0]=(u8)u32_lora_spd_in_flash;
			
			p->data_length[0]=0x00;
			p->data_length[1]=0x01;
		}
		break;
		
		//设置所在组信道
		case set_g_lora_channel:
		{
      uint8_t lora_channel_set=p->data_buff[0];
      #if gateway
				uint8_t node_serial=0;
				uint8_t len=(uint16_t)p->data_length[0]|p->data_length[1];
				for(node_serial=1;node_serial<9;++node_serial){
					uint8_t i=0;
					memcpy(p->device_id,terminal_id[node_serial],8);
					p->func_code[1]=platform_issue;
				  Data_Package(len,p);
					for(i=0;i<10;++i){
						LORA_send_data((char*)packet,len+21);
						delay_ms(100);
					}
					delay_ms(200);
				}
				memcpy(p->device_id,terminal_id[0],8);
			#endif
			
		  p->data_buff[0]=LoRA_Channel_Set(lora_channel_set,1,1);
			
			p->data_length[0]=0x00;
			p->data_length[1]=0x01;
		}
		break;
		
		//平台请求所在组信道参数
		case platform_request_g_lora_channel:
		{
      uint32_t u32_lora_ch_in_flash=0;
			STMFLASH_Read(Group_LORA_Channel,&u32_lora_ch_in_flash,1);
			p->data_buff[0]=(u8)u32_lora_ch_in_flash;
			
			p->data_length[0]=0x00;
			p->data_length[1]=0x01;
		}
		break;
		
		//设置所在组速率等级和信道
		case set_g_lora_speed_channel:
		{
			uint8_t lora_speed_grade_set=p->data_buff[0],lora_channel_set=p->data_buff[1];
      #if gateway
				uint8_t node_serial=0;
				uint8_t len=(uint16_t)p->data_length[0]|p->data_length[1];
				for(node_serial=1;node_serial<9;++node_serial){
					uint8_t i=0;
					memcpy(p->device_id,terminal_id[node_serial],8);
					p->func_code[1]=platform_issue;
				  Data_Package(len,p);
					for(i=0;i<5;++i){
						LORA_send_data((char*)packet,len+21);
						delay_ms(100);
					}
					delay_ms(500);
				}
				memcpy(p->device_id,terminal_id[0],8);
			#endif
			
		  LoRA_SPD_CH_Set(lora_speed_grade_set,lora_channel_set);
		}
		break;
				//查询所在组速率等级和信道
		case platform_request_g_lora_speed_ch:
		{
      uint32_t u32_lora_spd_in_flash,u32_lora_ch_in_flash=0;
			STMFLASH_Read(Group_LORA_Speed_Grade,&u32_lora_spd_in_flash,1);
			p->data_buff[0]=(u8)u32_lora_spd_in_flash;
			
			STMFLASH_Read(Group_LORA_Channel,&u32_lora_ch_in_flash,1);
			p->data_buff[1]=(u8)u32_lora_ch_in_flash;
			
			p->data_length[0]=0x00;
			p->data_length[1]=0x02;
		}
		break;
		//修改服务器ip地址
		case server_addr_modify:
		{
			uint32_t server_addr_to_write[2];
			memcpy(server_addr_to_write,p->data_buff,6);
			p->data_buff[0]=ec20_modify_server_addr(server_addr_to_write);
			p->data_length[0]=0;
			p->data_length[1]=1;
			ec20_reconnect_flag=1;
		}
			break;
		
		default:
			break;
	}
  Data_Send(p);
}

//ffff ffff
void Sensor_Aquire(Data_Packet_t *p){
		uint8_t sensor_id,buff_num=2;
		if(device_type_num==0){//设备A
			//Usart1 Sensor --对应PCB板U16/17
	    sensor_id=1;
			uint8_t sensor_type[3]={CH4,NH3,CO};

			for(uint8_t i=1;i<3;++i){
				if(MH4xx_ZE_Get_Value(sensor_type[i], &p->data_buff[buff_num+1])){
					p->data_buff[buff_num]=sensor_id+i;
					buff_num+=3;
				}
			}
			
			if(PMS_Data_Read(&p->data_buff[buff_num+1])){
				p->data_buff[buff_num]=4;
				buff_num+=5;
				}
		}
		else if(device_type_num==1){ //设备B
			sensor_id=5;
			uint8_t sensor_type[4]={SO2,H2S,NO2,CO2};

			sensor_id=5;
			for(uint8_t i=0;i<4;++i){
				if(MH4xx_ZE_Get_Value(sensor_type[i], &p->data_buff[buff_num+1])){
					p->data_buff[buff_num]=sensor_id+i;
					buff_num+=3;
				}
			}
		}
		else if(device_type_num==2){
		
			sensor_id=0x01;	
			if(MH4xx_ZE_Get_Value(CH4,&p->data_buff[buff_num+1])){
				p->data_buff[buff_num]=sensor_id;
				buff_num+=3;
			}
			sensor_id=0x02;	
			if(MH4xx_ZE_Get_Value(NH3,&p->data_buff[buff_num+1])){
				p->data_buff[buff_num]=sensor_id;
				buff_num+=3;
			}					
			sensor_id=0x04;
			if(PMS_Data_Read(&p->data_buff[buff_num+1])){
				p->data_buff[buff_num]=sensor_id;
				buff_num+=5;
			}
		}
    
		else if(device_type_num==3){
			
			sensor_id=0x03;	
			if(MH4xx_ZE_Get_Value(CO,&p->data_buff[buff_num+1])){
				p->data_buff[buff_num]=sensor_id;
				buff_num+=3;
			}
			sensor_id=0x06;	
			if(MH4xx_ZE_Get_Value(H2S,&p->data_buff[buff_num+1])){
				p->data_buff[buff_num]=sensor_id;
				buff_num+=3;
			}					
			
		}	
    else if(device_type_num==4){
			
//			sensor_id=0x05;	
//			if(MH4xx_ZE_Get_Value(SO2,&p->data_buff[buff_num+1])){
//				p->data_buff[buff_num]=sensor_id;
//				buff_num+=3;
//			}
//			sensor_id=0x07;	
//			if(MH4xx_ZE_Get_Value(NO2,&p->data_buff[buff_num+1])){
//				p->data_buff[buff_num]=sensor_id;
//				buff_num+=3;
//			}	
			sensor_id=0x08;	
			if(MH4xx_ZE_Get_Value(CO2,&p->data_buff[buff_num+1])){
				p->data_buff[buff_num]=sensor_id;
				buff_num+=3;
			}					
		}
		 else if(device_type_num==5){
			
			sensor_id=0x03;	
			if(MH4xx_ZE_Get_Value(CO,&p->data_buff[buff_num+1])){
				p->data_buff[buff_num]=sensor_id;
				buff_num+=3;
			}
			sensor_id=0x07;	
			if(MH4xx_ZE_Get_Value(NO2,&p->data_buff[buff_num+1])){
				p->data_buff[buff_num]=sensor_id;
				buff_num+=3;
			}	
			sensor_id=0x04;	
			if(PMS_Data_Read(&p->data_buff[buff_num+1])){
				p->data_buff[buff_num]=4;
				buff_num+=5;
			}		
		}
		else;
		
	//I2C1 Sensor
	if(SHT30_Acquire(0x44,&p->data_buff[buff_num+1])){
				p->data_buff[buff_num]=9;
				buff_num+=5;
			}	


//	if(max30105_Acquire(&p->data_buff[buff_num+1])){
//		p->data_buff[buff_num]=10;
//		buff_num+=3;
//	}	
//	
	p->data_length[0]=0;
	p->data_length[1]=buff_num;
}

void Sensor_Data_Update(void){
	Data_pack->func_code[1]=device_upload;
	Data_pack->func_code[2]=(uint8_t)(active_upload>>8);
	Data_pack->func_code[3]=(uint8_t)active_upload;
  memcpy(&Data_pack->data_buff[0],&sensor_status[0],2);
	
	Sensor_Aquire(Data_pack);
  
}

void Data_Upload(uint8_t terminal_num){
	
	if(terminal_num==1){
		Serial_num++;
		Data_pack->serial_num[0]=(uint8_t)(Serial_num>>8);
		Data_pack->serial_num[1]=(uint8_t)Serial_num;
		Data_Send(Data_pack);
	}
	else
		Data_Acquire(terminal_num);
}

void Data_Acquire(uint8_t terminal_num){
	
	//Data_Packet_t *Terminal_Packet= malloc(sizeof(Data_Packet_t));
	Terminal_Packet->data_head[0]=0x4a;
	Terminal_Packet->data_head[1]=0x54;
	memcpy(Terminal_Packet->device_id,terminal_id[terminal_num],8);
	Terminal_Packet->serial_num[0]=0x00;
	Terminal_Packet->serial_num[1]=0x00;
	Terminal_Packet->encry_mode=Encry_mode;
	Terminal_Packet->func_code[0]=func_code_type;
	Terminal_Packet->func_code[1]=platform_issue;
	Terminal_Packet->func_code[2]=(uint8_t)(active_upload>>8);
	Terminal_Packet->func_code[3]=(uint8_t)active_upload;
	Terminal_Packet->data_length[0]=0x00;
	Terminal_Packet->data_length[1]=0x00;
	uint16_t length =0;
	Data_Package(length,Terminal_Packet);
	LORA_send_data((char *)packet,length+21);
	
}
void Read_Software_Version(Data_Packet_t *p){
	for(int i=0;i<8;++i){
		p->data_buff[i]=(Sofeware_Version>>((7-i)*8));
	}
	p->data_length[0]=0;
	p->data_length[1]=8;
}

iapfun jump2app; 
void iap_load_app(u32 appxaddr)
{	
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		//printf("IAP_LOADing\r\n");
		delay_ms(50);
		jump2app();									//跳转到APP.
	}
	else 
	{
		printf("Error\n");//错误复位
		NVIC_SystemReset();
	}
}		 


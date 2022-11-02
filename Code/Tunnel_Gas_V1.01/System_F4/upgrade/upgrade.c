#include "upgrade.h"
#include "command.h"
#include "sys.h"
#include "string.h"
#include "led.h"
#include "delay.h"
#include "ec20.h"
#include "timer.h"
#include "stmflash.h"
#include "usart.h"
#include "LORA.h"
#include "wdg.h"


//��ŴӶ�Ӧ��ַȡ���ı��������߲�����Ӧ�ı���
u32 gateway_upgrade_flag=0;
u32 gateway_version_now=0;
u32 gateway_version_new=0;
u32 gatway_total_byte_nums=0;
u32 gatway_reve_byte_nums=0;
u32 gatway_total_package_nums=0;
u32 gatway_reve_package_nums=0;

u32 node_upgrade_flag=0;
u32 node_version_now=0;
u32 node_version_new=0;
u32 node_total_byte_nums=0;
u32 node_reve_byte_nums=0;
u32 node_total_package_nums=0;
u32 node_reve_package_nums=0;

u32 send_to_node_upgrade_status=0;
u32 send_to_node_upgrade_cnt1=0;
u32 send_to_node_upgrade_cnt2=0;
u32 send_to_node_package_nums1=0;
u32 send_to_node_package_nums2=0;
u32 send_to_node_package_nums3=0;
u32 send_to_node_package_nums4=0;
u32 send_to_node_package_nums5=0;

//�������
u32 pa_msg_buff[164]={0};
u32 flash_important_mes[8]={0};

u32 gatway_curr_package_nums=0;
u32 node_curr_package_nums=0;

//�����ڵ�İ���
u32 send_to_node_package_serial=0;
//�ӽڵ��յ����������
u32 reve_fr_node_package_serial=0;
//�����ڵ�����ܼ���
u32 send_to_node_package_total_nums=0;

//ÿ���豸��������״̬
u8 each_node_upgrade_status=0;
//���ٸ��ڵ��豸
u16 total_node_dev_nums=9;
//�ڵ������Ƭ�δ�С
u8 node_request_upgrade_mode_of_each_package=0;
//Ӧ�÷����ڵ��ÿ�����ֽ���
u16 should_send_to_node_nums_of_each_package=128;
//���һ��ʣ����ֽ�������ֻ��һ����ʱ����ֽ���
u16 remain_should_send_to_node_nums=0;

uint8_t enter_send_upgrade_meg_mode=0;
uint8_t lora_send_msg[256],head[2]={0x4A,0X54};
uint8_t current_node_id[8]={};
uint8_t communication_SN[2]={0,};
	
void firmware_send_process(void)
{
	u16 node_serial;
	u16 crc16_value=0;
  TIM_Cmd(TIM3,DISABLE);
	TIM2_Init(9999,8399);
	uint8_t node_upgrade_times=0;

		send_to_node_upgrade_status=STMFLASH_ReadWord(send_to_node_upgrade_status_addr);
	  
	
		for(node_serial=1;node_serial<total_node_dev_nums;node_serial++)
		{
			memcpy(current_node_id,terminal_id[node_serial],8);
			send_to_node_package_serial=0;
			
			each_node_upgrade_status=(u8)((send_to_node_upgrade_status>>node_serial*2)&0x03);//�жϵ���λ
			if((0x01==each_node_upgrade_status)||(0x02==each_node_upgrade_status))
			{
				memcpy(lora_send_msg,head,2);
				memcpy(&lora_send_msg[2],current_node_id,8);
				lora_send_msg[10]=0x00;
				lora_send_msg[11]=0x00;//���к�
				lora_send_msg[12]=Encry_mode;//���ܷ�ʽ
				lora_send_msg[13]=func_code_type;
				lora_send_msg[14]=platform_issue;
				lora_send_msg[15]=upgrade_inform_cmd_high;
				lora_send_msg[16]=upgrade_inform_cmd_low;//ָ����
				lora_send_msg[17]=0x00;
				lora_send_msg[18]=0x06;//����
				lora_send_msg[19]=node_upgrade;
				node_version_new=STMFLASH_ReadWord(node_version_new_addr);
				lora_send_msg[20]=node_version_new;
				node_total_byte_nums=STMFLASH_ReadWord(node_total_byte_nums_addr);
				lora_send_msg[21]=node_total_byte_nums>>24;
				lora_send_msg[22]=node_total_byte_nums>>16;
				lora_send_msg[23]=node_total_byte_nums>>8;
				lora_send_msg[24]=node_total_byte_nums;
				
				crc16_value=Check_Sum_Cal(lora_send_msg,25);
				lora_send_msg[25]=crc16_value/256;
				lora_send_msg[26]=crc16_value%256;//У��
				memset(USART_RX_BUF[0],0,USART_REC_LEN);
				LORA_send_data((char*)lora_send_msg,27);
				
				upgrade_cnt=0;
				upgrade_timeout_flag=0;
				TIM_Cmd(TIM2,ENABLE);//ʹ��timer2 ��Ϊ������ʱ������
				
				//�Ȼظ�
				while((1!=node_upgrade_rev_process())&&(!upgrade_timeout_flag)){
					delay_ms(50);
				#if iwatch_dog_work
					IWDG_Feed();
				#endif
				}
//				if(upgrade_timeout_flag){
//					TIM_Cmd(TIM2,DISABLE);
//					upgrade_cnt=0;//��ʱ��������
//					upgrade_timeout_flag=0;//������ʱFLAG����
//				}
				
				while((enter_send_upgrade_meg_mode)&&(node_upgrade_times<=3))
				{
					
					
				  memset(lora_send_msg,0,27);
					node_upgrade_times++;
					upgrade_cnt=0;//��ʱ��������
					upgrade_timeout_flag=0;//������ʱFLAG����
					
					#if iwatch_dog_work
						IWDG_Feed();
					#endif
					
					while(!upgrade_timeout_flag)//�ж��Ƿ�ʱ���趨ʱ��Ϊ30s
			    {
						#if iwatch_dog_work
							IWDG_Feed();
						#endif
						uint8_t upgrade_process_rev=node_upgrade_rev_process();
						if(2==upgrade_process_rev)//�յ���������
						{
							upgrade_cnt=0;//�յ��������� ��ʱ��������
							
							reve_fr_node_package_serial=((uint16_t)USART_RX_BUF[0][21]<<8)+USART_RX_BUF[0][22];
							memcpy(&lora_send_msg[10],&USART_RX_BUF[0][10],2);// �������к�
							node_request_upgrade_mode_of_each_package=USART_RX_BUF[0][23];
							memset(USART_RX_BUF[0],0,USART_REC_LEN);
							switch(node_request_upgrade_mode_of_each_package)
							{
								case 1:
									should_send_to_node_nums_of_each_package=128;
									break;
								case 2:
									should_send_to_node_nums_of_each_package=256;
									break;
								case 3:
									should_send_to_node_nums_of_each_package=512;
									break;
								case 4:
									should_send_to_node_nums_of_each_package=1024;
									break;
								case 5:
									should_send_to_node_nums_of_each_package=2048;
									break;
								case 6:
									should_send_to_node_nums_of_each_package=4096;
									break;
								case 7:
									should_send_to_node_nums_of_each_package=200;
									break;
								default:
									should_send_to_node_nums_of_each_package=200;
									break;
							}
							
		
							uint8_t temp=node_total_byte_nums%should_send_to_node_nums_of_each_package;
							if(0x00==temp)
							{
								node_total_package_nums=node_total_byte_nums/should_send_to_node_nums_of_each_package;
								
								node_total_package_nums-=1;
							}
							else
							{
								node_total_package_nums=node_total_byte_nums/should_send_to_node_nums_of_each_package+1;
								
								node_total_package_nums-=1;
							}
							memcpy(lora_send_msg,head,2);
							memcpy(&lora_send_msg[2],current_node_id,8);

							lora_send_msg[12]=Encry_mode;//���ܷ�ʽ
							lora_send_msg[13]=func_code_type;
							lora_send_msg[14]=platform_issue;
							lora_send_msg[15]=upgrade_request_cmd_high;
							lora_send_msg[16]=upgrade_request_cmd_low;//ָ����

							lora_send_msg[19]=node_upgrade;
							lora_send_msg[20]=node_version_new;//��flash
							lora_send_msg[21]=(uint8_t)(reve_fr_node_package_serial>>8);
							lora_send_msg[22]=(uint8_t)reve_fr_node_package_serial;
							
							if(node_total_package_nums==reve_fr_node_package_serial)//���һ������ֻ��һ��
								{
									remain_should_send_to_node_nums=node_total_byte_nums-(should_send_to_node_nums_of_each_package*reve_fr_node_package_serial);
									

									if(remain_should_send_to_node_nums%4)
										remain_should_send_to_node_nums=(remain_should_send_to_node_nums/4+1)*4;
									STMFLASH_Read((Node_firmware_bak_addr+reve_fr_node_package_serial*should_send_to_node_nums_of_each_package),(u32 *)&lora_send_msg[23],remain_should_send_to_node_nums/4);//?????????????
									lora_send_msg[17]=(uint8_t)((remain_should_send_to_node_nums+4)>>8);
									lora_send_msg[18]=(uint8_t)(remain_should_send_to_node_nums+4);//����
									crc16_value=Check_Sum_Cal(lora_send_msg,23+remain_should_send_to_node_nums);
									lora_send_msg[23+remain_should_send_to_node_nums]=crc16_value/256;
									lora_send_msg[24+remain_should_send_to_node_nums]=crc16_value%256;//У��
									
									LORA_send_data((char *)lora_send_msg,25+remain_should_send_to_node_nums);
								}
								else
								{
									STMFLASH_Read((Node_firmware_bak_addr+reve_fr_node_package_serial*should_send_to_node_nums_of_each_package),(u32 *)&lora_send_msg[23],should_send_to_node_nums_of_each_package/4);//???
									lora_send_msg[17]=(uint8_t)((should_send_to_node_nums_of_each_package+4)>>8);
									lora_send_msg[18]=(uint8_t)(should_send_to_node_nums_of_each_package+4);//����
									crc16_value=Check_Sum_Cal(lora_send_msg,23+should_send_to_node_nums_of_each_package);
									lora_send_msg[23+should_send_to_node_nums_of_each_package]=crc16_value/256;
									lora_send_msg[24+should_send_to_node_nums_of_each_package]=crc16_value%256;//У��
									
									LORA_send_data((char *)lora_send_msg,25+should_send_to_node_nums_of_each_package);
								}
								
							
							
							//send_to_node_package_serial++;
							
							STMFLASH_Read(Upgrade_Parameter_BASE,pa_msg_buff,164);
							pa_msg_buff[17]=send_to_node_upgrade_cnt1_addr;
							pa_msg_buff[19+node_serial]=reve_fr_node_package_serial;
							
							STMFLASH_Write(Upgrade_Parameter_BASE,pa_msg_buff,164);
							
							memset(lora_send_msg,0,25+should_send_to_node_nums_of_each_package);
						}
						else if(3==upgrade_process_rev)//�յ��������
						{
							enter_send_upgrade_meg_mode=0;
							send_to_node_package_serial=0;
							each_node_upgrade_status=0;
							break;
						}
					}
				}
				enter_send_upgrade_meg_mode=0;
				//ֹͣ��ʱ������ѭ��	
				TIM_Cmd(TIM2,DISABLE);
			}
			else if(0x03==each_node_upgrade_status)
			{
			
			}
			else
			{
				each_node_upgrade_status=0;
			}
		}
	
		#if 1
		LED1=0;
		delay_ms(500);
		LED1=1;
		delay_ms(500);
		#else
		LED0=0;
		delay_ms(500);
		LED0=1;
		delay_ms(500);
		#endif
		

		TIM_Cmd(TIM3,ENABLE);
}

int8_t node_upgrade_rev_process(void)
{
	u16 cmdlenth=0;
	u16 crc16_value=0;
	
	
	if(command_recieved_flag_LORA)//���ⲻ������
	{
		command_recieved_flag_LORA=0;
		if(0x00==strncmp((const char *)&USART_RX_BUF[0],(const char *)head,2))//�ǰ�ͷ����
		{
			if(USART_RX_BUF[0][13]==func_code_type)//�������ָ��
			{
				//���豸��Ӧ���豸
				//memcpy(current_node_id,&USART_RX_BUF[0][2],8);
				if(memcmp((char *)&current_node_id,&USART_RX_BUF[0][2],8)==0)
				{
					cmdlenth=USART_RX_BUF[0][17]<<8|USART_RX_BUF[0][18];
//					if(cmdlenth==(USART_RX_CNT[0]-21))//���ݳ��ȶ�
//					{
						crc16_value=Check_Sum_Cal(USART_RX_BUF[0],cmdlenth+19);
						
						if(crc16_value==(USART_RX_BUF[0][cmdlenth+19]<<8|USART_RX_BUF[0][cmdlenth+20]))//������ȷ
						{		
							//����һ���̼�
							
							if((0x20==USART_RX_BUF[0][15])&&(0x03==USART_RX_BUF[0][16]))
							{
								enter_send_upgrade_meg_mode=1;
								memset(USART_RX_BUF[0],0,USART_REC_LEN);
								return 1;
							}
							else if((0x20==USART_RX_BUF[0][15])&&(0x04==USART_RX_BUF[0][16]))
							{
								//memset(USART_RX_BUF[0],0,USART_REC_LEN);
								return 2;
							}
							else if((0x20==USART_RX_BUF[0][15])&&(0x05==USART_RX_BUF[0][16]))
							{
								memset(USART_RX_BUF[0],0,USART_REC_LEN);
								return 3;
							}
							else
							{
								memset(USART_RX_BUF[0],0,USART_RX_CNT[0]);
								//USART_RX_CNT[0]=0;
								
								//��������
								
								return 0;
							}
						}
						else
						{
							//���ش�����Ϣ��ƽ̨��.
										
							//��ͷ
//							memcpy(lora_send_msg,head,2);
//							//mac��ַ
//							memcpy(&lora_send_msg[2],current_node_id,8);
//							//svn
//							lora_send_msg[10]=USART_RX_BUF[0][10];//���кŵ�һ���ֽ���ʱ�̶�Ϊ0x00.ʵ�ʾ�Э���Ϊ���ӽڵ�Child_node
//							lora_send_msg[11]=USART_RX_BUF[0][11];//��Э��İ����к�ת��Ϊ��Э������к�.
//							//����
//							lora_send_msg[12]=Encry_mode;//������ʱδ0x00�����ܡ�
//							//����
//							lora_send_msg[13]=func_code_type;
//							lora_send_msg[14]=0x02;//�豸�������ݵ�ƽ̨
//							lora_send_msg[15]=0x00;//��15��16��ͬ��ʾһ���¼�������ֻ����һ��16.
//							lora_send_msg[16]=0x01;
//							//���ݳ���
//							lora_send_msg[17]=0x00;//��Ϊ��Э��ĳ���ֻ��һ���ֽڣ�������Э��16λ���ȵĸ�8λ�ݶ�Ϊ0x00
//							lora_send_msg[18]=0x00;
//							//CRC		
//							crc16_value=Check_Sum_Cal(lora_send_msg,19);
//							lora_send_msg[19]=crc16_value/256;
//							lora_send_msg[20]=crc16_value%256;
//							
//							LORA_send_data((char *)lora_send_msg,21);
							memset(USART_RX_BUF[0],0,USART_REC_LEN);
//							USART_RX_CNT[0]=0;
							
							return -1;
						}
//					}
//					else
//					{
//						//���ش�����Ϣ��ƽ̨��.
//									
//						//��ͷ
//						memcpy(lora_send_msg,head,2);
//						//mac��ַ
//						memcpy(&lora_send_msg[2],current_node_id,8);
//						//svn
//						lora_send_msg[10]=USART_RX_BUF[0][10];//���кŵ�һ���ֽ���ʱ�̶�Ϊ0x00.ʵ�ʾ�Э���Ϊ���ӽڵ�Child_node
//						lora_send_msg[11]=USART_RX_BUF[0][11];//��Э��İ����к�ת��Ϊ��Э������к�.
//						//����
//						lora_send_msg[12]=Encry_mode;//������ʱδ0x00�����ܡ�
//						//����
//						lora_send_msg[13]=func_code_type;
//						lora_send_msg[14]=0x02;//�豸�������ݵ�ƽ̨
//						lora_send_msg[15]=0x00;//��15��16��ͬ��ʾһ���¼�������ֻ����һ��16.
//						lora_send_msg[16]=0x03;
//						//���ݳ���
//						lora_send_msg[17]=0x00;//��Ϊ��Э��ĳ���ֻ��һ���ֽڣ�������Э��16λ���ȵĸ�8λ�ݶ�Ϊ0x00
//						lora_send_msg[18]=0x00;
//						//CRC		
//						crc16_value=Check_Sum_Cal(lora_send_msg,19);
//						lora_send_msg[19]=crc16_value/256;
//						lora_send_msg[20]=crc16_value%256;
//						
//						LORA_send_data((char *)lora_send_msg,21);
//						memset(USART_RX_BUF[0],0,USART_RX_CNT[0]);
//						USART_RX_CNT[0]=0;
//						
//						return -2;
//					}
   			}
				else
				{
					//���ش�����Ϣ��ƽ̨��.
								
					//��ͷ
//					memcpy(lora_send_msg,head,2);
//					//mac��ַ
//					memcpy(&lora_send_msg[2],current_node_id,8);
//					//svn
//					lora_send_msg[10]=USART_RX_BUF[0][10];//���кŵ�һ���ֽ���ʱ�̶�Ϊ0x00.ʵ�ʾ�Э���Ϊ���ӽڵ�Child_node
//					lora_send_msg[11]=USART_RX_BUF[0][11];//��Э��İ����к�ת��Ϊ��Э������к�.
//					//����
//					lora_send_msg[12]=Encry_mode;//������ʱδ0x00�����ܡ�
//					//����
//					lora_send_msg[13]=func_code_type;//����ۺϸп�
//					lora_send_msg[14]=0x02;//�豸�������ݵ�ƽ̨
//					lora_send_msg[15]=0x00;//��15��16��ͬ��ʾһ���¼�������ֻ����һ��16.
//					lora_send_msg[16]=0x04;
//					//���ݳ���
//					lora_send_msg[17]=0x00;//��Ϊ��Э��ĳ���ֻ��һ���ֽڣ�������Э��16λ���ȵĸ�8λ�ݶ�Ϊ0x00
//					lora_send_msg[18]=0x00;
//					//CRC		
//					crc16_value=Check_Sum_Cal(lora_send_msg,19);
//					lora_send_msg[19]=crc16_value/256;
//					lora_send_msg[20]=crc16_value%256;
//					
//					LORA_send_data((char *)lora_send_msg,21);
					memset(USART_RX_BUF[0],0,USART_REC_LEN);
					//USART_RX_CNT[0]=0;
					
					return -3;
				}
			}
			else
			{
//				//���ش�����Ϣ��ƽ̨��.
//							
//				//��ͷ
//				memcpy(lora_send_msg,head,2);
//				//mac��ַ
//				memcpy(&lora_send_msg[2],current_node_id,8);
//				//svn
//				lora_send_msg[10]=USART_RX_BUF[0][10];//���кŵ�һ���ֽ���ʱ�̶�Ϊ0x00.ʵ�ʾ�Э���Ϊ���ӽڵ�Child_node
//				lora_send_msg[11]=USART_RX_BUF[0][11];//��Э��İ����к�ת��Ϊ��Э������к�.
//				//����
//				lora_send_msg[12]=Encry_mode;//������ʱδ0x00�����ܡ�
//				//����
//				lora_send_msg[13]=func_code_type;//����ۺϸп�
//				lora_send_msg[14]=0x02;//�豸�������ݵ�ƽ̨
//				lora_send_msg[15]=0x00;//��15��16��ͬ��ʾһ���¼�������ֻ����һ��16.
//				lora_send_msg[16]=0x02;
//				//���ݳ���
//				lora_send_msg[17]=0x00;//��Ϊ��Э��ĳ���ֻ��һ���ֽڣ�������Э��16λ���ȵĸ�8λ�ݶ�Ϊ0x00
//				lora_send_msg[18]=0x00;
//				//CRC		
//				crc16_value=Check_Sum_Cal(lora_send_msg,19);
//				lora_send_msg[19]=crc16_value/256;
//				lora_send_msg[20]=crc16_value%256;
//				
//				LORA_send_data((char *)lora_send_msg,21);
				memset(USART_RX_BUF[0],0,USART_REC_LEN);
//				//USART_RX_CNT[0]=0;
//				
				return -4;
			}
		}
		else 
		{
			//������ȷ������
			memset(USART_RX_BUF[0],0,USART_REC_LEN);
			//USART_RX_CNT[0]=0;
			
			return -5;
		}
	}
	return 0;
}


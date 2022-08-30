#include "bluetooth.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "usart.h"
#include "stm32wlxx_LM401.h"

static const char cmd_exe_success_str[]="AT+ok";
static const char cmd_exe_err_str[]="AT+err_code";
unsigned char BleRxBuffer[255];
unsigned short BleRXBufferSize;
BLE_Param_T Ble;
/**
  * @brief  Initialize the BlueTooth Interface
  * @note   BlueTooth Module work as a bridge between mobile command 
	*					emmiter and MCU of device.
  *            
	* @param	void
  * @retval void
  */
void BlueTooth_Init(void)
{
	BSP_MultiPlexer_Init();
	BlueTooth_IO_Config();
	HAL_Delay(5000);
	//BlueTooth_default_Para_Config();
	//BlueTooth_Enter_CMD_Mode();
}

/**
  * @brief  Config the  IO Port connectting to BT Module
  * @note   BlueTooth Connect to MCU with UART(TX/RX), RSTN 
	*					port , user need to fill the function according to
	*         board design
	*					      
	* @param	void
  * @retval void
  */
void BlueTooth_IO_Config(void)
{
	BSP_BLE_RSTN_PIN_Init();
}

/**
  * @brief  Config the default parameter of BlueTooth Module
  * @note   User set the default parameters of BlueTooth ,
	*         parameters will be valid only if restart the moudle 
 	*         by hardware
	*					      
	* @param	[in] BaudRate set the baudrate
	*												2400 <= baudrate <= 115200
	*                       [0:2400, 1:4800, 2:9600, 3:19200,
	*												 4:38400,5:57600,6:115200 ]
	* @param  [in] bleName set the name of bluetooth module
	*
	* @param  [in] Addr    set the address of bluetooth module
	*                      
	* @param  [in] ConnInt set the connecting interval
	*                      parameters include [max],[min],[latency],[timeout]
	*                      max/min : [0x0006 ~ 0x0c80]* 1.25ms = 7.5~4000ms min<=max
	*                      Latency : [0x0000 ~ 0x01F3]
	*											 timeout: [0x000A ~ 0x0C80] 
	*										   [max*1.25*(1+latency)] < 10 * timeout.
	* @papram [in] AdvInt  set the interval of bluetooth broadcasting
	*	                     min/max:[0x50 ~ 0x3E80] * 0.625ms = 50~10000ms min<=max
	* @retval err_code
  */
void BlueTooth_default_Para_Config(BaudRate_Typedef baudrate,char *blename,char name_len,char *addr,char addr_len, ConnInt_T connint,AdvInt_T advint)
{
		Ble.Uart_Config.data_bits = Data_Bits_8b;
		Ble.Uart_Config.parity_bits = None;
		Ble.Uart_Config.stop_bits = Stop_Bits_1b;
		memcpy(Ble.blename,blename,name_len);
		memcpy(Ble.addr,addr,addr_len);
		Ble.connint = connint;
		Ble.advint = advint;
}

/**
  * @brief  Set the baudrate of BlueTooth Module
  * @note  
	*					      
	* @param	[in] BaudRate set the baudrate
	*												2400 <= baudrate <= 115200
	*                       [0:2400, 1:4800, 2:9600, 3:19200,
	*												 4:38400,5:57600,6:115200 ]
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_Set_BR(BaudRate_Typedef baudrate)
{
	Cmd_Error_Return_Typedef err_code=0;
	char dummy_buff[1]={0};
	unsigned short timeout = 500;
	char cmd[] ="AT+setBR 115200\r\n";
	Ble.Uart_Config.baudrate = baudrate;
	sprintf(cmd,"AT+serBR %d\r\n",baudrate);
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd),dummy_buff,timeout);
	return err_code;
}

/**
  * @brief  Read the name of BlueTooth Module
  * @note   
	*					      
	* @param	[out] *namebuff point of buffer where to copy bluetooth name
	*											
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_Get_Name(char *blename_point)
{
	char *blename_buff = blename_point;
	
	Cmd_Error_Return_Typedef err_code=0;
	unsigned short timeout = 500;
	char cmd[] ="AT+getName\r\n";
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd),blename_buff,timeout);
	return err_code;
}

/**
  * @brief  Set the name of BlueTooth Module
  * @note   
	*					      
	* @param	[in] *namebuff point of buffer where to copy bluetooth name
  *         [in] size      size of the buffer
	*											
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_Set_Name(char *blename_buff,char name_len)
{
	Cmd_Error_Return_Typedef err_code=0;
	unsigned short timeout = 500;
	char cmd[] ="AT+setName=";
	char dummy_buff[1]={0};
	strcat(cmd,blename_buff);
	strcat(cmd,"\r\n");
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd),dummy_buff,timeout);
	return err_code;
}

/**
  * @brief  Read the address of BlueTooth Module
  * @note   
	*					      
	* @param	[out] *address point of buffer where to copy bluetooth mac address
	*											
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_Get_Addr(char *bleaddr_point)
{
	char *bleaddr_buff = bleaddr_point;
	Cmd_Error_Return_Typedef err_code=0;
	unsigned short timeout = 500;
	char cmd[] ="AT+getAddr\r\n";
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd),bleaddr_buff,timeout);
	return err_code;
}

/**
  * @brief  Set the address of BlueTooth Module
  * @note   
	*					      
	* @param	[in] *address point of buffer where to copy bluetooth mac address
  *         [in] size      size of the buffer
	*											
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_Set_Addr(char *bleaddr_buff,char name_len)
{
	Cmd_Error_Return_Typedef err_code=0;
	unsigned short timeout = 500;
	char cmd[] ="AT+setAddr=";
	char dummy_buff[1] = {0};
	strcat(cmd,bleaddr_buff);
	strcat(cmd,"\r\n");
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd),dummy_buff,timeout);
	return err_code;
}

/**
  * @brief  Get the status of BlueTooth Module
  * @note   
	*					      
	* @param	[out] status status_code of mode
	*                      [00:idle, 01:broadcast, 02:connecting,
	*												03:scan with response, 04:scan without response] 
	*											
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_Get_Status(char *status_point)
{
	Cmd_Error_Return_Typedef err_code=0;
	char *status_buff = status_point;
	unsigned short timeout = 500;
	char cmd[] ="AT+getStatus\r\n";
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd),status_buff,timeout);
	return err_code;
}

/**
  * @brief  Set the interval of broadcasting
  * @note   include min and max value 
	*					      
	* @param	[in] min  minimun interval time of broadcastng

  * @param  [in] max maximun interval time of broadcasting
  *
	*					min/max:[0x50 ~ 0x3E80] * 0.625ms = 50~10000ms min<=max
	*                   							
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_Set_aAdvInt(AdvInt_T advint)
{
	Cmd_Error_Return_Typedef err_code=0;
	unsigned short timeout = 500;
	char dummy_buff[1]={0};
	char cmd[]="AT+setAdvInt %d %d\r\n";
	sprintf(cmd,"AT+setAdvInt %d %d\r\n",advint.Min,advint.Max);
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd),dummy_buff,timeout);
	return err_code;
}

/**
  * @brief  Set the interval of connecting
	* @note   parameters include [max],[min],[latency],[timeout]
	*
	 *@param  [in] max: maximun time of connecting interval
	*
	* @param  [in] min: minimun time of connecting interval
  *              min/max:[0x0006 ~ 0x0c80]* 1.25ms = 7.5~4000ms min<=max
  *
	* @param  [in] Latency: latency time of connecting interval
	*              Latency : [0x0000 ~ 0x01F3]
  *
	* @param  [in] timeout : timeout value of connecting
	*							 timeout: [0x000A ~ 0x0C80] [max*1.25*(1+latency)] < 10 * timeout.
  *                   							
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_Set_ConnInt(ConnInt_T connint)
{
	Cmd_Error_Return_Typedef err_code=0;
	unsigned short timeout = 500;
	char dummy_buff[1]={0};
	char cmd[]="AT+setConnInt %d %d %d %d\r\n";
	sprintf(cmd,"AT+setConnInt %d %d %d %d\r\n",connint.Max,connint.Min,connint.Latency,connint.Timeout);
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd),dummy_buff,timeout);
	return err_code;
}


/**
  * @brief  Bluetooth disconnect
	* @note   
	*
	* @param   void
  *                   							
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_DisConnect(void)
{
	Cmd_Error_Return_Typedef err_code=0;
	unsigned short timeout = 500;
	char cmd[] ="AT+disConnect\r\n";
	char dummy_buff[1] = {0};
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd),dummy_buff,timeout);
	return err_code;
}

/**
  * @brief  ReStart the bluetooth moudle
	* @note   
	*
	* @param  void
  *                   							
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_ReStart(void)
{
	Cmd_Error_Return_Typedef err_code=0;
	unsigned short timeout = 500;
	char cmd[] ="AT+reStart\r\n";
	char dummy_buff[1] = {0};
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd),dummy_buff,timeout);
	return err_code;
}

/**
  * @brief  Bluetooth Exit Cmd Mode
	* @note   transparent mode / AT command mode
	*
	* @param  void
  *                   							
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_Exit_CMD_Mode(void)
{
	Cmd_Error_Return_Typedef err_code=0;
	unsigned short timeout = 500;
	char cmd[] ="AT+exit\r\n";
	char dummy_buff[1] = {0};
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd),dummy_buff,timeout);
	return err_code;
}

/**
  * @brief  Bluetooth Enter Cmd Mode
	* @note   transparent mode / AT command mode
	*
	* @param  void
  *                   							
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_Enter_CMD_Mode(void)
{
	Cmd_Error_Return_Typedef err_code=0;
	unsigned short timeout = 500;
	char cmd[] ="+++";
	char dummy_buff[1] = {0};
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd)-1,dummy_buff,timeout);
	return err_code;
}

/**
  * @brief  Shutdown the bluetooth moudle
	* @note   
	*
	* @param  void
  *                   							
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_Shutdown(void)
{
	Cmd_Error_Return_Typedef err_code=0;
	unsigned short timeout = 500;
	char cmd[] ="AT+shutDown\r\n";
	char dummy_buff[1] = {0};
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd),dummy_buff,timeout);
	return err_code;
}

/**
  * @brief  set the bluetooth moudle with factory paramters 
	* @note   
	*
	* @param  void
  *                   							
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_Restore(void)
{
	Cmd_Error_Return_Typedef err_code=0;
	unsigned short timeout = 500;
	char cmd[] ="AT+reStore\r\n";
	char dummy_buff[1] = {0};
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd),dummy_buff,timeout);
	return err_code;
}

/**
  * @brief  query the vision information of bluetooth moudle 
	* @note   
	*
	* @param  [out] *info_buff : the point of buffer where to copy the 
	*                            information of vision
  *                   							
  * @retval error_code
  */
Cmd_Error_Return_Typedef BlueTooth_Get_Info(char *info_point)
{
	Cmd_Error_Return_Typedef err_code=0;
	char *info_buff = info_point;
	unsigned short timeout = 500;
	char cmd[] ="AT+getInfo\r\n";
	err_code = Ble_Cmd_Write_Read(cmd,sizeof(cmd),info_buff,timeout);
	return err_code;
}

/**
  * @brief  get the command help information 
	* @note   
	*
	* @param  [out] *help_info : the point of buffer where to copy the 
	*                            information of command help
  *                   							
  * @retval error_code
  */
void BlueTooth_Get_Help(void)
{
	
}

/**
  * @brief  Get the  parameter of BlueTooth Module
  * @note   parameters include baudrate£¬broadcast interval,connect interval
	*					      
	* @param	[out] param_struct
									BaudRate set the baudrate
	*												2400 <= baudrate <= 115200
	*                       [0:2400, 1:4800, 2:9600, 3:19200,
	*												 4:38400,5:57600,6:115200 ]
	*                      
	*								ConnInt set the connecting interval
	*                      parameters include [max],[min],[latency],[timeout]
	*                      max/min : [0x0006 ~ 0x0c80]* 1.25ms = 7.5~4000ms min<=max
	*                      Latency : [0x0000 ~ 0x01F3]
	*											 timeout: [0x000A ~ 0x0C80] 
	*										   [max*1.25*(1+latency)] < 10 * timeout.
	*								AdvInt  set the interval of bluetooth broadcasting
	*	                     min/max:[0x50 ~ 0x3E80] * 0.625ms = 50~10000ms min<=max
	* @retval err_code
  */
void BlueTooth_Get_Para(void)
{
	
}

/**
  * @brief  Ble CMD Write and receive response data
	* @note   
	*
	* @param  
  *                   							
  * @retval error_code
  */
Cmd_Error_Return_Typedef Ble_Cmd_Write_Read(char *cmd_str,char cmd_len ,char *return_data,unsigned short timeout)
{
	 	Ble_Uart_Status_T State = TX;
		char *ret;
		while(timeout!=0){
			switch(State)
			{
				case TX:
					BlueTooth_Data_Send(cmd_str,cmd_len);
					State=RX;
					break;
				case RX:
					if(BleRXBufferSize>0)
					{
						if((ret=(char*)strstr((const char *)BleRxBuffer,cmd_exe_success_str))!= NULL)
						{
							memcpy(return_data,(ret+sizeof(cmd_exe_success_str)),BleRXBufferSize-sizeof(cmd_exe_success_str));
							return cmd_exe_success;
						}
						
						else if((ret=(char*)strstr((const char *)BleRxBuffer,cmd_exe_err_str))!= NULL)
						{
							return(atoi(ret+sizeof(cmd_exe_err_str)));
						}
					}
					break;
			}
			timeout--;
		}
		return reply_timeout;
}

/**
  * @brief  BlueTooth Data Send Function
	* @note   User need to fill this function according to their own comminication Interface
	*
	* @param  
  *                   							
  * @retval error_code
  */
void BlueTooth_Data_Send(char *data_send,unsigned short data_len)
{
	HAL_UART_Transmit_IT(&huart1,(unsigned char *)data_send,data_len);
}
void BlueTooth_Process(void)
{


}


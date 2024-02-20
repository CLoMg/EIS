/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "net_io.h"
#include "net_device.h"
#include "onenet.h"
#include "selfcheck.h"
#include "sht3x.h"
#include "dsl_08.h"
#include "fault.h"
#include "mh4xx.h"
#include "stm32f1xx_hal_gpio.h"
#include "direct.h"
#include "sysparams.h"
#include "rtc.h"
#include "scd4x_i2c.h"
#include "iwdg.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define NET_TIME 60
unsigned short timerCount = 0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId RESENDHandle;
osThreadId SENSORHandle;
osThreadId SENDHandle;
osThreadId OTAHandle;
osThreadId DRPHandle;

osThreadId defaultTaskHandle;
osThreadId IWDGHandle;
osTimerId myTimer01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void RESEND_Task(void const * argument);
void IWDG_Task(void const * argument);
void SENSOR_Task(void const * argument);
void SEND_Task(void const * argument);
void OTA_Task(void const * argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void DRP_Task(void const * argument);
void OS_TimerCallback(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

//  /* Create the timer(s) */
//  /* definition and creation of myTimer01 */
//  osTimerDef(myTimer01, OS_TimerCallback);
//  myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);
//	

//  /* USER CODE BEGIN RTOS_TIMERS */
//  /* start timers, add new ones, ... */
//	osStatus timerresult = osOK;
//	timerresult = osTimerStart(myTimer01Handle,1000);
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
//  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
//  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

//  /* definition and creation of USART */
	osThreadDef(DRP, DRP_Task, osPriorityRealtime, 0, 128*3);
  DRPHandle = osThreadCreate(osThread(DRP), NULL);
  
//  /* USER CODE BEGIN RTOS_THREADS */
//  /* add threads, ... */
	osThreadDef(RESEND, RESEND_Task, osPriorityAboveNormal, 0, 128);
  RESENDHandle = osThreadCreate(osThread(RESEND), NULL);
//	
	osThreadDef(IWDG, IWDG_Task, osPriorityLow ,0, 64);
	IWDGHandle = osThreadCreate(osThread(IWDG), NULL);
//	

	osThreadDef(OTA, OTA_Task, osPriorityBelowNormal, 0, 128);
	OTAHandle = osThreadCreate(osThread(OTA), NULL);
//
	osThreadDef(SENSOR, SENSOR_Task, osPriorityNormal, 0, 128*2);//20230301 128*4 �� 128*2
	SENSORHandle = osThreadCreate(osThread(SENSOR), NULL);
////	
	osThreadDef(SEND, SEND_Task, osPriorityHigh, 0, 128*2);//20230301 128*4 �� 128*2
	SENDHandle = osThreadCreate(osThread(SEND), NULL);
//	

	

  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
		if(reconnect_flag == 1){
			reconnect_flag = 0;
			NET_DEVICE_Reset();														//�����豸��λ			
			netDeviceInfo.initStep = 0 ;
			NET_DEVICE_Init(oneNetInfo.ip, oneNetInfo.port)	;	
		}
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}


/* USER CODE BEGIN Header_USART_Task */
/*
************************************************************
*	�������ƣ�	DRP_Task
*
*	�������ܣ�	����ƽ̨�·�������
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		���ڽ�������������ģʽ��ʱ���ȴ�ƽ̨�·����������������
************************************************************
*/
/* USER CODE END Header_USART_Task */
void DRP_Task(void const * argument)
{
  /* USER CODE BEGIN USART_Task */
	if(OneNET_Init(ctx) < 0)
	{
		UsartPrintf(USART_DEBUG, "MqttSample_Init Failed\r\n");
	}
  /* Infinite loop */
  for(;;)
  {
		if(NET_DEVICE_Get_DataMode() == DEVICE_DATA_MODE)
		{
			if(OneNET_RecvPkt(ctx->mqttctx) == OneNETERR_NOERROR)
			{
				NET_DEVICE_ClrData();
			}
		}
    osDelay(10);
  }
  /* USER CODE END USART_Task */
}



/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/*
************************************************************
*	�������ƣ�	SENSOR_Task
*
*	�������ܣ�	���������ݲɼ�����ʾ
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		���������ݲɼ����񡣽�����Ӵ����������ݲɼ�����ȡ����ʾ
************************************************************
*/
void SENSOR_Task(void const * argument)
{
	unsigned int upload_cnt = 0;
	unsigned int PreWakeTime = osKernelSysTick();
	Direct_Init();
	scd4x_init();
	PM_Sensor_Init();
	//��ɳ�ʼ������ʱ500ms���ȴ��������ȶ�
	osDelay(5000);
	for(;;)
	{
		SCDxx_Process();
		AQI_Process();
	  HAL_GPIO_TogglePin(LED_ERR_GPIO_Port,LED_ERR_Pin);
		HAL_GPIO_TogglePin(LED_Run_GPIO_Port,LED_Run_Pin);

		
		if(upload_cnt % Upload_Period == 0){
			OneNet_SendData(0);
		}
		upload_cnt++;
		osDelayUntil(&PreWakeTime,1000);		
	}
}

/*
************************************************************
*	�������ƣ�	IWDG_Task
*
*	�������ܣ�	������Ź�
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		���Ź�����
************************************************************
*/
void IWDG_Task(void const * argument)
{
	unsigned  short reboot_cnt = 0;
	MX_IWDG_Init();
	while(1)
	{
	
		Iwdg_Feed(); 		//ι��
		reboot_cnt++;
		if(reboot_cnt > 60 * 60 )
			 Device_Reboot();
		osDelay(1000); 	//��������1000ms
	}

}

/*
************************************************************
*	�������ƣ�	RESEND_Task
*
*	�������ܣ�	�������
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		�������񡣷����������󲢵ȴ�������Ӧ�������趨ʱ����û����Ӧ������ƽ̨����
************************************************************
*/
void RESEND_Task(void const * argument)
{
  /* USER CODE BEGIN USART_Task */
		unsigned int PreWakeTime = osKernelSysTick();
  /* Infinite loop */
  for(;;)
  {
		OneNet_ReSend();
    osDelayUntil(&PreWakeTime,1000);	
  }
  /* USER CODE END USART_Task */
}


/*
************************************************************
*	�������ƣ�	SEND_Task
*
*	�������ܣ�	�ϴ�����������
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		���ݷ�������
************************************************************
*/
void SEND_Task(void const * argument)
{
	NET_DEVICE_IO_Init();													//�����豸IO��ʼ��
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);								//����Ϊ�����շ�ģʽ(����ESP8266Ҫ����AT�ķ��ػ���ƽ̨�·����ݵķ���)
	oneNetInfo.netWork =1;
	oneNetInfo.syncTime =1;
	for(;;)
	{
		OneNet_Send_Process();
		osDelay(10);
	}

}

/*
************************************************************
*	�������ƣ�	OTA_Task
*
*	�������ܣ�	�̼���������
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		  �̼��������񣬱Ƚϵ�ǰ�̼��汾�����¹̼��汾��
*             ���汾�и��£����ȡ���°汾�̼�
************************************************************
*/

void OTA_Task(void const * argument)
{
  Dev_OTA_Init();
	while(1)
	{
		if(FW_REC_VER > FW_Version)
			OneNET_FW_Query();
		osDelay(2000);													//��������25ms
	}
}
/* USER CODE END Application */

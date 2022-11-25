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
#include "battery.h"

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
osThreadId HEARTHandle;
osThreadId FAULTHandle;
osThreadId SENSORHandle;
osThreadId SENDHandle;
osThreadId NETHandle;

////������
//DATA_STREAM dataStream[] = {
//								{"Red_Led", &ledStatus.Led4Sta, TYPE_BOOL, 1},
//								{"Green_Led", &ledStatus.Led5Sta, TYPE_BOOL, 1},
//								{"Yellow_Led", &ledStatus.Led6Sta, TYPE_BOOL, 1},
//								{"Blue_Led", &ledStatus.Led7Sta, TYPE_BOOL, 1},
//								{"beep", &beepInfo.Beep_Status, TYPE_BOOL, 1},
//								{"temperature", &sht20Info.tempreture, TYPE_FLOAT, 1},
//								{"humidity", &sht20Info.humidity, TYPE_FLOAT, 1},
//								{"Xg", &adxlInfo.incidence_Xf, TYPE_FLOAT, 1},
//								{"Yg", &adxlInfo.incidence_Yf, TYPE_FLOAT, 1},
//								{"Zg", &adxlInfo.incidence_Zf, TYPE_FLOAT, 1},
//								{"errType", &faultTypeReport, TYPE_UCHAR, 1},
//							};
//unsigned char dataStreamLen = sizeof(dataStream) / sizeof(dataStream[0]);
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId DRPHandle;
osTimerId myTimer01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void HEART_Task(void const * argument);
void IWDG_Task(void const * argument);
void SENSOR_Task(void const * argument);
void SEND_Task(void const * argument);
void NET_Task(void const * argument);
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

//  /* Create the thread(s) */
//  /* definition and creation of defaultTask */
//  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
//  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of USART */
	osThreadDef(DRP, DRP_Task, osPriorityRealtime, 0, 128*4);
  DRPHandle = osThreadCreate(osThread(DRP), NULL);
  
//  /* USER CODE BEGIN RTOS_THREADS */
//  /* add threads, ... */
//	osThreadDef(HEART, HEART_Task, osPriorityHigh, 0, 128);
//  HEARTHandle = osThreadCreate(osThread(HEART), NULL);
//	
	osThreadDef(IWDG, IWDG_Task, osPriorityIdle, 0, 128);
	FAULTHandle = osThreadCreate(osThread(IWDG), NULL);
//	

	osThreadDef(NET, NET_Task, osPriorityLow, 0, 128*4);
	NETHandle = osThreadCreate(osThread(NET), NULL);
//
	osThreadDef(SENSOR, SENSOR_Task, osPriorityNormal, 0, 128*2);
	SENSORHandle = osThreadCreate(osThread(SENSOR), NULL);
//	
	osThreadDef(SEND, SEND_Task, osPriorityNormal, 0, 128*4);
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


/* USER CODE BEGIN Header_DRP_Task */
/*
************************************************************
*	�������ƣ�	DRP_Task
*
*	�������ܣ�	�������ݴ���
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����	�������ݴ�������������ģʽ��ʱ���ȴ�ƽ̨�·����������������
************************************************************
*/
/* USER CODE END Header_DRP_Task */
void DRP_Task(void const * argument)
{
  /* USER CODE BEGIN DRP_Task */
	if(MqttSample_Init(ctx) < 0)
	{
		UsartPrintf(USART_DEBUG, "MqttSample_Init Failed\r\n");
	}
  /* Infinite loop */
  for(;;)
  {
		if(NET_DEVICE_Get_DataMode() == DEVICE_DATA_MODE)
		{
			if(Mqtt_RecvPkt(ctx->mqttctx) == MQTTERR_NOERROR)
			{
				NET_DEVICE_ClrData();
			}
		}
    osDelay(10);
  }
  /* USER CODE END DRP_Task */
}


/* OS_TimerCallback function */
/*
************************************************************
*	�������ƣ�	OS_TimerCallBack
*
*	�������ܣ�	��ʱ�������״̬��־λ
*
*	��ڲ�����	�����ʱ�����
*
*	���ز�����	��
*
*	˵����		��ʱ�����񡣶�ʱ�������״̬�������������趨ʱ�����������ӣ������ƽ̨����
************************************************************
*/
void OS_TimerCallback(void const * argument)
{
  /* USER CODE BEGIN OS_TimerCallback */
	if(oneNetInfo.netWork == 0)											//�������Ͽ�
	{
		if(++timerCount >= NET_TIME) 									//�������Ͽ���ʱ
		{
			UsartPrintf(USART_DEBUG, "Tips:		Timer Check Err\r\n");
			
			checkInfo.NET_DEVICE_OK = 0;								//���豸δ����־
			
			NET_DEVICE_ReConfig(0);										//�豸��ʼ����������Ϊ��ʼ״̬
			
			oneNetInfo.netWork = 0;
			
			timerCount = 0;
		}
	}
	else
	{
		timerCount = 0;													//�������
	}
  /* USER CODE END OS_TimerCallback */
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
	uint8_t dummy_buff[20];
	scd4x_init();
	Direct_Init();
	PM_Sensor_Init();
	for(;;)
	{
		SCDxx_Process(dummy_buff);
		AQI_Process();
		PowerMonitor_Process();
		HAL_GPIO_TogglePin(LED_ERR_GPIO_Port,LED_ERR_Pin);
		HAL_GPIO_TogglePin(LED_Run_GPIO_Port,LED_Run_Pin);
		osDelay(5000); 										//��������500ms
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
	MX_IWDG_Init();
	while(1)
	{
	
		Iwdg_Feed(); 		//ι��
		
		osDelay(1000); 	//��������250ms
	
	}

}

/*
************************************************************
*	�������ƣ�	HEART_Task
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
void HEART_Task(void const * argument)
{
  /* USER CODE BEGIN USART_Task */
  /* Infinite loop */
  for(;;)
  {
		
		OneNet_HeartBeat();
		//UsartPrintf(USART_DEBUG,"HEART_TASK STACK REMAINED: %d \r\n",uxTaskGetStackHighWaterMark(HEARTHandle));
    osDelay(60000);
  }
  /* USER CODE END USART_Task */
}

/*
************************************************************
*	�������ƣ�	FAULT_Task
*
*	�������ܣ�	����״̬������
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		���ϴ������񡣵�������������豸����ʱ�����Ƕ�Ӧ��־λ��Ȼ���н��д���
************************************************************
*/
void FAULT_Task(void const * argument)
{

	while(1)
	{
		
		if(faultType != FAULT_NONE)									//��������־������
		{
			UsartPrintf(USART_DEBUG, "WARN:	Fault Process\r\n");
			Fault_Process();										//�����������
		}
		
		osDelay(50);											//��������50ms
	
	}

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

	for(;;)
	{
		OneNet_SendData(0);
		osDelay((unsigned int)Upload_Period*1000);

//		remainstack_size = uxTaskGetStackHighWaterMark(SENDHandle);
			//osDelay(1000);
	}

}

/*
************************************************************
*	�������ƣ�	NET_Task
*
*	�������ܣ�	�������ӡ�ƽ̨����
*
*	��ڲ�����	void���͵Ĳ���ָ��
*
*	���ز�����	��
*
*	˵����		���������������񡣻������������߼����������״̬������д�����״̬��Ȼ���������������
************************************************************
*/

void NET_Task(void const * argument)
{
//	UBaseType_t remainstack_size = 0; 
	NET_DEVICE_IO_Init();													//�����豸IO��ʼ��
	//NET_DEVICE_Reset();														//�����豸��λ
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);								//����Ϊ�����շ�ģʽ(����ESP8266Ҫ����AT�ķ��ػ���ƽ̨�·����ݵķ���)

	while(1)
	{
			//UsartPrintf(USART_DEBUG,"NETHandle STACK REMAINED: %d \r\n",uxTaskGetStackHighWaterMark(NETHandle));
			if(oneNetInfo.netWork != 1)			//��ʼ�������豸������������
			{
					oneNetInfo.netWork = 1;
					OneNet_Subscribe();
					osDelay(1000);
					OneNet_OnLine();
			}
		osDelay(25);													//��������25ms
		}
}
/* USER CODE END Application */


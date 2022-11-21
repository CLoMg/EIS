/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
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
#include "rtc.h"
#include "time.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 0x1;
  DateToUpdate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
long long int Read_Stamp(void)
{
	struct tm tm_new;
	uint16_t ms_h = 0U ,ms_l = 0U, Micro_Seconds = 0U; 
	long long int timestamp = 0;
	uint32_t SF = RTC_AUTO_1_SECOND & 0x000FFFFF ;
	RTC_DateTypeDef GetDate = {0};
	RTC_TimeTypeDef GetTime = {0};
	
	HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
		
//	ms_h = READ_REG(0x40002810 & RTC_DIVH_RTC_DIV);
//	ms_l = READ_REG(0x40002814 & RTC_DIVL_RTC_DIV);
	
	ms_h = (*(uint32_t *)0x40002810) & 0x000F;
	ms_l = (*(uint32_t *)0x40002814) & 0xFFFF;
	Micro_Seconds = ((float)(SF - (((uint32_t)ms_h<<16)|ms_l))) / SF * 1000;
	
	HAL_RTC_GetDate(&hrtc, &GetDate, RTC_FORMAT_BIN);
	
	tm_new.tm_sec = GetTime.Seconds;
	tm_new.tm_min = GetTime.Minutes;
	tm_new.tm_hour = GetTime.Hours - 8;
	tm_new.tm_mday = GetDate.Date;
	tm_new.tm_mon = GetDate.Month - 1;
	tm_new.tm_year = GetDate.Year + 100;
	
	timestamp = mktime(&tm_new); 
	timestamp *= 1000 ; 
	timestamp +=  Micro_Seconds;
		//timestamp = mktime(&tm_new);
	return timestamp;
}


void Dev_Set_Time(unsigned int timestamp)
{
	struct tm *stm= NULL;

	RTC_DateTypeDef RTC_DateStruct;
	RTC_TimeTypeDef	RTC_TimeStruct;
	
	stm = localtime(&timestamp);
	RTC_DateStruct.Year = stm->tm_year - 100;
	RTC_DateStruct.Month = stm->tm_mon + 1;
	RTC_DateStruct.Date = stm->tm_mday;
	RTC_TimeStruct.Hours = stm->tm_hour + 8;
	RTC_TimeStruct.Minutes = stm->tm_min;
	RTC_TimeStruct.Seconds = stm->tm_sec;
	
	HAL_RTC_SetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN);
	
}
/* USER CODE END 1 */

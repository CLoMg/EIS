/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED2_R_Pin GPIO_PIN_13
#define LED2_R_GPIO_Port GPIOC
#define LED2_L_Pin GPIO_PIN_1
#define LED2_L_GPIO_Port GPIOA
#define LED_Run_Pin GPIO_PIN_13
#define LED_Run_GPIO_Port GPIOB
#define LED_ERR_Pin GPIO_PIN_14
#define LED_ERR_GPIO_Port GPIOB
#define LORA_Wake_Pin GPIO_PIN_12
#define LORA_Wake_GPIO_Port GPIOA
#define LED1_L_Pin GPIO_PIN_15
#define LED1_L_GPIO_Port GPIOA
#define LED1_R_Pin GPIO_PIN_3
#define LED1_R_GPIO_Port GPIOB
#define LORA_nReset_Pin GPIO_PIN_8
#define LORA_nReset_GPIO_Port GPIOB
#define ADC_Bat_Pin GPIO_PIN_1
#define ADC_Bat_GPIO_Port GPIOB
#define Charge_Control_Pin GPIO_PIN_0
#define Charge_Control_GPIO_Port GPIOA

#define MultiPlexer_SW0_Pin GPIO_PIN_0
#define MultiPlexer_SW0_GPIO_Port GPIOB
#define MultiPlexer_SW1_Pin GPIO_PIN_7
#define MultiPlexer_SW1_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

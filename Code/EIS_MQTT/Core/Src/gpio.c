/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */
static GPIO_TypeDef* DIR_LIGHT_PORT[4] = {LED1_L_GPIO_Port, LED1_R_GPIO_Port,
																		LED2_L_GPIO_Port, LED2_R_GPIO_Port};
static unsigned int DIR_LIGHT_PIN[4] = {LED1_L_Pin, LED1_R_Pin, LED2_L_Pin, LED2_R_Pin};
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED2_R_GPIO_Port, LED2_R_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED2_L_Pin|LORA_Wake_Pin|LED1_L_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_Run_Pin|LED_ERR_Pin|LED1_R_Pin|LORA_nReset_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LED2_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED2_R_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = LED2_L_Pin|LORA_Wake_Pin|LED1_L_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = LED_Run_Pin|LED_ERR_Pin|LED1_R_Pin|LORA_nReset_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
void NET_DEVICE_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();


  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);


  /*Configure GPIO pin : PC1 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);

}


void Dir_Light_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
	
	//使用PA15作为普通IO口，需要禁用JTAG功能
  __HAL_RCC_AFIO_CLK_ENABLE();
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED2_R_GPIO_Port, LED2_R_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED2_L_Pin|LED1_L_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED1_R_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LED2_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED2_R_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PAPin PAPin PAPin */
  GPIO_InitStruct.Pin = LED2_L_Pin|LED1_L_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = LED1_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

unsigned char Dir_Light_GPIO_Status_Get(void)
{
	unsigned char dir_light_status[4]={0,};
	unsigned char i=0;
	for(;i<4;++i)
	 dir_light_status[i] = HAL_GPIO_ReadPin((GPIO_TypeDef *)DIR_LIGHT_PORT[i], DIR_LIGHT_PIN[i]);
	
	if((dir_light_status[0] == 0) &&(dir_light_status[1] == 0))
		return 0;
	else if((dir_light_status[0] == 1) &&(dir_light_status[1] == 0))
		return 1;
	else if((dir_light_status[0] == 0) &&(dir_light_status[1] == 1))
		return 2;
	else if((dir_light_status[0] == 1) &&(dir_light_status[1] == 1))
		return 3;
	else
		return 4;
}


/**
  * @brief  control the direction lights status.
  * @param  dir: the direction which will be lighten
  *            @arg both
  *            @arg left
  *            @arg right
  * @retval BSP status
  */
char Dir_Light_GPIO_Control(unsigned char dir)
{
  switch(dir){
		case 0:
		{
			HAL_GPIO_WritePin(LED1_L_GPIO_Port,LED1_L_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED1_R_GPIO_Port,LED1_R_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED2_L_GPIO_Port,LED2_L_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED2_R_GPIO_Port,LED2_R_Pin,GPIO_PIN_RESET);
		}
		break;
		case 1:
		{
			HAL_GPIO_WritePin(LED1_L_GPIO_Port,LED1_L_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED1_R_GPIO_Port,LED1_R_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED2_L_GPIO_Port,LED2_L_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED2_R_GPIO_Port,LED2_R_Pin,GPIO_PIN_RESET);
		}
		break;
		case 2:
		{
			HAL_GPIO_WritePin(LED1_L_GPIO_Port,LED1_L_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED1_R_GPIO_Port,LED1_R_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED2_L_GPIO_Port,LED2_L_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED2_R_GPIO_Port,LED2_R_Pin,GPIO_PIN_SET);
		}
		break;
	  case 3:
		{
			HAL_GPIO_WritePin(LED1_L_GPIO_Port,LED1_L_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED1_R_GPIO_Port,LED1_R_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED2_L_GPIO_Port,LED2_L_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED2_R_GPIO_Port,LED2_R_Pin,GPIO_PIN_SET);
		}
		break;
		default:
			return 0;
	}
  return 1;
}

void IIC1_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();



  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PD11 PD12 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void PMSensor_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);


  /*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
}


void MultiPlexer_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();


  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);


  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
}

/*!
    \brief      configure USART CHANNEL Select
		\param[in]  ch :channel to select [0~3]
    \retval     none
*/
void MultiPlexer_CH_Sel(unsigned char ch)
{
	switch(ch){
		case 0:
			HAL_GPIO_WritePin(MultiPlexer_SW0_GPIO_Port,MultiPlexer_SW0_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(MultiPlexer_SW1_GPIO_Port,MultiPlexer_SW1_Pin,GPIO_PIN_RESET);
			break;
		case 1:
			HAL_GPIO_WritePin(MultiPlexer_SW0_GPIO_Port,MultiPlexer_SW0_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(MultiPlexer_SW1_GPIO_Port,MultiPlexer_SW1_Pin,GPIO_PIN_RESET);
			break;
		case 2:
			HAL_GPIO_WritePin(MultiPlexer_SW0_GPIO_Port,MultiPlexer_SW0_Pin,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(MultiPlexer_SW1_GPIO_Port,MultiPlexer_SW1_Pin,GPIO_PIN_SET);
			break;
		case 3:
			HAL_GPIO_WritePin(MultiPlexer_SW0_GPIO_Port,MultiPlexer_SW0_Pin,GPIO_PIN_SET);
			HAL_GPIO_WritePin(MultiPlexer_SW1_GPIO_Port,MultiPlexer_SW1_Pin,GPIO_PIN_SET);
			break;
		default:
			break;
	}
}

void Charge_Control_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  
  __HAL_RCC_GPIOA_CLK_ENABLE();


  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Charge_Control_GPIO_Port, Charge_Control_Pin, GPIO_PIN_RESET);


  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = Charge_Control_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(Charge_Control_GPIO_Port, &GPIO_InitStruct);

	
	  /*Configure GPIO pin Output Level */
 // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
}

/*!
    \brief      configure USART CHANNEL Select
		\param[in]  ch :channel to select [0~3]
    \retval     none
*/
void Charge_Control(unsigned char new_state)
{
	if(new_state == 0)
		HAL_GPIO_WritePin(Charge_Control_GPIO_Port,Charge_Control_Pin,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(Charge_Control_GPIO_Port,Charge_Control_Pin,GPIO_PIN_RESET);
}

/* USER CODE END 2 */

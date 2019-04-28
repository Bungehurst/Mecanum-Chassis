/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#define PC_TX_Pin GPIO_PIN_2
#define PC_TX_GPIO_Port GPIOA
#define PC_RX_Pin GPIO_PIN_3
#define PC_RX_GPIO_Port GPIOA
#define WHEEL_3_PWM_Pin GPIO_PIN_0
#define WHEEL_3_PWM_GPIO_Port GPIOB
#define WHEEL_4_PWM_Pin GPIO_PIN_1
#define WHEEL_4_PWM_GPIO_Port GPIOB
#define WIRELESS_TX_Pin GPIO_PIN_10
#define WIRELESS_TX_GPIO_Port GPIOB
#define WIRELESS_RX_Pin GPIO_PIN_11
#define WIRELESS_RX_GPIO_Port GPIOB
#define WHEEL_4P_Pin GPIO_PIN_12
#define WHEEL_4P_GPIO_Port GPIOB
#define WHEEL_3P_Pin GPIO_PIN_13
#define WHEEL_3P_GPIO_Port GPIOB
#define WHEEL_2P_Pin GPIO_PIN_14
#define WHEEL_2P_GPIO_Port GPIOB
#define WHEEL_1P_Pin GPIO_PIN_15
#define WHEEL_1P_GPIO_Port GPIOB
#define WHEEL_4_Pin GPIO_PIN_8
#define WHEEL_4_GPIO_Port GPIOA
#define WHEEL_3_Pin GPIO_PIN_9
#define WHEEL_3_GPIO_Port GPIOA
#define WHEEL_2_Pin GPIO_PIN_10
#define WHEEL_2_GPIO_Port GPIOA
#define WHEEL_1_Pin GPIO_PIN_11
#define WHEEL_1_GPIO_Port GPIOA
#define LED_COM_Pin GPIO_PIN_15
#define LED_COM_GPIO_Port GPIOA
#define LED_SYS_Pin GPIO_PIN_3
#define LED_SYS_GPIO_Port GPIOB
#define WHEEL_1_PWM_Pin GPIO_PIN_4
#define WHEEL_1_PWM_GPIO_Port GPIOB
#define WHEEL_2_PWM_Pin GPIO_PIN_5
#define WHEEL_2_PWM_GPIO_Port GPIOB
#define LED_WHEEL_4_Pin GPIO_PIN_6
#define LED_WHEEL_4_GPIO_Port GPIOB
#define LED_WHEEL_3_Pin GPIO_PIN_7
#define LED_WHEEL_3_GPIO_Port GPIOB
#define LED_WHEEL_2_Pin GPIO_PIN_8
#define LED_WHEEL_2_GPIO_Port GPIOB
#define LED_WHEEL_1_Pin GPIO_PIN_9
#define LED_WHEEL_1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

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
#include "stm32u5xx_hal.h"

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
#define MODEM_PWR_EN_Pin GPIO_PIN_4
#define MODEM_PWR_EN_GPIO_Port GPIOB
#define MODEM_RING_Pin GPIO_PIN_6
#define MODEM_RING_GPIO_Port GPIOE
#define MODEM_RING_EXTI_IRQn EXTI6_IRQn
#define MODEM_SIM_DATA_Pin GPIO_PIN_5
#define MODEM_SIM_DATA_GPIO_Port GPIOD
#define MODEM_SIM_SELECT_1_Pin GPIO_PIN_12
#define MODEM_SIM_SELECT_1_GPIO_Port GPIOC
#define MODEM_SIM_SELECT_0_Pin GPIO_PIN_2
#define MODEM_SIM_SELECT_0_GPIO_Port GPIOD
#define DEBUG_UART_RX_Pin GPIO_PIN_10
#define DEBUG_UART_RX_GPIO_Port GPIOA
#define MODEM_DTR_Pin GPIO_PIN_9
#define MODEM_DTR_GPIO_Port GPIOC
#define DEBUG_UART_TX_Pin GPIO_PIN_9
#define DEBUG_UART_TX_GPIO_Port GPIOA
#define MODEM_SIM_RST_Pin GPIO_PIN_1
#define MODEM_SIM_RST_GPIO_Port GPIOG
#define MODEM_SIM_CLK_Pin GPIO_PIN_0
#define MODEM_SIM_CLK_GPIO_Port GPIOG
#define MODEM_UART_RTS_Pin GPIO_PIN_12
#define MODEM_UART_RTS_GPIO_Port GPIOD
#define MODEM_UART_TX_Pin GPIO_PIN_8
#define MODEM_UART_TX_GPIO_Port GPIOD
#define MODEM_UART_RX_Pin GPIO_PIN_9
#define MODEM_UART_RX_GPIO_Port GPIOD
#define MODEM_UART_CTS_Pin GPIO_PIN_11
#define MODEM_UART_CTS_GPIO_Port GPIOD
#define MODEM_RST_Pin GPIO_PIN_13
#define MODEM_RST_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

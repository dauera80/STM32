/**
  ******************************************************************************
  * @file    plf_hw_config.h
  * @author  MCD Application Team
  * @brief   This file contains the hardware configuration of the platform
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2018-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PLF_HW_CONFIG_H
#define PLF_HW_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* MISRAC messages linked to HAL include are ignored */
/*cstat -MISRAC2012-* */
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
/*cstat +MISRAC2012-* */

#include "main.h"
#include "plf_modem_config.h"
#include "usart.h" /* for huartX */

/* Exported constants --------------------------------------------------------*/

/* Platform defines ----------------------------------------------------------*/

/* ST33 configuration */
#define ST33_SPI_HANDLE                 hspi3
#define ST33_SPI_INSTANCE               SPI3
#define ST33_SPI_BAUDRATEPRESCALER      SPI_BAUDRATEPRESCALER_8
#define ST33_SPI_MOSI_PIN               SPI3_MOSI_PIN
#define ST33_SPI_MISO_PIN               SPI3_MISO_PIN
#define ST33_SPI_MOSI_MISO_SCK_PORT     SPI3_PORT
#define ST33_SPI_CS_PIN                 ST33_CS_Pin
#define ST33_SPI_CS_PORT                ST33_CS_GPIO_Port
#define ST33_SPI_ALT_FUNCTION           SPI3_ALT_FUNCTION
#define ST33_SPI_SCK_PIN                SPI3_SCK_PIN

#define MODEM_UART_HANDLE       huart3
#define MODEM_UART_INSTANCE     USART3
#define MODEM_UART_AUTOBAUD     (0)
#define MODEM_UART_IRQN         USART3_IRQn

#define MODEM_UART_BAUDRATE     (CONFIG_MODEM_UART_BAUDRATE)
#define MODEM_UART_WORDLENGTH   UART_WORDLENGTH_8B
#define MODEM_UART_STOPBITS     UART_STOPBITS_1
#define MODEM_UART_PARITY       UART_PARITY_NONE
#define MODEM_UART_MODE         UART_MODE_TX_RX

#if (CONFIG_MODEM_UART_RTS_CTS == 1)
#define MODEM_UART_HWFLOWCTRL   UART_HWCONTROL_RTS_CTS
#else
#define MODEM_UART_HWFLOWCTRL   UART_HWCONTROL_NONE
#endif /* (CONFIG_MODEM_UART_RTS_CTS == 1) */

#define MODEM_TX_GPIO_PORT      ((GPIO_TypeDef *)MDM_UART_TX_GPIO_Port)
#define MODEM_TX_PIN            MDM_UART_TX_Pin
#define MODEM_RX_GPIO_PORT      ((GPIO_TypeDef *)MDM_UART_RX_GPIO_Port)
#define MODEM_RX_PIN            MDM_UART_RX_Pin
#define MODEM_CTS_GPIO_PORT     ((GPIO_TypeDef *)MDM_UART_CTS_GPIO_Port)
#define MODEM_CTS_PIN           MDM_UART_CTS_Pin
#define MODEM_RTS_GPIO_PORT     ((GPIO_TypeDef *)MDM_UART_RTS_GPIO_Port)
#define MODEM_RTS_PIN           MDM_UART_RTS_Pin

/* ---- MODEM other pins configuration ---- */
/* output */
#define MODEM_PWR_EN_GPIO_PORT          MDM_PWR_EN_OUT_GPIO_Port
#define MODEM_PWR_EN_PIN                MDM_PWR_EN_OUT_Pin
#define MODEM_DTR_GPIO_PORT             MDM_DTR_OUT_GPIO_Port
#define MODEM_DTR_PIN                   MDM_DTR_OUT_Pin
/* input */
#define MODEM_RING_GPIO_PORT    ((GPIO_TypeDef *)MDM_RING_GPIO_Port)
#define MODEM_RING_PIN          MDM_RING_Pin
#define MODEM_RING_IRQN         MDM_RING_EXTI_IRQn

#define PPPOS_LINK_UART_HANDLE   NULL
#define PPPOS_LINK_UART_INSTANCE NULL

/* DEBUG INTERFACE CONFIGURATION */

/* use test board trace UART - DEFAULT */
#define TRACE_INTERFACE_UART_HANDLE     huart1
#define TRACE_INTERFACE_INSTANCE        USART1

#define COM_INTERFACE_UART_HANDLE      huart2
#define COM_INTERFACE_INSTANCE        ((USART_TypeDef *)USART2)
#define COM_INTERFACE_UART_IRQ         USART2_IRQn
#define COM_INTERFACE_UART_INIT        MX_USART2_UART_Init(); \
  HAL_NVIC_EnableIRQ(COM_INTERFACE_UART_IRQ);

#define CS_DISP_PIN GPIO_PIN_2
#define CS_DISP_GPIO_PORT GPIOB

/* Exported types ------------------------------------------------------------*/

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


#ifdef __cplusplus
}
#endif

#endif /* PLF_HW_CONFIG_H */

/**
  ******************************************************************************
  * @file    spi_ndlc.h
  * @author  MCD Application Team
  * @brief   Header for spi_ndlc.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SPI_NDLC_H
#define SPI_NDLC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"

#if ((USE_ST33 == 1) && (NDLC_INTERFACE == SPI_INTERFACE))

#include <stdint.h>
#include <stdbool.h>

#include "ndlc_config.h"

/* Exported constants --------------------------------------------------------*/
#define SPI_NDLC_OK                      0
#define SPI_NDLC_COMMUNICATION_ERROR    -1
#define SPI_NDLC_COMMUNICATION_TIMEOUT  -2
#define SPI_NDLC_INIT_ERROR             -3

/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* ToDo: Normal correction : use rsp buffer provided as parameter by Applications */
extern uint8_t ST33Data[260];

/* Exported macros -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
/**
  * @brief  SPI NDLC init
  * @param  hspi       - SPI handler
  * @param  p_handler  - handler to update
  * @retval false/true - init nok/ok
  */
bool spi_ndlc_init(void *hspi, ndlc_device_t *p_handler);

/**
  * @brief  SPI NDLC de-init
  * @param  p_handler  - handler to update
  * @retval false/true - de-init nok/ok
  */
bool spi_ndlc_deinit(ndlc_device_t *p_handler);

/**
  * @brief  SPI NDLC power off/on
  * @param  p_handler  - handler to update
  * @param  state_on   - false: request power off / true: request power on
  * @retval false/true - power off/on nok/ok
  */
bool spi_ndlc_power(ndlc_device_t *p_handler, bool state_on);

/**
  * @brief  Send a ATR
  * @param  p_handler - handler to use
  * @retval int32_t   - -1/0 - send ATR nok/ok
  */
int32_t spi_ndlc_atr(ndlc_device_t *p_handler);

/**
  * @brief  Send a command and receive the response
  * @param  p_handler     - handler to use
  * @param  buffer_tx     - TX buffer
  * @param  buffer_tx_len - TX buffer length
  * @param  buffer_rx     - RX buffer
  * @retval int32_t       - -1/>0 - tranceive nok/response length
  */
int32_t spi_ndlc_transceive_apdu(ndlc_device_t *p_handler, uint8_t *buffer_tx, uint16_t buffer_tx_len,
                                 uint8_t *buffer_rx);

/**
  * @brief  Send a abort
  * @param  p_handler - handler to use
  * @retval int32_t   - -1/0 - send abort nok/ok
  */
int32_t spi_ndlc_abort_apdu(ndlc_device_t *p_handler);

/**
  * @brief  Send a abort
  * @param  p_handler - handler to use
  * @retval int32_t   - -1/>0 - send/receive nok/ok
  */
int32_t spi_ndlc_send_receive_phy(void *p_handler, uint16_t length, uint8_t *tx_data, uint8_t *rx_data);

#endif /* #if (USE_ST33 == 1) && (NDLC_INTERFACE == SPI_INTERFACE) */

#ifdef __cplusplus
}
#endif

#endif /* SPI_NDLC_H */


/**
  ******************************************************************************
  * @file    ndlc_interface.h
  * @author  MCD Application Team
  * @brief   Ndlc interfcae definition
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
#ifndef NDLC_INTERFACE_H
#define NDLC_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ndlc_config.h"

#if (USE_ST33 == 1)
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
/* bool ndlc_init(void *p_handler_predefined, ndlc_device_t *p_handler); */

/* bool ndlc_deinit(ndlc_device_t *p_handler); */

/* bool ndlc_power(ndlc_device_t *p_handler, bool state_on); */

/* int32_t ndlc_atr(ndlc_device_t *p_handler); */

/* int32_t ndlc_apdu_snd_rcv(ndlc_device_t *p_handler, uint8_t *p_buf_tx, uint16_t buffer_tx_len, uint8_t *p_buf_rx); */

#endif /* USE_ST33 == 1 */

#ifdef __cplusplus
}
#endif

#endif /* NDLC_INTERFACE_H */

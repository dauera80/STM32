/**
  ******************************************************************************
  * @file    ndlc_commands.h
  * @author  MCD Application Team
  * @brief   Header for ndlc_commands.c module
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
#ifndef NDLC_COMMANDS_H
#define NDLC_COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ndlc_config.h"

#include <stdint.h>

#if (USE_ST33 == 1)

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

void NDLC_Initialization(ndlc_device_t *p_dev);
void NDLC_DeInitialization(ndlc_device_t *p_dev);

/**
  * @brief  Abort Command
  * @param  dev - pointer of the device
  * @param  outdata - pointer of the response
  * @retval int32_t - length of the response or error value
  */
int32_t NDLC_abort(ndlc_device_t *p_dev, uint8_t *outdata);

/**
  * @brief  APDU Command
  * @param  dev  - pointer of the device
  * @param  apdu - pointer of the apdu
  * @param  apdu_len - apdu length
  * @param  outdata  - pointer of the response
  * @retval int32_t  - length of the response or error value
  */
int32_t NDLC_apdu(ndlc_device_t *p_dev, uint8_t *apdu, uint16_t apdu_len, uint8_t *outdata);

#endif /* USE_ST33 == 1 */


#ifdef __cplusplus
}
#endif

#endif /* NDLC_COMMANDS_H */

/**
  ******************************************************************************
  * @file    ndlc.h
  * @author  MCD Application Team
  * @brief   Header for ndlc.c module
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
#ifndef NDLC_H
#define NDLC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ndlc_config.h"

#if (USE_ST33 == 1)
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
#define NDLC_MAJ_VER  0x00
#define NDLC_MIN_VER  0x04
#define NDLC_PATCH_VER  0x00

/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Send and Receive through NDLC
  * @param  dev  - pointer of the device
  * @param  len - outdata length
  * @param  outdata  - pointer of the outdata
  * @param  indata   - pointer of the indata
  * @retval int32_t  - length of the response or error value
  */
int32_t ndlc_send_receive(ndlc_device_t *p_dev, uint16_t len, uint8_t *outdata, uint8_t *indata);

#endif /* USE_ST33 == 1 */

#ifdef __cplusplus
}
#endif

#endif /* NDLC_H */

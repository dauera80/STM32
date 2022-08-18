/**
  ******************************************************************************
  * @file    spi_ndlc_device.h
  * @author  MCD Application Team
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SPI_NDLC_DEVICE_H
#define SPI_NDLC_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"

#if (USE_ST33 == 1)

#include "spi.h"

/* Exported constants --------------------------------------------------------*/
#ifndef NDLC_DEBUG
#define NDLC_DEBUG  1
#endif /* NDLC_DEBUG */

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  SPI_HandleTypeDef *hspi;
  GPIO_TypeDef  *cs_port;
  uint16_t      *cs_pin;
  void *power;
} STSpiDevHnd_t;

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* USE_ST33 == 1 */

#ifdef __cplusplus
}
#endif

#endif /* SPI_NDLC_DEVICE_H */


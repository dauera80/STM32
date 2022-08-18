/**
  ******************************************************************************
  * @file    ndlc_config.h
  * @author  MCD Application Team
  * @brief   Ndlc Configuration definition
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
#ifndef NDLC_CONFIG_H
#define NDLC_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"

#if (USE_ST33 == 1)

/* Exported constants --------------------------------------------------------*/
typedef enum
{
  NDLC_IDLE = 0,
  NDLC_SENDCMD,
  NDLC_READACK,
  NDLC_READHEADER,
  NLDC_READDATA,
  NLDC_SENDACK
} ndlc_status;

/* Exported types ------------------------------------------------------------*/
typedef struct
{
  void *handlePtr;  /* Handle to a physical link */
  ndlc_status status;
  uint16_t dataLen;
  uint8_t *data;
} ndlc_device_t;

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
#if (NDLC_INTERFACE == SPI_INTERFACE)

#include "spi_ndlc.h"
#include "spi_ndlc_device.h"

#define NDLC_INIT              spi_ndlc_init
#define NDLC_DEINIT            spi_ndlc_deinit
#define NDLC_POWER             spi_ndlc_power
#define NDLC_ATR               spi_ndlc_atr
#define NDLC_SEND_RECEIVE_APDU spi_ndlc_transceive_apdu

#define NDLC_SEND_RECEIVE_PHY  spi_ndlc_send_receive_phy

#else

#define NDLC_INIT
#define NDLC_DEINIT
#define NDLC_POWER
#define NDLC_ATR
#define NDLC_SEND_RECEIVE_APDU

#define NDLC_SEND_RECEIVE_PHY

#endif /* NDLC_INTERFACE == SPI_INTERFACE */

#endif /* USE_ST33 == 1 */

#ifdef __cplusplus
}
#endif

#endif /* NDLC_INTERFACE_H */

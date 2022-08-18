/**
  ******************************************************************************
  * @file    sys_spi.h
  * @author  MCD Application Team
  * @brief   Header for sys_spi.c module
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
#ifndef SYS_SPI_H
#define SYS_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"

#if ((USE_DISPLAY == 1) || (USE_ST33 == 1))

#include <stdbool.h>

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef uint8_t sys_spi_configuration_t;

#define SYS_SPI_NO_CONFIGURATION      (sys_spi_configuration_t)0U /* init value */
#define SYS_SPI_DISPLAY_CONFIGURATION (sys_spi_configuration_t)1U
#define SYS_SPI_ST33_CONFIGURATION    (sys_spi_configuration_t)2U

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Power ON the SPI to access to Display or ST33
  * @param  -
  * @retval bool - true/false SPI Power ON OK/NOK
  */
bool sys_spi_power_on(void);

/**
  * @brief  Power OFF the SPI to improve power consumption
  * @param  -
  * @retval bool - true/false SPI Power OFF OK/NOK
  */
bool sys_spi_power_off(void);

/**
  * @brief  SPI acquire
  * @param  sys_spi_conf - requested configuration
  * @retval -
  */
void sys_spi_acquire(sys_spi_configuration_t sys_spi_conf);

/**
  * @brief  SPI release
  * @param  sys_spi_conf - requested configuration
  * @retval -
  */
void sys_spi_release(sys_spi_configuration_t sys_spi_conf);

/**
  * @brief  SPI Initialization
  * @param  -
  * @retval bool - true/false SPI initialization OK/NOK
  */
bool sys_spi_init(void);

#endif /* (USE_DISPLAY == 1) || (USE_ST33 == 1) */

#ifdef __cplusplus
}
#endif

#endif /* SYS_SPI_H */


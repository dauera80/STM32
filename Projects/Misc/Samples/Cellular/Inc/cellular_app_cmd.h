/**
  ******************************************************************************
  * @file    cellular_app_cmd.h
  * @author  MCD Application Team
  * @brief   Header for cellular_app_cmd.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef CELLULAR_APP_CMD_H
#define CELLULAR_APP_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"

#if (USE_CELLULAR_APP == 1)
#if (USE_CMD_CONSOLE == 1)

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Initialization CellularApp command management
  * @param  -
  * @retval -
  */
void cellular_app_cmd_init(void);

/**
  * @brief  Start CellularApp command management
  * @note   - Registration to CMD module
  * @param  -
  * @retval -
  */
void cellular_app_cmd_start(void);

#endif /* USE_CMD_CONSOLE == 1 */
#endif /* USE_CELLULAR_APP == 1 */

#ifdef __cplusplus
}
#endif

#endif /* CELLULAR_APP_CMD_H */

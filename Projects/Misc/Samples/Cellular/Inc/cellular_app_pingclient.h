/**
  ******************************************************************************
  * @file    cellular_app_pingclient.h
  * @author  MCD Application Team
  * @brief   Header for cellular_app_pingclient.c module
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
#ifndef CELLULAR_APP_PINGCLIENT_H
#define CELLULAR_APP_PINGCLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"

#if (USE_CELLULAR_APP == 1)

#include "cellular_app_socket.h"

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Send a message to PingClt
  * @param  queue_msg - Message to send
  * @retval bool      - false/true - Message not send / Message send
  */
bool cellular_app_pingclient_send_msg(uint32_t queue_msg);

/**
  * @brief  Get status of PingClt application
  * @retval cellular_app_process_status_t - PingClt application process status
  */
cellular_app_process_status_t cellular_app_pingclient_get_status(void);

/**
  * @brief  Set status of PingClt application
  * @param  process_status    - process status new value to set inactive/active
  * @retval bool - false/true - P not updated / application update in progress
  */
bool cellular_app_pingclient_set_status(bool process_status);

/**
  * @brief  Change distant of PingClt application
  * @param  index             - PPingClt index to change (must be 0U)
  * @param  distant_type      - distant type value
  * @param  p_distantip       - distant ip value  (supported for PingClt only)
  * @param  distantip_len     - distant ip length (supported for PingClt only)
  * @retval bool - false/true - application not updated / application update in progress
  */
bool cellular_app_pingclient_distant_change(uint8_t index, cellular_app_distant_type_t distant_type,
                                            uint8_t *p_distantip, uint32_t distantip_len);

/**
  * @brief  Display PingClt status
  * @param  -
  * @retval -
  */
void cellular_app_pingclient_display_status(void);

/**
  * @brief  Initialize all needed structures to support PingClt feature
  * @param  -
  * @retval -
  */
void cellular_app_pingclient_init(void);

/**
  * @brief  Start PingClt thread
  * @param  -
  * @retval -
  */
void cellular_app_pingclient_start(void);

#endif /* USE_CELLULAR_APP == 1 */

#ifdef __cplusplus
}
#endif

#endif /* CELLULAR_APP_PINGCLIENT_H */

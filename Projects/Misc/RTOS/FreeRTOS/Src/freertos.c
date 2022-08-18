/**
  ******************************************************************************
  * @file    freertos.c
  * @author  MCD Application Team
  * @brief   Default task : System init
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


/* Includes ------------------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Global variables ----------------------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

extern void application_init(void);
extern void application_start(void);

/* Private function Definition -----------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/

/* Init FreeRTOS */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */

  /* Application components statical init  */
  application_init();

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* definition and creation of DebounceTimer */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_THREADS */

  /* Application components start */
  application_start();

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}


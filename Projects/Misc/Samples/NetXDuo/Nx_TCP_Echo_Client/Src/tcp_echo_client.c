/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tcp_echo_client.c
  * @author  MCD Application Team
  * @brief   TCP echoclient NetXDuo applicative file
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
/*cstat -MISRAC2012-* */
#include "nx_api.h"
#include "nxd_dns.h"
/*cstat +MISRAC2012-* */

#ifndef TCP_ECHO_CLIENT_PARALLEL
#define TCP_ECHO_CLIENT_PARALLEL 2U
#endif /* TCP_ECHO_CLIENT_PARALLEL */

/* Interval to send packets. The default value is 300 ticks which is 3000ms.  */
#ifndef TCP_ECHO_CLIENT_SLEEP_INTERVAL
#define TCP_ECHO_CLIENT_SLEEP_INTERVAL   (ULONG)300
#endif /* NX_DRIVER_THREAD_INTERVAL */

#ifndef DEFAULT_PRIORITY
#define DEFAULT_PRIORITY 4
#endif /* DEFAULT_PRIORITY */

#ifndef DEFAULT_MEMORY_SIZE
#define DEFAULT_MEMORY_SIZE 1024
#endif /* DEFAULT_MEMORY_SIZE */

#ifndef WINDOW_SIZE
#define WINDOW_SIZE 512
#endif /* WINDOW_SIZE */

#ifndef DEFAULT_TIMEOUT
#define DEFAULT_TIMEOUT (20 * NX_IP_PERIODIC_RATE)
#endif /* DEFAULT_TIMEOUT */

#ifndef MAX_PACKET_COUNT
#define MAX_PACKET_COUNT 20UL
#endif /* MAX_PACKET_COUNT */

#ifndef DEFAULT_MESSAGE
#define DEFAULT_MESSAGE "TCP Client From ST XCC: "
#endif /* DEFAULT_MESSAGE */

#ifndef TCP_SERVER_PORT
#define TCP_SERVER_PORT 7
#endif /* TCP_SERVER_PORT */

#ifndef TCP_SERVER_ADDRESS
#define TCP_SERVER_ADDRESS IP_ADDRESS(52,215,34,155)
#endif /* TCP_SERVER_ADDRESS */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

static NX_IP          *p_IpInstance;
static NX_PACKET_POOL *p_AppPool;

static UBOOL          tcp_echoclient_data_is_ready; /* false/0: data is down, true/1: data is up */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
typedef enum
{
  TCP_SOCKET_INVALID = 0,
  TCP_SOCKET_CREATED,
  TCP_SOCKET_BIND,
  TCP_SOCKET_CONNECTED,
  TCP_SOCKET_CLOSING
} tcp_socket_state_t;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* Declare the sample interface.  */
extern VOID sample_init(void);
extern VOID sample_start(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr,
                         UINT(*unix_time_callback)(ULONG *unix_time));

static void Error_Handler(void);
static VOID App_TCP_Thread_Entry(ULONG thread_input);
static void tcp_echoclient_link_status_change_notify_cb(NX_IP *ip_ptr, UINT interface_index, UINT link_up);

static void tcp_echoclient_link_status_change_notify_cb(NX_IP *ip_ptr, UINT interface_index, UINT link_up)
{
  if ((ip_ptr == p_IpInstance) && (interface_index == 0U))
  {
    bool data_is_ready = (link_up == 1U) ? true : false;
    if (tcp_echoclient_data_is_ready != data_is_ready)
    {
      tcp_echoclient_data_is_ready = data_is_ready;
      if (tcp_echoclient_data_is_ready == true)
      {
        (void)printf("Data is UP\n\r");
      }
      else
      {
        (void)printf("Data is DOWN !!!\n\r");
      }
    }
  }
}
/* USER CODE BEGIN 1 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval none
  */
static void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  (void)printf("\nERROR\n");
  for (;;)
  {
    (void)tx_thread_sleep(20);
  }
  /* USER CODE END Error_Handler_Debug */
}

/**
  * @brief  TCP thread entry.
  * @param  thread_input: thread user data
  * @retval none
  */

static void App_TCP_Thread_Entry(ULONG thread_input)
{
  static NX_TCP_SOCKET TCPSocket[TCP_ECHO_CLIENT_PARALLEL];

  UINT status;
  ULONG count   = 1U; /* ULONG: to allow long duration test */
  ULONG success = 0U; /* ULONG: to allow long duration test */

  tcp_socket_state_t socket_state = TCP_SOCKET_INVALID;

  ULONG bytes_read;
  ULONG link_status;
  UCHAR data_buffer[512];

  CHAR str[2];

  NX_PACKET *server_packet;
  NX_PACKET *data_packet;

  str[0] = '0' + (thread_input % 10U);
  str[1] = '\n';

  /* In case of multi-instance, to avoid to have same thread running at same time add a delay */
  (void)printf("App TCP[%ld] will sleep %lu ticks before to start\n\r", thread_input, 200UL * thread_input);
  (void)tx_thread_sleep(200UL * thread_input);

  while (count <= MAX_PACKET_COUNT)
  {
    /* Wait Link/Data is enabled before to do/send data on network. */
    status = nx_ip_interface_status_check(p_IpInstance, 0U, NX_IP_LINK_ENABLED, &link_status,
                                          TX_WAIT_FOREVER); /* NX_WAIT_FOREVER); => MISRAC2012-Rule-7.2 */
    if (status == (UINT)NX_SUCCESS)
    {
      if (socket_state == TCP_SOCKET_INVALID)
      {
        /* Create/Bind/Connect TCP socket. */
        status = nx_tcp_socket_create(p_IpInstance, &TCPSocket[thread_input], (CHAR *)"TCP Client Socket",
                                      NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, WINDOW_SIZE,
                                      NULL, NULL);
        if (status == (UINT)NX_SUCCESS)
        {
          socket_state = TCP_SOCKET_CREATED;
        }
        else
        {
          (void)printf("App TCP[%ld] socket create fail - status: %u !!!\n\r", thread_input, status);
        }
      }

      if (socket_state == TCP_SOCKET_CREATED)
      {
        /* Bind the client socket for the NX_ANY_PORT. */
        status = nx_tcp_client_socket_bind(&TCPSocket[thread_input], (UINT)NX_ANY_PORT,
                                           TX_WAIT_FOREVER); /* NX_WAIT_FOREVER); => MISRAC2012-Rule-7.2 */
        if (status == (UINT)NX_SUCCESS)
        {
          socket_state = TCP_SOCKET_BIND;
        }
        else
        {
          (void)printf("App TCP[%ld] socket bind fail - status: %u !!!\n\r", thread_input, status);
        }
      }

      if (socket_state == TCP_SOCKET_BIND)
      {
        /* Connect to the remote server on the specified port. */
        status = nx_tcp_client_socket_connect(&TCPSocket[thread_input],
                                              (ULONG)TCP_SERVER_ADDRESS, (UINT)TCP_SERVER_PORT,
                                              TX_WAIT_FOREVER); /* NX_WAIT_FOREVER); => MISRAC2012-Rule-7.2 */
        if (status == (UINT)NX_SUCCESS)
        {
          socket_state = TCP_SOCKET_CONNECTED;
        }
        else
        {
          (void)printf("App TCP[%ld] socket connect fail - status: %u !!!\n\r", thread_input, status);
        }
      }

      /* Packet Creation/Send/Receive process */
      if (socket_state == TCP_SOCKET_CONNECTED)
      {
        /* Check status.  */
        (void)TX_MEMSET(data_buffer, (INT)'\0', sizeof(data_buffer));

        /* Allocate the packet to send over the TCP socket. */
        status = nx_packet_allocate(p_AppPool, &data_packet, (ULONG)NX_TCP_PACKET,
                                    TX_WAIT_FOREVER); /* NX_WAIT_FOREVER); => MISRAC2012-Rule-7.2 */
        if (status == (UINT) NX_SUCCESS)
        {
          /* Append the message to send into the packet. */
          status = nx_packet_data_append(data_packet, (VOID *)DEFAULT_MESSAGE, (sizeof(DEFAULT_MESSAGE) - 1U),
                                         p_AppPool, TX_WAIT_FOREVER); /* NX_WAIT_FOREVER); => MISRAC2012-Rule-7.2 */
          status += nx_packet_data_append(data_packet, str, sizeof(str), p_AppPool,
                                          TX_WAIT_FOREVER); /* NX_WAIT_FOREVER); => MISRAC2012-Rule-7.2 */
          if (status != (UINT)NX_SUCCESS)
          {
            (void)nx_packet_release(data_packet);
            (void)printf("App TCP[%ld] msg[%lu] append fail - status: %u !!!\n\r", thread_input, count, status);
          }
        }
        else
        {
          (void)printf("App TCP[%ld] msg[%lu] allocate fail - status: %u !!!\n\r", thread_input, count, status);
        }

        /* Send/Receive process */
        if (status == (UINT)NX_SUCCESS)
        {
          /* Send the packet over the TCP socket. */
          status = nx_tcp_socket_send(&TCPSocket[thread_input], data_packet, DEFAULT_TIMEOUT);
          if (status == (UINT)NX_SUCCESS)
          {
            /* Wait for the server response. */
            status = nx_tcp_socket_receive(&TCPSocket[thread_input], &server_packet, DEFAULT_TIMEOUT);
            if (status == (UINT)NX_SUCCESS)
            {
              /* Retrieve the data sent by the server */
              status = nx_packet_data_retrieve(server_packet, data_buffer, &bytes_read);
              if (status == (UINT)NX_SUCCESS)
              {
                /* Print the received data */
                (void)printf("\n\rApp TCP[%ld] msg[%lu] reception OK - Server:%lu.%lu.%lu.%lu:%u [MSG]: %.*s",
                             thread_input, count,
                             ((TCP_SERVER_ADDRESS & 0xFF000000U) >> 24), ((TCP_SERVER_ADDRESS & 0x00FF0000U) >> 16),
                             ((TCP_SERVER_ADDRESS & 0x0000FF00U) >> 8), ((TCP_SERVER_ADDRESS & 0x000000FFU)),
                             TCP_SERVER_PORT, (INT)bytes_read, data_buffer);

                /* Release the server packet */
                (void)nx_packet_release(server_packet);
                success++;
              }
              else
              {
                /* Message not retrieved */
                (void)printf("App TCP[%ld] msg[%lu] retrieve fail - status: %u !!!\n\r", thread_input, count, status);
              }
            }
            else
            {
              /* No message received */
              (void)printf("App TCP[%ld] msg[%lu] reception fail - status: %u !!!\n\r", thread_input, count, status);
            }
          }
          else
          {
            (void)nx_packet_release(data_packet);
            (void)printf("App TCP[%ld] msg[%lu] send fail - status: %u !!!\n\r", thread_input, count, status);
          }
        }
      }

      /* If something goes wrong, close and delete the socket to restart from right state */
      if (status != (UINT)NX_SUCCESS)
      {
        (void)printf("App TCP[%ld] something goes wrong, close and delete the socket\n\r", thread_input);
        while (socket_state != TCP_SOCKET_INVALID)
        {
          switch (socket_state)
          {
            case TCP_SOCKET_CREATED :
              /* Delete the socket. */
              status = nx_tcp_socket_delete(&TCPSocket[thread_input]);
              if (status == (UINT)NX_SUCCESS)
              {
                (void)printf("App TCP[%ld] deleted\n\r", thread_input);
                socket_state = TCP_SOCKET_INVALID;
              }
              break;
            case TCP_SOCKET_BIND :
              /* Unbind the socket. */
              status = nx_tcp_client_socket_unbind(&TCPSocket[thread_input]);
              if (status == (UINT)NX_SUCCESS)
              {
                (void)printf("App TCP[%ld] unbind\n\r", thread_input);
                socket_state = TCP_SOCKET_CREATED;
              }
              break;
            case TCP_SOCKET_CONNECTED :
              /* Wait Link/Data is UP before to send any data on network. */
              status = nx_ip_interface_status_check(p_IpInstance, 0U, NX_IP_LINK_ENABLED, &link_status,
                                                    TX_WAIT_FOREVER); /* NX_WAIT_FOREVER); => MISRAC2012-Rule-7.2 */
              if (status == (UINT)NX_SUCCESS)
              {
                /* Disconnect the socket. */
                status = nx_tcp_socket_disconnect(&TCPSocket[thread_input], DEFAULT_TIMEOUT);
                if (status == (UINT)NX_SUCCESS)
                {
                  (void)printf("App TCP[%ld] disconnected\n\r", thread_input);
                  socket_state = TCP_SOCKET_BIND;
                }
              }
              break;
            default :
              /* Impossible case */
              (void)printf("App TCP[%ld] impossible case\n\r", thread_input);
              socket_state = TCP_SOCKET_INVALID;
              break;
          }
        }
      }

      /* Next occurrence */
      count++;
      (void)printf("App TCP[%ld] Wait %lu ticks before to send next packet.\n\r", thread_input,
                   TCP_ECHO_CLIENT_SLEEP_INTERVAL);
      /* Sleep for timer interval.  */
      (void)tx_thread_sleep(TCP_ECHO_CLIENT_SLEEP_INTERVAL);
    }
  }

  /* Print test summary on the UART. */
  if (success == MAX_PACKET_COUNT)
  {
    (void)printf("\n\r------------------ App TCP[%ld] Test OK  SUCCESS : %lu - FAIL : %lu ---------------------\n\r",
                 thread_input, success, (MAX_PACKET_COUNT - success));
  }
  else
  {
    (void)printf("\n\r------------------ App TCP[%ld] Test NOK SUCCESS : %lu - FAIL : %lu ---------------------\n\r",
                 thread_input, success, (MAX_PACKET_COUNT - success));
  }

  (void)printf("App TCP[%ld] close and delete the socket\n\r", thread_input);
  /* Clean-Up socket before to leave */
  while (socket_state != TCP_SOCKET_INVALID)
  {
    switch (socket_state)
    {
      case TCP_SOCKET_CREATED :
        /* Delete the socket. */
        status = nx_tcp_socket_delete(&TCPSocket[thread_input]);
        if (status == (UINT)NX_SUCCESS)
        {
          (void)printf("App TCP[%ld] deleted\n\r", thread_input);
          socket_state = TCP_SOCKET_INVALID;
        }
        break;
      case TCP_SOCKET_BIND :
        /* Unbind the socket. */
        status = nx_tcp_client_socket_unbind(&TCPSocket[thread_input]);
        if (status == (UINT)NX_SUCCESS)
        {
          (void)printf("App TCP[%ld] unbind\n\r", thread_input);
          socket_state = TCP_SOCKET_CREATED;
        }
        break;
      case TCP_SOCKET_CONNECTED :
        /* Wait Link/Data is UP before to send any data on network. */
        status = nx_ip_interface_status_check(p_IpInstance, 0U, NX_IP_LINK_ENABLED, &link_status,
                                              TX_WAIT_FOREVER); /* NX_WAIT_FOREVER); => MISRAC2012-Rule-7.2 */
        if (status == (UINT)NX_SUCCESS)
        {
          /* Disconnect the socket. */
          status = nx_tcp_socket_disconnect(&TCPSocket[thread_input], DEFAULT_TIMEOUT);
          if (status == (UINT)NX_SUCCESS)
          {
            (void)printf("App TCP[%ld] disconnected\n\r", thread_input);
            socket_state = TCP_SOCKET_BIND;
          }
        }
        break;

      default :
        /* Impossible case */
        (void)printf("App TCP[%ld] impossible case\n\r", thread_input);
        socket_state = TCP_SOCKET_INVALID;
        break;
    }
  }

  /* This thread is not needed any more, relinquish it */
  (void)printf("App TCP[%ld] job is finished - thread is relinquished\n\r", thread_input);
  tx_thread_relinquish();
}


/**
  * @brief  Initialize all needed structures to support EchoClt feature
  * @param  -
  * @retval none
  */
VOID sample_init(void)
{
  /* Initialize data ready status to false */
  tcp_echoclient_data_is_ready = false;
}

/**
  * @brief  Start all EchoClt threads
  * @brief  Main thread entry.
  * @param  ip_ptr: Pointer to NX_IP instance
  * @param  pool_ptr: Pointer to packet pool
  * @param  dns_ptr: Pointer to NX_DNS instance - UNUSED
  * @param  unix_time: Pointer to UTC callback function - UNUSED
  * @retval none
  */
VOID sample_start(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr,
                  UINT(*unix_time_callback)(ULONG *unix_time))
{
  static TX_THREAD AppTCPThread[TCP_ECHO_CLIENT_PARALLEL];
  static ULONG     AppTCPThreadStack[TCP_ECHO_CLIENT_PARALLEL][DEFAULT_MEMORY_SIZE];

  UINT status;

  NX_PARAMETER_NOT_USED(dns_ptr);
  NX_PARAMETER_NOT_USED(unix_time_callback);

  /* Check input parameters */
  if ((ip_ptr == NULL) || (pool_ptr == NULL))
  {
    Error_Handler();
  }

  p_IpInstance = ip_ptr;
  p_AppPool = pool_ptr;

  /* Register to IpLinkStatusChange notification */
  status = nx_ip_link_status_change_notify_set(p_IpInstance, tcp_echoclient_link_status_change_notify_cb);
  if (status != (UINT)NX_SUCCESS)
  {
    (void)printf("NxIpLinkStatusChangeNotifySet fail: %u\n\r", status);
    Error_Handler();
  }

  /* Create thread to run TCP echo client. */
  for (UINT i = 0U; i < TCP_ECHO_CLIENT_PARALLEL; i++)
  {
    status = tx_thread_create(&AppTCPThread[i], (CHAR *)"App TCP Thread", App_TCP_Thread_Entry, (ULONG)i,
                              AppTCPThreadStack[i], sizeof(AppTCPThreadStack[i]),
                              DEFAULT_PRIORITY, DEFAULT_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

    if (status != TX_SUCCESS)
    {
      (void)printf("App TCP[%d] Thread fail: %u !!!\n\r", i, status);
      Error_Handler();
    }
  }
}
/* USER CODE END 1 */

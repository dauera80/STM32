/**
  ******************************************************************************
  * @file    app_netxduo.c
  * @author  MCD Application Team
  * @brief   NetXDuo applicative file
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

/* Includes ------------------------------------------------------------------*/
#include "app_netxduo.h"
#include "nx_api.h"
#include "nxd_dns.h"
#include "nxd_sntp_client.h"

/* Include the sample interface.  */
extern VOID sample_init(void);
extern VOID sample_start(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr,
                         UINT(*unix_time_callback)(ULONG *unix_time));

/* Define the helper thread for running Azure SDK on ThreadX (THREADX IoT Platform).  */
#ifndef SAMPLE_HELPER_STACK_SIZE
#define SAMPLE_HELPER_STACK_SIZE        (4096)
#endif /* SAMPLE_HELPER_STACK_SIZE  */

#ifndef SAMPLE_HELPER_THREAD_PRIORITY
#define SAMPLE_HELPER_THREAD_PRIORITY   (4)
#endif /* SAMPLE_HELPER_THREAD_PRIORITY  */

/* Define user configurable symbols. */
#ifndef SAMPLE_IP_STACK_SIZE
#define SAMPLE_IP_STACK_SIZE            (2048)
#endif /* SAMPLE_IP_STACK_SIZE  */

#ifndef SAMPLE_PACKET_COUNT
#define SAMPLE_PACKET_COUNT             (32)
#endif /* SAMPLE_PACKET_COUNT  */

#ifndef SAMPLE_PACKET_SIZE
#define SAMPLE_PACKET_SIZE              (1536)
#endif /* SAMPLE_PACKET_SIZE  */

#define SAMPLE_POOL_SIZE                ((SAMPLE_PACKET_SIZE + sizeof(NX_PACKET)) * SAMPLE_PACKET_COUNT)

#ifndef SAMPLE_IP_THREAD_PRIORITY
#define SAMPLE_IP_THREAD_PRIORITY       (1)
#endif /* SAMPLE_IP_THREAD_PRIORITY */

#define SAMPLE_IPV4_ADDRESS             IP_ADDRESS(0, 0, 0, 0)
#define SAMPLE_IPV4_MASK                IP_ADDRESS(0, 0, 0, 0)

#ifndef SAMPLE_SNTP_SYNC_MAX
#define SAMPLE_SNTP_SYNC_MAX            30
#endif /* SAMPLE_SNTP_SYNC_MAX */

#ifndef SAMPLE_SNTP_UPDATE_MAX
#define SAMPLE_SNTP_UPDATE_MAX          10
#endif /* SAMPLE_SNTP_UPDATE_MAX */

#ifndef SAMPLE_SNTP_UPDATE_INTERVAL
#define SAMPLE_SNTP_UPDATE_INTERVAL     (NX_IP_PERIODIC_RATE / 2)
#endif /* SAMPLE_SNTP_UPDATE_INTERVAL */

/* Default time. GMT: Friday, Jan 1, 2022 12:00:00 AM. Epoch timestamp: 1640995200.  */
#ifndef SAMPLE_SYSTEM_TIME
#define SAMPLE_SYSTEM_TIME              1640995200
#endif /* SAMPLE_SYSTEM_TIME  */

/* Seconds between Unix Epoch (1/1/1970) and NTP Epoch (1/1/1999) */
#define SAMPLE_UNIX_TO_NTP_EPOCH_SECOND 0x83AA7E80

static TX_THREAD        sample_helper_thread;
static NX_PACKET_POOL   pool_0;
static NX_IP            ip_0;
static NX_DNS           dns_0;
static NX_SNTP_CLIENT   sntp_client;

/* System clock time for UTC.  */
static ULONG            unix_time_base;

/* Define the stack/cache for ThreadX.  */
static ULONG sample_ip_stack[SAMPLE_IP_STACK_SIZE / sizeof(ULONG)];
#ifndef SAMPLE_POOL_STACK_USER
static ULONG sample_pool_stack[SAMPLE_POOL_SIZE / sizeof(ULONG)];
static ULONG sample_pool_stack_size = sizeof(sample_pool_stack);
#else
extern ULONG sample_pool_stack[];
extern ULONG sample_pool_stack_size;
#endif /* ifndef SAMPLE_POOL_STACK_USER */
static ULONG sample_helper_thread_stack[SAMPLE_HELPER_STACK_SIZE / sizeof(ULONG)];

static const CHAR *sntp_servers[] =
{
  "0.pool.ntp.org",
  "1.pool.ntp.org",
  "2.pool.ntp.org",
  "3.pool.ntp.org",
};
static UINT sntp_server_index;

/* Define the prototypes for sample thread.  */
static void sample_helper_thread_entry(ULONG parameter);

#ifndef SAMPLE_DHCP_DISABLE
static UINT dhcp_wait();
#endif /* SAMPLE_DHCP_DISABLE */

static UINT dns_create(ULONG dns_server_address);

static UINT sntp_time_sync();
static UINT unix_time_get(ULONG *unix_time);

#ifndef SAMPLE_NETWORK_DRIVER
#define SAMPLE_NETWORK_DRIVER           _nx_ram_network_driver
#endif /* SAMPLE_NETWORK_DRIVER */

/* Include the platform IP driver. */
void SAMPLE_NETWORK_DRIVER(struct NX_IP_DRIVER_STRUCT *driver_req);

#ifdef SAMPLE_BOARD_SETUP
void SAMPLE_BOARD_SETUP();
#endif /* SAMPLE_BOARD_SETUP */

#ifdef SAMPLE_NETWORK_CONFIGURE
void SAMPLE_NETWORK_CONFIGURE(NX_IP *ip_ptr, ULONG *dns_address);
#endif /* SAMPLE_NETWORK_CONFIGURE */


/**
  * @brief  Application NetXDuo Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
#if 0
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *)memory_ptr;

  /* USER CODE BEGIN MX_NetXDuo_MEM_POOL */
  /* USER CODE END MX_NetXDuo_MEM_POOL */

  /* USER CODE BEGIN MX_NetXDuo_Init */
  printf("Nx_TCP_Echo_Client application started..\n");

  /* Allocate the memory for packet_pool.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pool_0,  NX_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the Packet pool to be used for packet allocation */
  ret = nx_packet_pool_create(&AppPool, "Main Packet Pool", PAYLOAD_SIZE, pool_0, NX_PACKET_POOL_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  /* Allocate the memory for Ip_Instance */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, (2 * DEFAULT_MEMORY_SIZE), TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main NX_IP instance */
  ret = nx_ip_create(&IpInstance, "Main Ip instance", NULL_ADDRESS, NULL_ADDRESS, &AppPool, nx_stm32_eth_driver,
                     pointer, (2 * DEFAULT_MEMORY_SIZE), DEFAULT_PRIORITY);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  /* Allocate the memory for ARP */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_MEMORY_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /*  Enable the ARP protocol and provide the ARP cache size for the IP instance */
  ret = nx_arp_enable(&IpInstance, (VOID *)pointer, DEFAULT_MEMORY_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  /* Enable the ICMP */
  ret = nx_icmp_enable(&IpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  /* Enable the UDP protocol required for  DHCP communication */
  ret = nx_udp_enable(&IpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  /* Enable the TCP protocol */
  ret = nx_tcp_enable(&IpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  /* Allocate the memory for main thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, (2 * DEFAULT_MEMORY_SIZE), TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main thread */
  ret = tx_thread_create(&AppMainThread, "App Main thread", App_Main_Thread_Entry, 0, pointer,
                         (2 * DEFAULT_MEMORY_SIZE), DEFAULT_PRIORITY, DEFAULT_PRIORITY, TX_NO_TIME_SLICE,
                         TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  /* Allocate the memory for TCP server thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, (2 * DEFAULT_MEMORY_SIZE), TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* create the TCP server thread */
  ret = tx_thread_create(&AppTCPThread, "App TCP Thread", App_TCP_Thread_Entry, 0, pointer, (2 * DEFAULT_MEMORY_SIZE),
                         DEFAULT_PRIORITY, DEFAULT_PRIORITY, TX_NO_TIME_SLICE, TX_DONT_START);

  if (ret != TX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  /* create the DHCP client */
  ret = nx_dhcp_create(&DHCPClient, &IpInstance, "DHCP Client");

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }

  /* set DHCP notification callback  */

  /* USER CODE END MX_NetXDuo_Init */

  return ret;
}
#endif /* to update */

UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT status;

  NX_PARAMETER_NOT_USED(memory_ptr);

  /* Initialize the NetX system. */
  nx_system_initialize();

  /* Create a packet pool. */
  status = nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", SAMPLE_PACKET_SIZE,
                                 (UCHAR *)sample_pool_stack, sample_pool_stack_size);

  /* Check for pool creation error.  */
  if (status == NX_SUCCESS)
  {
    /* Initialize sample */
    sample_init();

    /* Create an IP instance.  */
    status = nx_ip_create(&ip_0, "NetX IP Instance 0",
                          SAMPLE_IPV4_ADDRESS, SAMPLE_IPV4_MASK,
                          &pool_0, SAMPLE_NETWORK_DRIVER,
                          (UCHAR *)sample_ip_stack, sizeof(sample_ip_stack),
                          SAMPLE_IP_THREAD_PRIORITY);

    /* Check for IP create errors. */
    if (status == NX_SUCCESS)
    {
      /* Enable TCP traffic. */
      status = nx_tcp_enable(&ip_0);
    }
    else
    {
      printf("nx_ip_create fail: %u\r\n", status);
    }

    /* Check for TCP enable errors. */
    if (status == NX_SUCCESS)
    {
      /* Enable UDP traffic. */
      status = nx_udp_enable(&ip_0);
    }
    else
    {
      printf("nx_tcp_enable fail: %u\r\n", status);
    }

    /* Check for UDP enable errors. */
    if (status == NX_SUCCESS)
    {
      /* Create sample helper thread. */
      status = tx_thread_create(&sample_helper_thread, "Demo Thread",
                                sample_helper_thread_entry, 0,
                                sample_helper_thread_stack, SAMPLE_HELPER_STACK_SIZE,
                                SAMPLE_HELPER_THREAD_PRIORITY, SAMPLE_HELPER_THREAD_PRIORITY,
                                TX_NO_TIME_SLICE, TX_AUTO_START);

      /* Check status. */
      if (status != TX_SUCCESS)
      {
        printf("Demo helper thread creation fail: %u\r\n", status);
      }
    }
    else
    {
      printf("nx_udp_enable fail: %u\r\n", status);
    }
  }
  else
  {
    printf("nx_packet_pool_create fail: %u\r\n", status);
  }

  return (status);
}

/* Define sample helper thread entry. */
void sample_helper_thread_entry(ULONG parameter)
{
  UINT  status;
  UINT  unix_time;
  ULONG dns_server_address[3];

  NX_PARAMETER_NOT_USED(parameter);

  SAMPLE_NETWORK_CONFIGURE(&ip_0, &dns_server_address[0]);

  /* Create DNS. */
  status = dns_create(dns_server_address[0]);

  /* Check for DNS create errors. */
  if (status != NX_SUCCESS)
  {
    printf("dns_create fail: %u\r\n", status);
    return;
  }

  /* Sync up time by SNTP at start up. */
  status = sntp_time_sync();

  /* Check status. */
  if (status != NX_SUCCESS)
  {
    printf("SNTP Time Sync failed.\r\n");
    printf("Set Time to default value: SAMPLE_SYSTEM_TIME.");
    unix_time_base = SAMPLE_SYSTEM_TIME;
  }
  else
  {
    printf("SNTP Time Sync successfully.\r\n");
  }

  unix_time_get((ULONG *)&unix_time);
  srand(unix_time);

  /* Start sample. */
  sample_start(&ip_0, &pool_0, &dns_0, unix_time_get);

  /* This thread is not needed any more, relinquish it */
  (void)printf("Sample thread job is finished - thread is relinquished\n\r");
  tx_thread_relinquish();
}

static UINT dns_create(ULONG dns_server_address)
{
  UINT status;

  /* Create a DNS instance for the Client.  Note this function will create
     the DNS Client packet pool for creating DNS message packets intended
     for querying its DNS server.  */
  status = nx_dns_create(&dns_0, &ip_0, (UCHAR *)"DNS Client");

  /* Is the DNS client configured for the host application to create the packet pool?  */
#ifdef NX_DNS_CLIENT_USER_CREATE_PACKET_POOL
  if (status == NX_SUCCESS)
  {
    /* Yes, use the packet pool created above which has appropriate payload size
       for DNS messages.  */
    status = nx_dns_packet_pool_set(&dns_0, ip_0.nx_ip_default_packet_pool);
    if (status != NX_SUCCESS)
    {
      nx_dns_delete(&dns_0);
    }
  }
#endif /* NX_DNS_CLIENT_USER_CREATE_PACKET_POOL */

  if (status == NX_SUCCESS)
  {
    /* Add an IPv4 server address to the Client list.  */
    status = nx_dns_server_add(&dns_0, dns_server_address);
    if (status == NX_SUCCESS)
    {
      /* Output DNS Server address.  */
      printf("DNS Server address: %lu.%lu.%lu.%lu\r\n",
             (dns_server_address >> 24), (dns_server_address >> 16 & 0xFF), (dns_server_address >> 8 & 0xFF),
             (dns_server_address & 0xFF));
    }
    else
    {
      nx_dns_delete(&dns_0);
    }
  }

  return (status);
}

/* Sync up the local time. */
static UINT sntp_time_sync_internal(ULONG sntp_server_address)
{
  UINT status;
  UINT server_status;
  UINT i;

  /* Create the SNTP Client to run in broadcast mode.. */
  status = nx_sntp_client_create(&sntp_client, &ip_0, 0, &pool_0, NX_NULL, NX_NULL,
                                 NX_NULL /* no random_number_generator callback */);

  /* Check status. */
  if (status == NX_SUCCESS)
  {
    /* Use the IPv4 service to initialize the Client and set the IPv4 SNTP server. */
    status = nx_sntp_client_initialize_unicast(&sntp_client, sntp_server_address);

    /* Check status. */
    if (status != NX_SUCCESS)
    {
      nx_sntp_client_delete(&sntp_client);
      return (status);
    }

    /* Set local time to 0 */
    status = nx_sntp_client_set_local_time(&sntp_client, 0, 0);

    /* Check status. */
    if (status != NX_SUCCESS)
    {
      nx_sntp_client_delete(&sntp_client);
      return (status);
    }

    /* Run Unicast client */
    status = nx_sntp_client_run_unicast(&sntp_client);

    /* Check status. */
    if (status != NX_SUCCESS)
    {
      nx_sntp_client_stop(&sntp_client);
      nx_sntp_client_delete(&sntp_client);
      return (status);
    }

    /* Wait till updates are received */
    for (i = 0U; i < SAMPLE_SNTP_UPDATE_MAX; i++)
    {
      /* First verify we have a valid SNTP service running. */
      status = nx_sntp_client_receiving_updates(&sntp_client, &server_status);

      /* Check status. */
      if ((status == NX_SUCCESS) && (server_status == NX_TRUE))
      {
        /* Server status is good. Now get the Client local time. */
        ULONG sntp_seconds;
        ULONG sntp_fraction;
        ULONG system_time_in_second;

        /* Get the local time. */
        status = nx_sntp_client_get_local_time(&sntp_client, &sntp_seconds, &sntp_fraction, NX_NULL);

        /* Check status. */
        if (status != NX_SUCCESS)
        {
          continue;
        }

        /* Get the system time in second. */
        system_time_in_second = tx_time_get() / TX_TIMER_TICKS_PER_SECOND;

        /* Convert to Unix epoch and minus the current system time. */
        unix_time_base = (sntp_seconds - (system_time_in_second + SAMPLE_UNIX_TO_NTP_EPOCH_SECOND));

        /* Time sync successfully. */

        /* Stop and delete SNTP. */
        nx_sntp_client_stop(&sntp_client);
        nx_sntp_client_delete(&sntp_client);

        return (NX_SUCCESS);
      }

      /* Sleep.  */
      tx_thread_sleep(SAMPLE_SNTP_UPDATE_INTERVAL);
    }

    /* Time sync failed. - Return not success. */
    status = NX_NOT_SUCCESSFUL;
    /* Stop and delete SNTP. */
    nx_sntp_client_stop(&sntp_client);
    nx_sntp_client_delete(&sntp_client);
  }

  return (status);
}

static UINT sntp_time_sync(void)
{
  UINT  status;
  ULONG gateway_address;
  ULONG sntp_server_address[3];

  /* Sync time by SNTP server array. */
  for (UINT i = 0U; i < SAMPLE_SNTP_SYNC_MAX; i++)
  {
    printf("SNTP Time Sync...%s\r\n", sntp_servers[sntp_server_index]);

    /* Make sure the network is still valid. */
    while (nx_ip_gateway_address_get(&ip_0, &gateway_address))
    {
      tx_thread_sleep(NX_IP_PERIODIC_RATE);
    }

    /* Look up SNTP Server address. */
    status = nx_dns_host_by_name_get(&dns_0, (UCHAR *)sntp_servers[sntp_server_index], &sntp_server_address[0],
                                     (5 * NX_IP_PERIODIC_RATE));

    /* Check status. */
    if (status == NX_SUCCESS)
    {
      /* Start SNTP to sync the local time. */
      status = sntp_time_sync_internal(sntp_server_address[0]);

      /* Check status.  */
      if (status == NX_SUCCESS)
      {
        return (NX_SUCCESS);
      }
    }

    /* Switch SNTP server every time.  */
    sntp_server_index = (sntp_server_index + 1) % (sizeof(sntp_servers) / sizeof(sntp_servers[0]));
  }

  return (NX_NOT_SUCCESSFUL);
}

static UINT unix_time_get(ULONG *unix_time)
{
  /* Return number of seconds since Unix Epoch (1/1/1970 00:00:00).  */
  *unix_time = unix_time_base + (tx_time_get() / TX_TIMER_TICKS_PER_SECOND);

  return (NX_SUCCESS);
}
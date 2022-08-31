/**
  ******************************************************************************
  * @file    cellular_app_cmd.c
  * @author  MCD Application Team
  * @brief   Cellular Application Command management
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
#include "plf_config.h"

#if (USE_CELLULAR_APP == 1)
#if (USE_CMD_CONSOLE == 1)

#include <string.h>
#include <stdbool.h>

#include "cellular_app_cmd.h"

#include "cellular_app.h"
#include "cellular_app_trace.h"

#include "cellular_app_socket.h"
#include "cellular_app_echoclient.h"
#include "cellular_app_pingclient.h"

#include "cmd.h"

/* Private typedef -----------------------------------------------------------*/
/* cmd 'cellularapp' [help] */
/* cmd 'echoclient   [help|perf <n>|stat|stat reset|status]' */
/* cmd 'echoclient i [on|off|period <n>|protocol <type>|server <name>|size <n>]' */
/* cmd 'ping         [help|ip1|ip2|ddd.ddd.ddd.ddd|status]' */
#define CELLULAR_APP_CMD_ARG_MAX_NB                5U  /* Maximum number of arguments in a command line      */

#define ECHOCLIENT_PERF_MIN_ITER                   5U  /* Minimum value for <n> in cmd 'echoclient perf <n>' */
#define ECHOCLIENT_PERF_MAX_ITER                 255U  /* Maximum value for <n> in cmd 'echoclient perf <n>' */

/* Private defines -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t *p_cellular_app_cmd_argv[CELLULAR_APP_CMD_ARG_MAX_NB]; /* pointer on arguments in a cmd line */
static uint8_t cellular_app_cmd_argc; /* arguments number in a cmd line */

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Callback to treat command input for all CellularApp */
static void cellular_app_cmd_cb(uint8_t *p_cmd_line);

/* Treat 'cellularapp' generic cmd */
static void cellular_app_cmd_generic(void);
/* Treat 'echoclient' generic command */
static bool cellular_app_cmd_echoclient_generic(void);
/* Treat 'echoclient' instance command */
static void cellular_app_cmd_echoclient_instance(void);
/* Treat 'pingclient' command */
static void cellular_app_cmd_pingclient(void);

/* Treat 'cellularapp' help command */
static void cellular_app_cmd_help(void);
/* Treat 'echoclient' help command */
static void cellular_app_cmd_echoclient_help(void);
/* Treat 'pingclient' help command */
static void cellular_app_cmd_pingclient_help(void);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Callback to treat command input for all CellularApp
  * @param  p_cmd_line   - pointer on command parameters
  * @retval -
  */
static void cellular_app_cmd_cb(uint8_t *p_cmd_line)
{
  uint8_t len; /* parameter length */
  const uint8_t *p_cmd;

  p_cmd = (uint8_t *)strtok((CRC_CHAR_t *)p_cmd_line, " \t"); /* Get the first token and tokenize cmd line */

  if (p_cmd != NULL)
  {
    len = (uint8_t)crs_strlen(p_cmd);

    /* Parameters parsing */
    for (cellular_app_cmd_argc = 0U; cellular_app_cmd_argc < CELLULAR_APP_CMD_ARG_MAX_NB; cellular_app_cmd_argc++)
    {
      /* strtok() with NULL parameter asks strtok() to continue tokenizing the string passes previously */
      p_cellular_app_cmd_argv[cellular_app_cmd_argc] = (uint8_t *)strtok(NULL, " \t");
      if (p_cellular_app_cmd_argv[cellular_app_cmd_argc] == NULL) /* No more token */
      {
        break; /* exit from loop */
      }
    }

    /* The same callback is used for 'cellularapp' 'echoclient' 'ping', must call the right function */
    /* Is it a command cellularapp ? */
    if (memcmp((const CRC_CHAR_t *)p_cmd, "cellularapp", len) == 0)
    {
      cellular_app_cmd_generic();
    }
    /* Is it a command echoclient ? */
    else if (memcmp((const CRC_CHAR_t *)p_cmd, "echoclient", len) == 0)
    {
      /* Check if it is a generic command : echoclient [help|perf <n>|stat|stat reset|status] */
      if (cellular_app_cmd_echoclient_generic() == false)
      {
        /* EchoClt generic not able to treat the command line.  Check if it is an instance command :
         * echoclient [i] [on|off|period <n>|protocol <type>|server <name>|size <n>] */
        cellular_app_cmd_echoclient_instance();
      }
    }
    /* Is it a command pingclient ? */
    else if (memcmp((const CRC_CHAR_t *)p_cmd, "ping", len) == 0)
    {
      /* ping [help|ip1|ip2|status|xxx] */
      cellular_app_cmd_pingclient();
    }
    else
    {
      __NOP(); /* Nothing to do */
    }
  }
}

/**
  * @brief  Treat 'cellularapp' generic cmd
  * @param  -
  * @retval -
  */
static void cellular_app_cmd_generic(void)
{
  uint32_t len; /* length of an argument in the command */

  /* cmd 'cellularapp' [help] ? */
  if (cellular_app_cmd_argc == 0U) /* No parameters */
  {
    /* cmd 'cellularapp' without parameter not authorized */
    /* Display help */
    cellular_app_cmd_help();
  }
  else /* At least one parameter provided */
  {
    /* cmd 'cellularapp' [help] ? */
    len = crs_strlen(p_cellular_app_cmd_argv[0]);
    if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[0], "help", len) == 0)
    {
      /* cmd 'cellularapp help': display help */
      cellular_app_cmd_help();
    }
    else
    {
      /* cmd 'cellularapp xxx ...' with unknown parameter not authorized */
      /* Display help */
      cellular_app_cmd_help();
    }
  }
}

/**
  * @brief  Treat 'echoclient' generic command
  * @param  -
  * @retval bool - false/true : not treated(not a generic command) / treated (no need to call EchClt instance function)
  */
static bool cellular_app_cmd_echoclient_generic(void)
{
  bool result = true; /* command has been treated */
  uint32_t len;       /* length of an argument in the command */
  uint32_t total_ko;  /* number of echoclient ko */
  const uint8_t *trace_label = cellular_app_type_string[CELLULAR_APP_TYPE_ECHOCLIENT]; /* use for trace */

  /* cmd 'echoclient' [help|perf <n>|stat|stat reset|status] ? */
  if (cellular_app_cmd_argc == 0U) /* No parameters */
  {
    /* cmd 'echoclient' without parameter not authorized */
    /* Display EchoClt help */
    cellular_app_cmd_echoclient_help();
  }
  else /* At least one parameter provided */
  {
    /* cmd 'echoclient' [help|perf <n>|stat|stat reset|status] ? */
    len = crs_strlen(p_cellular_app_cmd_argv[0]);
    if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[0], "help", len) == 0)
    {
      /* cmd 'echoclient help': display EchoClt help */
      cellular_app_cmd_echoclient_help();
    }
    else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[0], "perf", len) == 0)
    {
#if (USE_LOW_POWER == 1)
      /* If low power is activated, a close socket is systematically done after each exchange,
       * Performance feature is then deactivated */
      /* Display a reminder: with a build low power, performance feature can not be activated */
      PRINT_FORCE("%s: Low power is activated! This feature is not managed in this case!", trace_label)
#else /* USE_LOW_POWER == 0 */
      /* cmd 'echoclient perf ...': start performance snd/rcv test */
      /* all process must be off before to do a performance test */
      /* cmd 'echoclient perf' <n> */
      if (cellular_app_cmd_argc == 1U) /* Is <n> provided ? cmd 'echoclient perf' or cmd 'echoclient perf' <n> */
      {
        /* No <n> provided : request performance snd/rcv test with the programmed iterations */
        if (cellular_app_performance_start(CELLULAR_APP_TYPE_ECHOCLIENT, 0U) == false)
        {
          __NOP();
          /* cellular_app_performance_start() already display an error according linked to the error */
        }
      }
      else
      {
        int32_t  atoi_res;  /* result of crs_atoi() */

        /* cmd 'echoclient perf <n>': start performance snd/rcv test with <n> iterations */
        /* Check <n> parameter */
        atoi_res = crs_atoi(p_cellular_app_cmd_argv[1]);
        if ((atoi_res > 0) /* conversion is ok */
            && (atoi_res >= ((int32_t)(ECHOCLIENT_PERF_MIN_ITER)))
            && (atoi_res <= ((int32_t)(ECHOCLIENT_PERF_MAX_ITER))))
        {
          /* <n> provided : request performance snd/rcv test with <n> iterations */
          if (cellular_app_performance_start(CELLULAR_APP_TYPE_ECHOCLIENT, (uint8_t)atoi_res) == false)
          {
            __NOP();
            /* cellular_app_performance_start() already display an error according linked to the error */
          }
        }
        else
        {
          /* <n> parameter is incorrect */
          /* Display a reminder about iter [min,max] */
          PRINT_FORCE("%s: Parameter iter must be [%d,%d] !", trace_label,
                      ECHOCLIENT_PERF_MIN_ITER, ECHOCLIENT_PERF_MAX_ITER)
        }
      }
#endif /* USE_LOW_POWER == 1 */
    }
    else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[0], "stat", len) == 0)
    {
      /* cmd 'echoclient stat ...' : echoclient statistics */
      /* cmd 'echoclient stat reset' : echoclient statistics reset */
      if (cellular_app_cmd_argc == 1U) /* No parameter after 'stat' */
      {
        /* cmd 'echoclient stat' : display echoclient statistics for all instances */
        PRINT_FORCE("<<< Being %s Statistics >>>\r\n", trace_label)
        for (uint8_t i = 0U; i < ECHOCLIENT_THREAD_NUMBER; i++)
        {
          cellular_app_socket_stat_desc_t socket_stat;
          if (cellular_app_socket_get_stat(CELLULAR_APP_TYPE_ECHOCLIENT, i, &socket_stat) == true)
          {
            /* Sum the number of echoclient errors */
            total_ko = socket_stat.connect.ko + socket_stat.close.ko + socket_stat.send.ko + socket_stat.receive.ko;
            PRINT_FORCE("%s %d Statistics:", trace_label, (i + 1U))
            PRINT_FORCE("  o loop count:%ld ok:%ld ko:%ld",
                        socket_stat.process_counter, socket_stat.receive.ok, total_ko)
            PRINT_FORCE("  o connect: ok:%ld ko:%ld - close  : ok:%ld ko:%ld",
                        socket_stat.connect.ok, socket_stat.connect.ko, socket_stat.close.ok, socket_stat.close.ko)
            PRINT_FORCE("  o send   : ok:%ld ko:%ld - receive: ok:%ld ko:%ld",
                        socket_stat.send.ok, socket_stat.send.ko, socket_stat.receive.ok, socket_stat.receive.ko)
          }
          else
          {
            /* Should not happen */
            PRINT_FORCE("%s %d: Statistics NOT found!", trace_label, (i + 1U))
          }
        }
        PRINT_FORCE("<<< End   %s Statistics >>>\r\n", trace_label)
      }
      else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[1], "reset", len) == 0)
      {
        /* cmd 'echoclient stat reset' : reset echoclient statistics for all instances */
        for (uint8_t i = 0U; i < ECHOCLIENT_THREAD_NUMBER; i++)
        {
          /* Request statistics reset for EchoClt instance i */
          cellular_app_socket_reset_stat(CELLULAR_APP_TYPE_ECHOCLIENT, i);
          PRINT_FORCE("%s %d: Statistics reset", trace_label, (i + 1U))
        }
      }
      else
      {
        /* cmd 'echoclient stat xxx': unknown parameter */
        PRINT_FORCE("%s: Parameter %s not supported!", trace_label, p_cellular_app_cmd_argv[1])
      }
    }
    /* len = length of the input so 'stat' must be checked before 'status' */
    else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[0], "status", len) == 0)
    {
      /* cmd 'echoclient status' : display status of EchoClt instances
       * Distant Server Name and IP Port,
       * Protocol - current and requested,
       * Period between two send,
       * Size of buffer send
       */
      PRINT_FORCE("<<< Begin %s Status >>>\r\n", trace_label)
      /* Request display status for all EchoClt instances */
      cellular_app_echoclient_display_status();
      PRINT_FORCE("<<< End   %s Status >>>", trace_label)
    }
    else /* cmd 'echoclient xxx ...': unknown by echoclient generic treatment */
    {
      result = false; /* command has not be treated by EchoClt generic */
    }
  }

  return (result);
}

/**
  * @brief  Treat 'echoclient' instance command
  * @param  -
  * @retval -
  */
static void cellular_app_cmd_echoclient_instance(void)
{
  uint8_t echoclt_index = 0U; /* echoclient index = (instance - 1U) if not provided in cmd line then index = 0U */
  uint8_t argv_index = 0U;    /* index in the command arguments - start on first parameter */
  uint8_t len;                /* length of an argument in the command */
  int32_t atoi_res;           /* result of crs_atoi() */
  const uint8_t *trace_label = cellular_app_type_string[CELLULAR_APP_TYPE_ECHOCLIENT]; /* use for trace */

  /* If first parameter is a number then it's referring to an instance number   */
  /* echoclient on - echoclient 1 on: refer to echoclient instance 1 so index 0 */
  /* echoclient 2 on: refer to echoclient instance 2 so index 1 on              */
  if ((*p_cellular_app_cmd_argv[argv_index] >= (uint8_t)'0') && (*p_cellular_app_cmd_argv[argv_index] <= (uint8_t)'9'))
  {
    /* Fist character is a number */
    if (((uint8_t)crs_strlen(p_cellular_app_cmd_argv[argv_index]) == 1U) /* length parameter is 1 so number is [0, 9] */
        /* Check instance is [1, ECHOCLIENT_THREAD_NUMBER] */
        && (*p_cellular_app_cmd_argv[argv_index] > ((uint8_t)'0'))
        && (*p_cellular_app_cmd_argv[argv_index] <= ((uint8_t)'0' + ECHOCLIENT_THREAD_NUMBER)))
    {
      /* instance: from 1 to ECHOCLIENT_THREAD_NUMBER
         index   : from 0 to ECHOCLIENT_THREAD_NUMBER - 1 */
      echoclt_index = (*p_cellular_app_cmd_argv[argv_index] - (uint8_t)'0') - 1U;
      argv_index++; /* skip 'instance' argument */
    }
    else /* instance parameter nok */
    {
      /* Display a reminder about possible values for instance */
      PRINT_FORCE("%s: Parameter 'instance' must be [%d, %d]!", trace_label, 1, ECHOCLIENT_THREAD_NUMBER)
      /* Set EchoClt instance to an impossible value stop the treatment */
      echoclt_index = 0xFFU;
    }
  }
  /* else first parameter is not instance, echoclt_index already to set to 0U */

  if ((echoclt_index != 0xFFU) /* if cmd 'echoclient n' with value 'n' nok then echoclt_index = 0xFFU */
      && (argv_index < cellular_app_cmd_argc)) /* if cmd 'echoclient' or 'echoclient <n>' only, then error */
  {
    /* Still some parameters to analyze */
    /* cmd 'echoclient ...' or cmd 'echoclient 'n' ...' */
    len = (uint8_t)crs_strlen(p_cellular_app_cmd_argv[argv_index]); /* length of the next argument in the command */
    if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index], "on", len) == 0)
    {
      /* cmd 'echoclient [i] on': start echoclient i only if not already started */
      cellular_app_process_status_t process_status;
      /* Get process status of EchoClt index */
      process_status = cellular_app_echoclient_get_status(echoclt_index);

      switch (process_status)
      {
        case CELLULAR_APP_PROCESS_OFF :
          /* Request start EchoClt index */
          if (cellular_app_set_status(CELLULAR_APP_TYPE_ECHOCLIENT, echoclt_index, true) == true)
          {
            PRINT_FORCE("<<< %s %d START requested...>>>", trace_label, (echoclt_index + 1U))
          }
          else
          {
            PRINT_FORCE("%s %d START rejected!", trace_label, (echoclt_index + 1U))
          }
          break;
        case CELLULAR_APP_PROCESS_ON :
        case CELLULAR_APP_PROCESS_STOP_REQUESTED :
        case CELLULAR_APP_PROCESS_START_REQUESTED :
          /* Reject start EchoClt index */
          PRINT_FORCE("%s %d: Bad state to do this command (current state: %s) !", trace_label, (echoclt_index + 1U),
                      cellular_app_process_status_string[process_status]);
          break;
        case CELLULAR_APP_PROCESS_STATUS_MAX :
        default :
          __NOP(); /* Impossible case */
          break;
      }
    }
    else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index], "off", len) == 0)
    {
      /* cmd 'echoclient [i] off': stop echoclient i only if already started */
      cellular_app_process_status_t process_status;
      /* Get process status of EchoClt index */
      process_status = cellular_app_echoclient_get_status(echoclt_index);

      switch (process_status)
      {
        case CELLULAR_APP_PROCESS_ON :
          /* Request stop EchoClt index */
          if (cellular_app_set_status(CELLULAR_APP_TYPE_ECHOCLIENT, echoclt_index, false) == true)
          {
            PRINT_FORCE("<<< %s %d STOP requested...>>>", trace_label, (echoclt_index + 1U))
          }
          else
          {
            PRINT_FORCE("%s %d: STOP rejected!", trace_label, (echoclt_index + 1U))
          }
          break;
        case CELLULAR_APP_PROCESS_OFF :
        case CELLULAR_APP_PROCESS_STOP_REQUESTED :
        case CELLULAR_APP_PROCESS_START_REQUESTED :
          /* Reject stop EchoClt index */
          PRINT_FORCE("%s %d: Bad state to do this command (current state: %s)!", trace_label, (echoclt_index + 1U),
                      cellular_app_process_status_string[process_status]);
          break;
        case CELLULAR_APP_PROCESS_STATUS_MAX :
        default :
          __NOP(); /* Impossible case */
          break;
      }
    }
    else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index], "size", len) == 0)
    {
      /* cmd 'echoclient [i] size xxx': for echoclient i, configure size of message to send */
      atoi_res = 0; /* default value: 0 impossible size value means new size value not provided or incorrect */
      argv_index++; /* skip 'size' argument */
      if (cellular_app_cmd_argc == (argv_index + 1U)) /* a size value must be provided */
      {
        atoi_res = crs_atoi(p_cellular_app_cmd_argv[argv_index]);
        /* res must be a number >= minimal size */
        /* reserve one byte for '\0' */
        if ((atoi_res > 0) && (atoi_res < 0xFFFF))
        {
          /* Request size modification */
          if (cellular_app_set_snd_buffer_len(CELLULAR_APP_TYPE_ECHOCLIENT, echoclt_index, (uint16_t)atoi_res) == true)
          {
            PRINT_FORCE("%s %d: New trame size: %ldbytes requested", trace_label, (echoclt_index + 1U), atoi_res)
          }
          else
          {
            PRINT_FORCE("%s %d: New trame size: %ldbytes NOT requested!", trace_label, (echoclt_index + 1U), atoi_res)
          }
        }
        else
        {
          /* Impossible size value */
          atoi_res = 0;
        }
      }

      if (atoi_res == 0) /* size value not provided or incorrect */
      {
        /* Display a reminder about size availability / value */
        PRINT_FORCE("%s: value for 'size' must be provided and > 0 !", trace_label)
      }
    }
    else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index], "period", len) == 0)
    {
      /* cmd 'echoclient [i] period xxx':  for echoclient i, configure period between two sends */
      atoi_res = 0; /* default value: 0 impossible period value means new period value not provided or incorrect */
      argv_index++; /* skip 'period' argument */
      if (cellular_app_cmd_argc == (argv_index + 1U)) /* a period value must be provided */
      {
        atoi_res = crs_atoi(p_cellular_app_cmd_argv[argv_index]);
        /* res must be a number */
        if (atoi_res > 0)
        {
          /* Request period modification */
          if (cellular_app_set_period(CELLULAR_APP_TYPE_ECHOCLIENT, echoclt_index, (uint32_t)atoi_res) == true)
          {
            PRINT_FORCE("%s %d: New process period: %ldms requested", trace_label, (echoclt_index + 1U),
                        (uint32_t)atoi_res)
          }
          else
          {
            PRINT_FORCE("%s %d: New process period: %ldms NOT requested!", trace_label, (echoclt_index + 1U),
                        atoi_res)
          }
        }
        /* else atoi_res = 0 - nothing to do */
      }

      if (atoi_res == 0)  /* period value not provided or incorrect */
      {
        /* Display a reminder about period availability */
        PRINT_FORCE("%s: value for 'period' must be provided and > 0 !", trace_label)
      }
    }
    else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index], "protocol", len) == 0)
    {
      /* cmd 'echoclient [i] protocol xxx': for echoclient i, configure protocol used to address the distant */
      /* protocol default value: CELLULAR_APP_SOCKET_PROTO_MAX impossible protocol value means
       * new protocol value not provided or incorrect */
      cellular_app_socket_protocol_t protocol = CELLULAR_APP_SOCKET_PROTO_MAX;
      argv_index++; /* skip 'protocol' argument */
      if (cellular_app_cmd_argc == (argv_index + 1U)) /* a protocol value must be provided */
      {
        /* Check protocol value */
        len = (uint8_t)crs_strlen(p_cellular_app_cmd_argv[argv_index]);
        if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index], "TCP", len) == 0)
        {
          protocol = CELLULAR_APP_SOCKET_TCP_PROTO;
        }
        else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index], "UDP", len) == 0)
        {
          protocol = CELLULAR_APP_SOCKET_UDP_PROTO;
        }
        else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index], "UDPSERVICE", len) == 0)
        {
          /* UDP not-connected mode supported:
           * in LwIP always
           * in Modem only if (UDP_SERVICE_SUPPORTED == 1U) */
#if (USE_SOCKETS_TYPE == USE_SOCKETS_MODEM)
#if (UDP_SERVICE_SUPPORTED == 1U)
          protocol = CELLULAR_APP_SOCKET_UDP_SERVICE_PROTO;
#else  /* UDP_SERVICE_SUPPORTED == 0U */
          /* protocol already initialized to an illegal value */
          /* protocol = CELLULAR_APP_SOCKET_PROTO_MAX; */
          __NOP();
#endif /* UDP_SERVICE_SUPPORTED == 1U */
#else  /* USE_SOCKETS_TYPE != USE_SOCKETS_MODEM */
          protocol = CELLULAR_APP_SOCKET_UDP_SERVICE_PROTO;
#endif /* USE_SOCKETS_TYPE == USE_SOCKETS_MODEM */
        }
        else /* Protocol invalid */
        {
          /* protocol already initialized to an illegal value */
          /* protocol = CELLULAR_APP_SOCKET_PROTO_MAX; */
          __NOP();
        }

        if (protocol != CELLULAR_APP_SOCKET_PROTO_MAX)
        {
          /* Request protocol modification */
          if (cellular_app_set_protocol(CELLULAR_APP_TYPE_ECHOCLIENT, echoclt_index, protocol) == false)
          {
            PRINT_FORCE("%s %d: New protocol value NOT requested!", trace_label, (echoclt_index + 1U))
          }
        }
      }

      if (protocol == CELLULAR_APP_SOCKET_PROTO_MAX) /* protocol value not provided or incorrect */
      {
        /* Display a reminder about protocol availability and values by taking into account modem capacity */
#if ((USE_SOCKETS_TYPE == USE_SOCKETS_MODEM) && (UDP_SERVICE_SUPPORTED == 0U))
        PRINT_FORCE("%s: Value for 'protocol' must be provided and [TCP|UDP] !", trace_label)
        PRINT_FORCE("          TCP|UDP: mode connected")
#else
        PRINT_FORCE("%s: Value for 'protocol' must be provided and [TCP|UDP|UDPSERVICE] !", trace_label)
        PRINT_FORCE("          TCP|UDP: mode connected - UDPSERVICE: mode not-connected")
#endif /* (USE_SOCKETS_TYPE == USE_SOCKETS_MODEM) && (UDP_SERVICE_SUPPORTED == 0U) */
      }
    }
    else if ((memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index], "server", len) == 0)
             || (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index], "distant", len) == 0))
    {
      /* cmd 'echoclient [i] server xxx': for echoclient i, configure server used */
      /* cmd 'echoclient [i] distant xxx': for echoclient i, configure distant used */
      /* distant default value: CELLULAR_APP_DISTANT_TYPE_MAX impossible distant value means
       * new distant value not provided or incorrect */
      cellular_app_distant_type_t distant = CELLULAR_APP_DISTANT_TYPE_MAX;
      argv_index++; /* skip 'server|distant' parameter */
      if (cellular_app_cmd_argc == (argv_index + 1U)) /* a server value must be provided */
      {
        len = (uint8_t)crs_strlen(p_cellular_app_cmd_argv[argv_index]); /* length of server parameter */
        /* Check parameter value versus distant possible values */
        if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index],
                   cellular_app_distant_string[CELLULAR_APP_DISTANT_MBED_TYPE], len) == 0)
        {
          distant = CELLULAR_APP_DISTANT_MBED_TYPE;
        }
        else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index],
                        cellular_app_distant_string[CELLULAR_APP_DISTANT_UBLOX_TYPE], len) == 0)
        {
          distant = CELLULAR_APP_DISTANT_UBLOX_TYPE;
        }
        else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index],
                        cellular_app_distant_string[CELLULAR_APP_DISTANT_LOCAL_TYPE], len) == 0)
        {
          distant = CELLULAR_APP_DISTANT_LOCAL_TYPE;
        }
        else /* Distant invalid */
        {
          /* distant already initialized to an illegal value */
          /* distant = CELLULAR_APP_DISTANT_TYPE_MAX; */
          __NOP();
        }

        if (distant != CELLULAR_APP_DISTANT_TYPE_MAX)
        {
          /* Request distant modification */
          if (cellular_app_echoclient_distant_change(echoclt_index, distant, NULL, 0U) == false)
          {
            PRINT_FORCE("%s %d: New distant value NOT requested!", trace_label, (echoclt_index + 1U))
          }
        }
      }

      if (distant == CELLULAR_APP_DISTANT_TYPE_MAX) /* server|distant value not provided or incorrect */
      {
        /* Display a reminder about server|distant availability and possible values */
        PRINT_FORCE("%s: Value for server must be provided and [%s|%s|%s] !", trace_label,
                    cellular_app_distant_string[CELLULAR_APP_DISTANT_MBED_TYPE],
                    cellular_app_distant_string[CELLULAR_APP_DISTANT_UBLOX_TYPE],
                    cellular_app_distant_string[CELLULAR_APP_DISTANT_LOCAL_TYPE])
      }
    }
    else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index], "valid", len) == 0)
    {
      argv_index++; /* skip 'valid' parameter */
      if (cellular_app_cmd_argc == (argv_index + 1U)) /* a value must be provided */
      {
        /* cmd 'echoclient valid xxx': use for automatic test to obtain the statistics */
        len = (uint8_t)crs_strlen(p_cellular_app_cmd_argv[argv_index]);
        if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[argv_index], "stat", len) == 0)
        {
          /* cmd 'echoclient [i] valid stat': use for automatic test to obtain the statistics for echoclient i */
          cellular_app_socket_stat_desc_t socket_stat;
          uint32_t total_ko; /* number of echoclient ko */

          /* Request statistics of EchoClt index */
          if (cellular_app_socket_get_stat(CELLULAR_APP_TYPE_ECHOCLIENT, echoclt_index, &socket_stat) == true)
          {
            /* Sum the number of echoclient errors */
            total_ko =  socket_stat.connect.ko + socket_stat.close.ko + socket_stat.send.ko + socket_stat.receive.ko;
            /* Trace is specific to validation type */
            TRACE_VALID("@valid@:echo%d:stat:%ld/%ld\n\r", (echoclt_index + 1U), socket_stat.receive.ok, total_ko)
          }
          else
          {
            /* Trace is specific to validation type */
            TRACE_VALID("@valid@:echo%d:stat:%d/%d - Not found!\n\r", (echoclt_index + 1U), 0U, 0U)
          }
        }
        else
        {
          /* Display a reminder about valid parameter availability and values */
          TRACE_VALID("@valid@:echo%d: only parameter [stat] for 'valid' is supported!\n\r", (echoclt_index + 1U))
        }
      }
      else
      {
        /* Display a reminder about valid parameter availability and values */
        TRACE_VALID("@valid@:echo%d: for 'valid' parameter [stat] must be provided!\n\r", (echoclt_index + 1U))
      }
    }
    else /* cmd 'echoclient xxx' with a param xxx not supported */
    {
      PRINT_FORCE("%s %s: Unrecognised parameter! Usage:", trace_label, p_cellular_app_cmd_argv[argv_index])
      /* Display echoclient help to remind command supported */
      cellular_app_cmd_echoclient_help();
    }
  }
  else
  {
    if (echoclt_index != 0xFFU)
    {
      /* Not enough or too many arguments */
      PRINT_FORCE("%s: incorrect command! Usage:", trace_label)
      /* Display echoclient help to remind command supported */
      cellular_app_cmd_echoclient_help();
    }
  }
}

/**
  * @brief  Treat 'pingclient' command
  * @param  -
  * @retval -
  */
static void cellular_app_cmd_pingclient(void)
{
  uint8_t pintclt_index = 0U; /* only one PingClt instance */
  uint32_t len;               /* length of an argument in the command */
  const uint8_t *trace_label = cellular_app_type_string[CELLULAR_APP_TYPE_PINGCLIENT]; /* use for trace */

  /* cmd 'ping' [help|perf <n>|stat|stat reset|status] ? */
  if (cellular_app_cmd_argc == 0U) /* No parameters is ok == ping start or stop according to the ping status */
  {
    /* cmd 'ping' with no parameter: start a ping session on current ping index or stop it according to ping status*/
    cellular_app_process_status_t process_status;
    /* Request PingClt status */
    process_status = cellular_app_pingclient_get_status();

    switch (process_status)
    {
      case CELLULAR_APP_PROCESS_OFF :
        /* Ping status is off => request Ping start */
        if (cellular_app_set_status(CELLULAR_APP_TYPE_PINGCLIENT, pintclt_index, true) == true)
        {
          PRINT_FORCE("<<< %s START requested...>>>", trace_label)
        }
        else
        {
          PRINT_FORCE("%s: START rejected!", trace_label)
        }
        break;
      case CELLULAR_APP_PROCESS_ON :
        /* Ping status is on => request Ping stop */
        if (cellular_app_set_status(CELLULAR_APP_TYPE_PINGCLIENT, pintclt_index, false) == true)
        {
          PRINT_FORCE("<<< %s STOP requested...>>>", trace_label)
        }
        else
        {
          PRINT_FORCE("%s: STOP rejected!", trace_label)
        }
        break;
      case CELLULAR_APP_PROCESS_STOP_REQUESTED  :
      case CELLULAR_APP_PROCESS_START_REQUESTED :
        /* A Ping process already in progress, reject new modification */
        PRINT_FORCE("%s: Bad state to do this command (current state: %s)!", trace_label,
                    cellular_app_process_status_string[process_status])
        break;
      default :
        __NOP(); /* Impossible case */
        break;
    }
  }
  else /* At least one parameter provided */
  {
    /* cmd 'ping' [help|ip1|ip2|ddd.ddd.ddd.ddd|status] ? */
    len = crs_strlen(p_cellular_app_cmd_argv[0]); /* Length of parameter */
    if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[0], "help", len) == 0)
    {
      /* cmd 'ping help': display pingclient help */
      cellular_app_cmd_pingclient_help();
    }
    else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[0], "ip1", crs_strlen(p_cellular_app_cmd_argv[0])) == 0)
    {
      /* cmd 'ping ip1': cmd allowed only if no ping in progress */
      if (cellular_app_pingclient_get_status() == CELLULAR_APP_PROCESS_OFF)
      {
        /* Request 1)Ping distant change to IP1 2)Ping start */
        if (cellular_app_pingclient_distant_change(pintclt_index, CELLULAR_APP_DISTANT_IP1_TYPE, NULL, 0U) == true)
        {
          /* Request Ping start */
          if (cellular_app_set_status(CELLULAR_APP_TYPE_PINGCLIENT, pintclt_index, true) == true)
          {
            PRINT_FORCE("<<< %s START requested...>>>", trace_label)
          }
        }
      }
      else
      {
        PRINT_FORCE("%s: Already in progress! Stop it or wait its end before to retry!", trace_label)
      }
    }
    else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[0], "ip2", crs_strlen(p_cellular_app_cmd_argv[0])) == 0)
    {
      /* cmd 'ping ip2': cmd allowed only if no ping in progress */
      if (cellular_app_pingclient_get_status() == CELLULAR_APP_PROCESS_OFF)
      {
        /* Request 1)Ping distant change to IP2 2)Ping start */
        if (cellular_app_pingclient_distant_change(pintclt_index, CELLULAR_APP_DISTANT_IP2_TYPE, NULL, 0U) == true)
        {
          /* Request Ping start */
          if (cellular_app_set_status(CELLULAR_APP_TYPE_PINGCLIENT, pintclt_index, true) == true)
          {
            PRINT_FORCE("<<< %s START requested...>>>", trace_label)
          }
        }
      }
      else
      {
        PRINT_FORCE("%s: Already in progress! Stop it or wait its end before to retry!", trace_label)
      }
    }
    else if (memcmp((CRC_CHAR_t *)p_cellular_app_cmd_argv[0], "status", len) == 0)
    {
      /* cmd 'ping status' : display status of ping
       * IP1 value
       * IP2 value
       * Dynamic IP value - if defined
       * Ping index
       * Ping session status
       */
      PRINT_FORCE("<<< Begin %s Status >>>\r\n", trace_label)
      /* Request display status for Ping */
      cellular_app_pingclient_display_status();
      PRINT_FORCE("<<< End   %s Status >>>", trace_label)
    }
    else
    {
      /* cmd 'ping xxx' enter - last command supported is to provide a IP address */
      /* This modification is possible only if Ping process is off */
      if (cellular_app_pingclient_get_status() == CELLULAR_APP_PROCESS_OFF)
      {
        /* Request 1)Ping distant change to IPx address 2)Ping start */
        if (cellular_app_pingclient_distant_change(pintclt_index, CELLULAR_APP_DISTANT_IPx_TYPE,
                                                   p_cellular_app_cmd_argv[0], len)
            == true)
        {
          /* Request Ping start */
          if (cellular_app_set_status(CELLULAR_APP_TYPE_PINGCLIENT, pintclt_index, true) == true)
          {
            PRINT_FORCE("<<< %s START requested...>>>", trace_label)
          }
        }
        else
        {
          PRINT_FORCE("%s: Bad parameter or invalid IP %s!", p_cellular_app_cmd_argv[0], trace_label)
        }
      }
      else
      {
        PRINT_FORCE("%s: Already in progress! Stop it or wait its end before to retry!", trace_label)
      }
    }
  }
}

/**
  * @brief  Treat 'cellularapp' help command: display help for all cellular applications
  * @param  -
  * @retval -
  */
static void cellular_app_cmd_help(void)
{
  CMD_print_help((uint8_t *)"cellularapp");
  /* The only possible command for cellularapp is 'help' */
  PRINT_FORCE("cellularapp help    : display all commands supported")
  /* Display help for EchoClt */
  cellular_app_cmd_echoclient_help();
  /* Display help for PingClt */
  cellular_app_cmd_pingclient_help();
}

/**
  * @brief  Treat 'echoclient' help command
  * @param  -
  * @retval -
  */
static void cellular_app_cmd_echoclient_help(void)
{
  CMD_print_help((uint8_t *)"echoclient");

  /* Display information about EchoClt cmd and its supported parameters */
  PRINT_FORCE("echoclient help           : display this help   ")
  PRINT_FORCE("echoclient [i] on         : start echoclient i")
  PRINT_FORCE("echoclient [i] off        : stop  echoclient i")
  PRINT_FORCE("echoclient [i] period <n> : set the process period to n (ms) for echoclient i")
  PRINT_FORCE("echoclient [i] size <n>   : set buffer size to n (bytes)for echoclient i")

  /* UDP not-connected mode supported
   * always in LwIP
   * in Modem only if UDP_SERVICE_SUPPORTED == 1U */
#if ((USE_SOCKETS_TYPE == USE_SOCKETS_MODEM) && (UDP_SERVICE_SUPPORTED == 0U))
  PRINT_FORCE("echoclient [i] protocol [TCP|UDP] : for echoclient i, set socket protocol to:")
  PRINT_FORCE("                                    TCP|UDP: connected mode")
#else
  PRINT_FORCE("echoclient [i] protocol [TCP|UDP|UDPSERVICE] : for echoclient i, set protocol to:")
  PRINT_FORCE("                                               TCP|UDP: connected mode")
  PRINT_FORCE("                                               UDPSERVICE: UDP not-connected mode")
#endif /* (USE_SOCKETS_TYPE == USE_SOCKETS_MODEM) && (UDP_SERVICE_SUPPORTED == 0U) */

  PRINT_FORCE("echoclient [i] server [%s|%s|%s] : for echoclient i, set distant Server to %s|%s|%s",
              cellular_app_distant_string[CELLULAR_APP_DISTANT_MBED_TYPE],
              cellular_app_distant_string[CELLULAR_APP_DISTANT_UBLOX_TYPE],
              cellular_app_distant_string[CELLULAR_APP_DISTANT_LOCAL_TYPE],
              cellular_app_distant_string[CELLULAR_APP_DISTANT_MBED_TYPE],
              cellular_app_distant_string[CELLULAR_APP_DISTANT_UBLOX_TYPE],
              cellular_app_distant_string[CELLULAR_APP_DISTANT_LOCAL_TYPE])

  PRINT_FORCE("echoclient status     : for all echoclients, display:")
  PRINT_FORCE("                        Server Name, IP and Port, Protocol, Period, Size value and state")
  PRINT_FORCE("echoclient perf       : using echoclient 1, start performance snd/rcv test with default iterations nb")
  PRINT_FORCE("echoclient perf <n>   : using echoclient 1, start performance snd/rcv test with only n iterations")
  PRINT_FORCE("echoclient stat       : display statistic of all echoclients")
  PRINT_FORCE("echoclient stat reset : reset statistic of all echoclients")
}

/**
  * @brief  Treat 'pingclient' help command
  * @param  -
  * @retval -
  */
static void cellular_app_cmd_pingclient_help(void)
{
  CMD_print_help((uint8_t *)"ping");

  /* Display information about Ping cmd and its supported parameters */
  PRINT_FORCE("ping help            : display this help   ")
  PRINT_FORCE("ping                 : if no ping in progress,")
  PRINT_FORCE("                       start a 10 pings session to IP address pointed by Ping index")
  PRINT_FORCE("                       else stop the ping session and set Ping index to the next defined IP")
  PRINT_FORCE("ping ip1             : if no ping in progress, set Ping index to IP1 and start a 10 pings session")
  PRINT_FORCE("ping ip2             : if no ping in progress, set Ping index to IP2 and start a 10 pings session")
  PRINT_FORCE("ping ddd.ddd.ddd.ddd : if no ping in progress,")
  PRINT_FORCE("                       set Dynamic IP address to ddd.ddd.ddd.ddd,r")
  PRINT_FORCE("                       set Ping index to Dynamic IP and start a 10 pings session")
  PRINT_FORCE("ping status          : display addresses for IP1, IP2, Dynamic IP, current Ping index and Ping state")
}

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Initialization CellularApp command management
  * @param  -
  * @retval -
  */
void cellular_app_cmd_init(void)
{
  __NOP(); /* Nothing to do */
}

/**
  * @brief  Start CellularApp command management
  * @note   Registration to CMD module
  * @param  -
  * @retval -
  */
void cellular_app_cmd_start(void)
{
  /* Same callback is used for cellularapp, echoclient, ping */
  /* Registration to cmd module to support cellularapp cmd */
  CMD_Declare((uint8_t *)"cellularapp", cellular_app_cmd_cb, (uint8_t *)"CellularApp commands");
  /* Registration to cmd module to support echoclient cmd  */
  CMD_Declare((uint8_t *)"echoclient", cellular_app_cmd_cb, (uint8_t *)"EchoClient commands");
  /* Registration to cmd module to support ping cmd        */
  CMD_Declare((uint8_t *)"ping", cellular_app_cmd_cb, (uint8_t *)"Ping commands");
}

#endif /* USE_CMD_CONSOLE == 1 */
#endif /* USE_CELLULAR_APP == 1 */

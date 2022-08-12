/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

#include "plf_sw_config.h"
#include "plf_hw_config.h"
#include "rng.h"
   
int hardware_rand(void)
{

    /* Return the random number.  */
    uint32_t aRandom32bit;
    HAL_RNG_GenerateRandomNumber(&hrng, &aRandom32bit);
    return aRandom32bit;
}

#if (USE_PRINTF == 0)
#if (USE_TRACE_APPLICATION == 1)
#include "trace_interface.h"
size_t __write(int handle, const unsigned char *buf, size_t bufSize)
{
  /* Go through trace interface module to display traces */
  traceIF_uartPrintForce(DBG_CHAN_APPLICATION, (uint8_t *)buf, bufSize);
  return (bufSize);
}
#else /* USE_TRACE_APPLICATION == 0 */
size_t __write(int handle, const unsigned char *buf, size_t bufSize)
{
  UNUSED(handle);
  UNUSED(buf);
  return (bufSize);
}
#endif /* USE_TRACE_APPLICATION == 1 */
#else /* USE_PRINTF == 1 */
size_t __write(int handle, const unsigned char *buf, size_t bufSize)
{

    /* Check for the command to flush all handles */
    if (handle == -1) 
    {    
        return 0;  
    }    
    /* Check for stdout and stderr      (only necessary if FILE descriptors are enabled.) */  
    if (handle != 1 && handle != 2)  
    {    
        return -1;  
    }   

    if(HAL_UART_Transmit(&TRACE_INTERFACE_UART_HANDLE, (uint8_t*)buf, bufSize, 5000) != HAL_OK)
    {    
        return -1;  
    }    

    return bufSize;
}

size_t __read(int handle, unsigned char *buf, size_t bufSize)
{  

    /* Check for stdin      (only necessary if FILE descriptors are enabled) */ 
    if (handle != 0)  
    {    
        return -1;  
    }   

    if(HAL_UART_Receive(&TRACE_INTERFACE_UART_HANDLE, (uint8_t *)buf, bufSize, 0x10000000) != HAL_OK)
    {    
        return -1;  
    }
    return bufSize;
}
#endif
/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    main.h
* @data         2015/11/30
* @auther       yan zeyu
* @module       main module
* @brief        main module
***************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/***************************************************************************************************
 * INCLUDES
 */
#include "stm32l0xx_hal.h"
#include "uart.h"
#include "clock.h"

#include "stm32l0538_discovery.h"   //for debug

/***************************************************************************************************
 * MACRO
 */
//#define FLASH_DISABLE

#ifdef FLASH_DISABLE
#define FLASH_SECTORS_TO_BE_PROTECTED    OB_WRP_AllPages
#endif

#endif /* __MAIN_H */

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/

/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    processing.h
* @data         2015/11/29
* @auther       yan zeyu
* @module       flash module
* @brief        flash module
***************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OTA_H
#define __OTA_H

/***************************************************************************************************
 * INCLUDES
 */
#include "stm32l0xx_hal.h"
#include "custom_protocol.h"
#include "stop.h"
#include "jump.h"
#include "clock.h"
#include "xmodem.h"

#include "stm32l0538_discovery.h"

/***************************************************************************************************
 * MACRO
 */
#define FRAME_TIMEOUT       2000
#define WAIT_FOR_SERVER     200
#define WAKEUP_PERIOD       5
 
typedef enum 
{
    NO_ERROR        = 0x01,	/* no error occur */
    CHECK_ERROR     = 0x05,
    TRANS_ERROR     = 0x06,
    XMODEM_ERROR    = 0x07,
    FLASH_ERROR     = 0x08
    
} errorTypeDef;

/***************************************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
 *
***************************************************************************************************/
/***************************************************************************************************
 * @fn      ota()
 *
 * @brief   ota bootloader main processing.
 *
 * @author  yan zeyu
 *
 * @param   pageStart: the address of the first page to be erased.
 * 					wordsLength: the number of words need to be erased.
 * @return  state of option
 */
void ota(void);

#endif	/* __OTA_H */

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/


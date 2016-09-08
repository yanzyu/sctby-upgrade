/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    stop.h
* @data         2015/11/27
* @auther       yan zeyu
* @module       flash module
* @brief        flash module
***************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STOP_H
#define __STOP_H

/***************************************************************************************************
 * INCLUDES
 */
#include "stm32l0xx_hal.h"

/***************************************************************************************************
 * MACRO
 */
#define RTC_ASYNCH_PREDIV  0x7F
#define RTC_SYNCH_PREDIV   0x0130

/***************************************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
 *
***************************************************************************************************/
/***************************************************************************************************
 * @fn      wakeUpConfig()
 *
 * @brief   Config the RTC wakeup function.
 *
 * @author  yan zeyu
 *
 * @param   none
 *
 * @return  state of option
 */
HAL_StatusTypeDef wakeUpConfig(void);

/***************************************************************************************************
 * @fn      wakeUpDisable()
 *
 * @brief   Disable the RTC wakeup function.
 *
 * @author  yan zeyu
 *
 * @param   none
 *
 * @return  state of option
 */
HAL_StatusTypeDef wakeUpDisable(void);

/***************************************************************************************************
 * @fn      setWakeUpTime()
 *
 * @brief   Set the period of wakeup interrupt.
 *
 * @author  yan zeyu
 *
 * @param   period: auto wakeup period
 *
 * @return  state of option
 */
HAL_StatusTypeDef setWakeUpPeriod(uint32_t period);

/***************************************************************************************************
 * @fn      enterStopMode()
 *
 * @brief   make the mcu enter stop mode.
 *
 * @author  yan zeyu
 *
 * @param   none.
 *
 * @return  none.
 */
void enterStopMode(void);

/***************************************************************************************************
 * @fn      wakeUpTimeIRQHandler()
 *
 * @brief   auto-wakeup interrupt handle.
 *
 * @author  yan zeyu
 *
 * @param   none.
 *
 * @return  none.
 */
void wakeUpTimeIRQHandler(void);

/***************************************************************************************************
 * @fn      HAL_RTC_MspInit()
 *
 * @brief   This function configures the hardware resources used in this example: 
 *           - Peripheral's clock enable
 *           - NVIC configuration for RTC interrupt request enable
 
 * @author  yan zeyu
 *
 * @param   hrtc: RTC handle pointer.
 *
 * @return  none.
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);

/***************************************************************************************************
 * @fn      HAL_RTC_MspDeInit()
 *
 * @brief   This function freeze the hardware resources used in this example:
 *          - Disable the Peripheral's clock
 
 * @author  yan zeyu
 *
 * @param   hrtc: RTC handle pointer.
 *
 * @return  none.
 */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc);

#endif  /* __STOP_H */

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/


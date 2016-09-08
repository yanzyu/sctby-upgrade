/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    clock.h
* @data         2015/11/27
* @auther       yan zeyu
* @module       flash module
* @brief        flash module
***************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CLOCK_H
#define __CLOCK_H

/***************************************************************************************************
 * INCLUDES
 */
 #include "stm32l0xx_hal.h"
 
/***************************************************************************************************
 * @fn      clkConfig()
 *
 * @brief   Config the mcu clock.
 *          Use HSI osc, open the PLL, running on 32MHz.
 
 * @author  yan zeyu
 *
 * @param   none.
 *
 * @return  none.
 */
void clkConfig(void);

#endif /* __CLOCK_H */

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/


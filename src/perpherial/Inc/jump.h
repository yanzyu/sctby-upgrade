/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    jump.h
* @data         2015/11/27
* @auther       yan zeyu
* @module       flash module
* @brief        flash module
***************************************************************************************************/
#ifndef __JUMP_H
#define __JUMP_H

/***************************************************************************************************
 * INCLUDES
 */
#include "stm32l0xx_hal.h"

/***************************************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
 *
***************************************************************************************************/
/***************************************************************************************************
 * @fn      Jump()
 *
 * @brief   Jump to the user application
 *
 * @author  yan zeyu
 *
 * @param   appEntry: the entry addess of the main application.
 *
 * @return  none.
 */
void jump(uint32_t appEntry);

#endif  /* __JUMP_H */

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/

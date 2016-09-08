/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    jump.c
* @data         2015/11/27
* @auther       yan zeyu
* @module       flash module
* @brief        flash module
***************************************************************************************************/
/***************************************************************************************************
 * INCLUDES
 */
#include "jump.h"

/***************************************************************************************************
 * PRIVATE VARIABLE
 */
typedef  void (*pFunction)(void);
pFunction JumpToApplication;
uint32_t JumpAddress;

/***************************************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
 *
***************************************************************************************************/
/***************************************************************************************************
 * @fn      jump()
 *
 * @brief   Jump to the user application
 *
 * @author  yan zeyu
 *
 * @param   appEntry: the entry addess of the main application.
 *
 * @return  none.
 */
void jump(uint32_t appEntry)
{
    JumpAddress = *(__IO uint32_t*) (appEntry + 4);
    /* Jump to user application */
    JumpToApplication = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*)appEntry);

    JumpToApplication();
}

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/

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
#ifndef __BOOT_MAIN_H
#define __BOOT_MAIN_H

/***************************************************************************************************
 * INCLUDES
 */
#include "stm32l0xx_hal.h"

/***************************************************************************************************
 * MACRO
 */
#define APP_BASE    0x08003000
#define DESC_BASE   0x08003300
typedef struct{
    unsigned int entry;
    unsigned int size;
    unsigned int ver;   /* use only byte 2 = major, byte 3 = minor */
    unsigned int crc; /* 似乎没有办法在这里加入crc校验值，因为程序复位后，在初始化阶段应该会将这个值初始化 */
}moduleInfo_t;  // module info type
    
typedef struct{
    unsigned int firmVer;
    unsigned int moduleCnt;
    moduleInfo_t *module;
}moduleDesc_t;  // module description type



#endif /* __MAIN_H */

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/

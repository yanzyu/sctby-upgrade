/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    uart.h
* @data         2015/11/27
* @auther       yan zeyu
* @module       flash module
* @brief        flash module
***************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_H
#define __UART_H

/***************************************************************************************************
 * INCLUDES
 */
#include "stm32l0xx.h"

/***************************************************************************************************
 * MACRO
 */
#define UART_BAUD	115200
#define UartMaxSize 256

void InitUart1(void);
void DeinitUart1(void);
uint8_t uartSend_IT(uint8_t *buf, uint32_t n);
uint32_t uartReceive_Pkt(uint8_t *buf, uint32_t timeout);
void uartSendString(uint8_t *string);

#endif  /* __UART_H */

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/

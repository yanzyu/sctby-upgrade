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
#include "stm32l0xx_hal.h"

/***************************************************************************************************
 * MACRO
 */
#define USARTx                           USART2
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART2_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 

#define USARTx_FORCE_RESET()             __HAL_RCC_USART2_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_2
#define USARTx_TX_GPIO_PORT              GPIOA  
#define USARTx_TX_AF                     GPIO_AF4_USART2
#define USARTx_RX_PIN                    GPIO_PIN_3
#define USARTx_RX_GPIO_PORT              GPIOA 
#define USARTx_RX_AF                     GPIO_AF4_USART2

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART2_IRQn
#define USARTx_IRQHandler                USART2_IRQHandler

/***************************************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
 *
***************************************************************************************************/
/***************************************************************************************************
 * @fn      uartInit()
 *
 * @brief   Init the uart hardware, set the baud rate 9600
 *
 * @author  yan zeyu
 *
 * @param   none
 *
 * @return  state of option
 */
HAL_StatusTypeDef uartInit(void);

/***************************************************************************************************
 * @fn      disableUart()
 *
 * @brief   disable uart hardware
 *
 * @author  yan zeyu
 *
 * @param   none.
 *
 * @return  none
 */
void disableUart(void);

/***************************************************************************************************
 * @fn      uartReceive_Block()
 *
 * @brief   Receive data from uart using block mode.
 *
 * @author  yan zeyu
 *
 * @param   dataBuffer: buffer to contain the data received.
 *          length:     length of the data to be recevie.
 *          timeout:    timeout of one receive option in ms.
 *
 * @return  state of option
 */
HAL_StatusTypeDef uartReceive_Block(uint8_t* dataBuffer, uint32_t length, uint32_t timeout);

/***************************************************************************************************
 * @fn      uartReceive_Block()
 *
 * @brief   Receive data from uart using block mode.
 *
 * @author  yan zeyu
 *
 * @param   dataBuffer: buffer to contain the data received.
 *          length:     length of the data to be recevie.
 *          timeout:    timeout of one receive option in ms.
 *
 * @return  state of option
 */
HAL_StatusTypeDef uartReceive_IT(uint8_t* dataBuffer, uint32_t length);

/***************************************************************************************************
 * @fn      uartTransform_IT()
 *
 * @brief   Send data array from uart using block mode.
 *
 * @author  yan zeyu
 *
 * @param   dataSource: data array to be send.
 *          wordsLength:length of the data in bytes.
 *
 * @return  state of option
 */
HAL_StatusTypeDef uartSend_IT(uint8_t* dataSource, uint32_t length);

/***************************************************************************************************
 * @fn      uartSendString_IT()
 *
 * @brief   Send a string from uart using block mode.
 *
 * @author  yan zeyu
 *
 * @param   pString: the string to be send.
 *
 * @return  state of option
 */
HAL_StatusTypeDef uartSendString_IT(unsigned char* pString);

/***************************************************************************************************
 * @fn      uartGetError()
 *
 * @brief   get the error state of uart.
 *
 * @author  yan zeyu
 *
 * @param   none.
 *
 * @return  error state of uart
 */
uint32_t uartGetError(void);

/***************************************************************************************************
 * @fn      uartIRQHandler() / setReady()
 *
 * @brief   uart interrupt handler
 *
 * @author  yan zeyu
 *
 * @param   none.
 *
 * @return  none
 */
void uartIRQHandler(void);
void setReady(void);

/***************************************************************************************************
 * @fn      HAL_UART_MspInit()
 *
 * @brief   This function configures the hardware resources used in this example: 
 *           - Peripheral's clock enable
 *           - Peripheral's GPIO Configuration  
 *           - NVIC configuration for UART interrupt request enable
 *
 * @author  yan zeyu
 *
 * @param   huart: uart handler structure.
 *
 * @return  none
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart);

/***************************************************************************************************
 * @fn      HAL_UART_MspDeInit()
 *
 * @brief   This function frees the hardware resources used in this example:
 *          - Disable the Peripheral's clock
 *          - Revert GPIO and NVIC configuration to their default state
 *
 * @author  yan zeyu
 *
 * @param   huart: uart handler structure.
 *
 * @return  none
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart);

#endif  /* __UART_H */

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/

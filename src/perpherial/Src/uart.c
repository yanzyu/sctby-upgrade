/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    uart.c
* @data         2015/11/27
* @auther       yan zeyu
* @module       flash module
* @brief        flash module
***************************************************************************************************/
/***************************************************************************************************
 * INCLUDES
 */
#include "uart.h"

/***************************************************************************************************
 * LOCAL VARIABLE
 */
UART_HandleTypeDef UartHandle;
__IO ITStatus UartReady = RESET;

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
HAL_StatusTypeDef uartInit()
{
    UartHandle.Instance        = USARTx;
    UartHandle.Init.BaudRate   = 38400;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits   = UART_STOPBITS_1;
    UartHandle.Init.Parity     = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode       = UART_MODE_TX_RX;
    
    if(HAL_UART_Init(&UartHandle) != HAL_OK)
    {
        return HAL_ERROR;
    }
  
    return HAL_OK;
}

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
void disableUart(void)
{
    HAL_UART_MspDeInit(&UartHandle);
}

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
HAL_StatusTypeDef uartReceive_Block(uint8_t* dataBuffer, uint32_t length, uint32_t timeout)
{
	HAL_StatusTypeDef state;
	state = HAL_UART_Receive(&UartHandle, dataBuffer, length, timeout);
    if(state != HAL_OK)
        return state;
       
    return HAL_OK;
}

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
HAL_StatusTypeDef uartReceive_IT(uint8_t* dataBuffer, uint32_t length)
{
	if(HAL_UART_Receive_IT(&UartHandle, dataBuffer, length) != HAL_OK)
        return HAL_ERROR;
    
    while(UartReady != SET)
    {
    }
    UartReady = RESET;
           
    return HAL_OK;
}

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
HAL_StatusTypeDef uartSend_IT(uint8_t* dataSource, uint32_t length)
{
    if(HAL_UART_Transmit_IT(&UartHandle, (uint8_t*)dataSource, length) != HAL_OK)
        return HAL_ERROR;

    while(UartReady != SET)
    {
    }
    UartReady = RESET;
   
    return HAL_OK;
}

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
HAL_StatusTypeDef uartSendString_IT(unsigned char* pString)
{
    uint32_t stringLength = 0;
    while(pString[stringLength] != '\0')
        stringLength ++;
    
    if(HAL_UART_Transmit_IT(&UartHandle, (uint8_t*)pString, stringLength) != HAL_OK)
       return HAL_ERROR;

    while(UartReady != SET)
    {
    }
    UartReady = RESET;
    return HAL_OK;
}

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
uint32_t uartGetError(void)
{
    return HAL_UART_GetError(&UartHandle);
}

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
void uartIRQHandler(void)
{
	HAL_UART_IRQHandler(& UartHandle);
}

void setReady(void)
{
	UartReady = SET;
}

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
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
    GPIO_InitTypeDef  GPIO_InitStruct;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    USARTx_TX_GPIO_CLK_ENABLE();
    USARTx_RX_GPIO_CLK_ENABLE();
    /* Enable USART2 clock */
    USARTx_CLK_ENABLE(); 

    /*##-2- Configure peripheral GPIO ##########################################*/  
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = USARTx_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH  ;
    GPIO_InitStruct.Alternate = USARTx_TX_AF;

    HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USARTx_RX_PIN;
    GPIO_InitStruct.Alternate = USARTx_RX_AF;

    HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

    /*##-3- Configure the NVIC for UART ########################################*/
    /* NVIC for USART1 */
    HAL_NVIC_SetPriority(USARTx_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USARTx_IRQn);
}

/***************************************************************************************************
 * @fn      HAL_UART_MspInit()
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
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
    /*##-1- Reset peripherals ##################################################*/
    USARTx_FORCE_RESET();
    USARTx_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks #################################*/
    /* Configure UART Tx as alternate function  */
    HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
    /* Configure UART Rx as alternate function  */
    HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);

    /*##-3- Disable the NVIC for UART ##########################################*/
    HAL_NVIC_DisableIRQ(USARTx_IRQn);
}

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/


/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    crc.h
* @data         2015/11/27
* @auther       yan zeyu
* @module       flash module
* @brief        flash module
***************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CRC_H
#define __CRC_H

/***************************************************************************************************
 * INCLUDES
 */
#include "stm32l0xx_hal.h"

/***************************************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
 *
***************************************************************************************************/
/***************************************************************************************************
 * @fn      crcCheck16()
 *
 * @brief   crc16 check function
 *
 * @author  yan zeyu
 *
 * @param   dataBuffer: data to be checked.
 * 			Length: the number of bytes need to be erased.
 *
 * @return  check result
 */
uint16_t crc16(uint8_t *dataBuffer, uint32_t length);

/***************************************************************************************************
 * @fn      crc32Init()
 *
 * @brief   Initializes the CRC according to the specified
 *          parameters in the CRC_InitTypeDef and creates the associated handle.
 *
 * @author  yan zeyu
 *
 * @param   none
 *
 * @return  state of option
 */
HAL_StatusTypeDef crc32Init(void);

/***************************************************************************************************
 * @fn      crc32Deinit()
 *
 * @brief   Deinitializes the CRC according to the specified
 *          parameters in the CRC_InitTypeDef and creates the associated handle.
 *
 * @author  yan zeyu
 *
 * @param   none
 *
 * @return  state of option
 */
HAL_StatusTypeDef crc32Deinit(void);

/***************************************************************************************************
 * @fn      crcCheck32()
 *
 * @brief   crc16 check function
 *
 * @author  yan zeyu
 *
 * @param   dataBuffer: data to be checked.
 * 			Length: the number of words need to be erased.
 *
 * @return  check result
 */
uint32_t crcCheck32(uint8_t* dataBuffer, uint32_t length);

/***************************************************************************************************
 * @fn      HAL_CRC_MspInit()
 *
 * @brief   This function configures the hardware resources used in this example: 
 *           - Peripheral's clock enable 
 *
 * @author  yan zeyu
 *
 * @param   hcrc: CRC handle pointer
 *
 * @return  none.
 */
void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc);

/***************************************************************************************************
 * @fn      HAL_CRC_MspDeInit()
 *
 * @brief   This function freeze the hardware resources used in this example:
 *          - Disable the Peripheral's clock
 *
 * @author  yan zeyu
 *
 * @param   hcrc: CRC handle pointer
 *
 * @return  none.
 */
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc);

#endif  /* __CRC_H */

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/

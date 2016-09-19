/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    crc.c
* @data         2015/11/27
* @auther       yan zeyu
* @module       flash module
* @brief        flash module
***************************************************************************************************/
/***************************************************************************************************
 * INCLUDES
 */
#include "crc.h"

/*****************************************************************************
 * private variable

*****************************************************************************/
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
uint16_t crc16(uint8_t *dataBuffer, uint32_t length)
{
	uint16_t crc = 0;
	uint32_t i = 0;
 
	while (length--)  //len是所要计算的长度
	{
        crc = crc^(int)(*dataBuffer++) << 8; //    
        for (i=8; i!=0; i--) 
        {
            if (crc & 0x8000)   
                crc = crc << 1 ^ DEFAULT_CRC16_POLY;    
            else
                crc = crc << 1;
		}
    }
	return crc;
}

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
void crc32Init(void)
{
    /* Enable the peripheral clock USART1 */
    RCC->AHBENR |= RCC_AHBENR_CRCEN;
    
    CRC->POL = DEFAULT_CRC32_POLY;          // set CRC polynomial
    CRC->INIT = DEFAULT_CRC32_INITVALUE;    // set CRC init value
    CRC->CR = (CRC->CR & ~(CRC_CR_REV_IN | CRC_CR_REV_OUT)) \
        | (CRC_CR_REV_IN | CRC_CR_REV_OUT); // Bit reversal done by byte, Bit-reversed output format
    CRC->CR = CRC->CR & ~CRC_CR_POLYSIZE;   // 32 bit polynomial
}

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
uint32_t crc32(uint32_t* dataBuffer, uint32_t length)
{
    uint32_t index;
    CRC->CR = CRC->CR | CRC_CR_RESET;   // reset the CRC32 data register
    while(CRC->CR & CRC_CR_RESET) {}    // wait for reset
    for (index = 0; index < length; index++) {
        CRC->DR = dataBuffer[index];
    }
    return CRC->DR ^ 0xffffffff;
}

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/
 
 

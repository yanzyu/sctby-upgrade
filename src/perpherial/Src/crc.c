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
#include "string.h"

/*****************************************************************************
 * private variable

*****************************************************************************/
const static uint16_t POLYNOMIAL16 = 0x1021;

CRC_HandleTypeDef   CrcHandle;

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
                crc = crc << 1 ^ POLYNOMIAL16;    
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
HAL_StatusTypeDef crc32Init(void)
{
    CrcHandle.Instance = CRC; 
  
    CrcHandle.Init.DefaultPolynomialUse    = DEFAULT_POLYNOMIAL_ENABLE;
    CrcHandle.Init.DefaultInitValueUse     = DEFAULT_INIT_VALUE_ENABLE;
    CrcHandle.Init.InputDataInversionMode  = CRC_INPUTDATA_INVERSION_NONE;
    CrcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
    CrcHandle.InputDataFormat              = CRC_INPUTDATA_FORMAT_WORDS;
	
	/* DeInitializes the CRC peripheral */
    HAL_CRC_DeInit(&CrcHandle);
  
    /* Initialise CRC */
    HAL_CRC_Init(&CrcHandle);

    if(HAL_CRC_Init(&CrcHandle) != HAL_OK)
    {
        /* Initialization Error */
        return HAL_ERROR;
    }
	return HAL_OK;
}

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
HAL_StatusTypeDef crc32Deinit(void)
{
	return HAL_CRC_DeInit(&CrcHandle);
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
uint32_t crcCheck32(uint8_t* dataBuffer, uint32_t length)
{
    uint8_t crcArray[256];
    uint32_t crcLength = 0;
    uint8_t temp = 0;
    
    if( length > 256 )
    {
        return 0;
    }else{
        crcLength = (length + 3) & 0xFFFFFFFC;
        memset( crcArray, 0, crcLength );
        memcpy( crcArray, dataBuffer, length );
        
        for( uint32_t i = 0; i < crcLength; i+=4 )
        {
            temp = crcArray[i+0];
            crcArray[i+0] = crcArray[i+3];
            crcArray[i+3] = temp;
            temp = crcArray[i+1];
            crcArray[i+1] = crcArray[i+2];
            crcArray[i+2] = temp;
        }
        

        return HAL_CRC_Calculate(&CrcHandle, (uint32_t*)crcArray, crcLength >> 2);
    }
    
    
}

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
void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc)
{
   /* CRC Peripheral clock enable */
  __HAL_RCC_CRC_CLK_ENABLE();
}

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
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc)
{
  /* Enable CRC reset state */
  __HAL_RCC_CRC_FORCE_RESET();
  
  /* Release CRC from reset state */
  __HAL_RCC_CRC_RELEASE_RESET();
}

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/
 
 

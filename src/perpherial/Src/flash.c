/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    flash.c
* @data         2015/11/27
* @auther       yan zeyu
* @module       flash module
* @brief        flash module
***************************************************************************************************/
/***************************************************************************************************
 * INCLUDES
 */
#include "flash.h"

/***************************************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
 *
***************************************************************************************************/
/***************************************************************************************************
 * @fn      flashPageErase()
 *
 * @brief   erase several pages of flash
 *
 * @author  yan zeyu
 *
 * @param   pageStart: the address of the first page to be erased.
 * 					Length: 	 the number of bytes need to be erased.
 * @return  state of option
 */
FLASH_STATE flashPageErase(uint32_t pageStart, uint32_t length)
{
  FLASH_EraseInitTypeDef    EraseInitStruct;
  uint32_t result        =  FLASHIF_OK;

  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_SIZERR |
                         FLASH_FLAG_OPTVERR | FLASH_FLAG_RDERR | FLASH_FLAG_FWWERR | 
                         FLASH_FLAG_NOTZEROERR);
  HAL_FLASH_Unlock();

  EraseInitStruct.PageAddress  = pageStart;
  EraseInitStruct.TypeErase    = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.NbPages      = length / FLASH_PAGE_SIZE;
    
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &result) != HAL_OK)
      return FLASHIF_ERASE_ERROR;
  
  HAL_FLASH_Lock();

  return FLASHIF_OK;
}	


/***************************************************************************************************
 * @fn      flashPageWrite()
 *
 * @brief   write an word array into flash
 *					flashPageErase() should be called responsiblly before use this function
 *
 * @author  yan zeyu
 *
 * @param   destination: the address to be written into.
 * 					p_source:    word array to be written.
 *          wordsLength: the length of the array.
 * @return  state of option
 */
FLASH_STATE flashWordWrite(uint32_t destination, uint8_t *p_source, uint32_t wordsLength)
{
    uint32_t data    =   0;
    uint32_t i       =   0;
  
    /* unlock the flash */
    HAL_FLASH_Unlock();

    /* write the words to the memory */
    while(i<wordsLength)
    {
        /* transfer 8bit data to 32bit data */
        data = (uint32_t)p_source[i*4] + (uint32_t)(p_source[i*4+1] << 8) +  
            (uint32_t)(p_source[i*4+2] << 16) +  (uint32_t)(p_source[i*4+3] << 24);
        
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, destination + i * 4, data) == HAL_OK)
            i++;
        else
            return FLASHIF_WRITING_ERROR;
    }
    /* lock the flash */
    HAL_FLASH_Lock();

    return FLASHIF_OK;
}	
/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/


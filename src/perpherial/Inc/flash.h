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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_H
#define __FLASH_H

/***************************************************************************************************
 * INCLUDES
 */
#include "stm32l0xx.h"

/* NVM key definitions */
#define FLASH_PDKEY1               ((uint32_t)0x04152637) /*!< Flash power down key1 */
#define FLASH_PDKEY2               ((uint32_t)0xFAFBFCFD) /*!< Flash power down key2: used with FLASH_PDKEY1 
                                                              to unlock the RUN_PD bit in FLASH_ACR */

#define FLASH_PEKEY1               ((uint32_t)0x89ABCDEF) /*!< Flash program erase key1 */
#define FLASH_PEKEY2               ((uint32_t)0x02030405) /*!< Flash program erase key: used with FLASH_PEKEY2
                                                               to unlock the write access to the FLASH_PECR register and
                                                               data EEPROM */

#define FLASH_PRGKEY1              ((uint32_t)0x8C9DAEBF) /*!< Flash program memory key1 */
#define FLASH_PRGKEY2              ((uint32_t)0x13141516) /*!< Flash program memory key2: used with FLASH_PRGKEY2
                                                               to unlock the program memory */

#define FLASH_OPTKEY1              ((uint32_t)0xFBEAD9C8) /*!< Flash option key1 */
#define FLASH_OPTKEY2              ((uint32_t)0x24252627) /*!< Flash option key2: used with FLASH_OPTKEY1 to
                                                              unlock the write access to the option byte block */

/* Error codes used to make the red led blinking */
#define ERROR_ERASE 0x01
#define ERROR_PROG  0x02
#define ERROR_HALF_PROG 0x04
#define ERROR_PROG_FLAG 0x08
#define ERROR_WRITE_PROTECTION 0x10
#define ERROR_FETCH_DURING_ERASE 0x20
#define ERROR_FETCH_DURING_PROG 0x40
#define ERROR_SIZE 0x80
#define ERROR_ALIGNMENT 0x100
#define ERROR_NOT_ZERO 0x200
#define ERROR_UNKNOWN 0x400


#define FLASH_SR_FWWERR ((uint32_t)0x00020000)


/***************************************************************************************************
 * MACRO
 */
#define HALF_PAGE_SIZE                0x40
#define HALF_PAGE_WORD                (HALF_PAGE_SIZE / 4)
 
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
 * 					wordsLength: the number of words need to be erased.
 * @return  state of option
 */
uint16_t flashPageErase(uint32_t pageStart, uint32_t length);

/***************************************************************************************************
 * @fn      flashHalfPageWrite()
 *
 * @brief   write an word array into flash using half-page write
 *			flashPageErase() should be called responsiblly before use this function
 *
 * @author  yan zeyu
 *
 * @param   destination: the address to be written into.
 * 					p_source:    word array to be written.
 *          wordsLength: the length of the array.
 * @return  state of option
 */
uint16_t flashHalfPageWrite(uint32_t flash_addr, uint32_t *data);

uint8_t flashUnlock(void);

uint8_t flashLock(void);

#endif  /* __FLASH_H */


/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/

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

uint16_t flashHalfPageWrite(uint32_t flash_addr, uint32_t *data) {    
    uint8_t i;
    uint16_t error;
    
    flashUnlock();
    /* (1) Set the PROG and FPRG bits in the FLASH_PECR register 
         to enable a half page programming */
    /* (2) Perform the data write (half-word) at the desired address */
    /* (3) Wait until the BSY bit is reset in the FLASH_SR register */
    /* (4) Check the EOP flag in the FLASH_SR register */
    /* (5) clear it by software by writing it at 1 */
    /* (6) Reset the PROG and FPRG bits to disable programming */
    FLASH->PECR |= FLASH_PECR_PROG | FLASH_PECR_FPRG; /* (1) */
    for (i = 0; i < HALF_PAGE_SIZE; i+=4, data++) {
        *(__IO uint32_t*)(flash_addr + i) = *data; /* (2) */
    }
    while ((FLASH->SR & FLASH_SR_BSY) != 0) { /* (3) */
    /* For robust implementation, add here time-out management */
    }  
    if ((FLASH->SR & FLASH_SR_EOP) != 0) { /* (4) */
        FLASH->SR = FLASH_SR_EOP; /* (5) */
    }
    /* Manage the error cases */
    else if ((FLASH->SR & FLASH_SR_FWWERR) != 0) { /* Check Fetch while Write error */
        error |= ERROR_FETCH_DURING_PROG; /* Report the error to the main progran */
        FLASH->SR = FLASH_SR_FWWERR; /* Clear the flag by software by writing it at 1*/
    }
    else if ((FLASH->SR & FLASH_SR_NOTZEROERR) != 0) { /* Check Not Zero error */
        /* This error occurs if the address content was not cleared/erased 
         before the programming */
        error |= ERROR_NOT_ZERO; /* Report the error to the main progran */
        FLASH->SR = FLASH_SR_NOTZEROERR; /* Clear the flag by software by writing it at 1*/
    }
    else if ((FLASH->SR & FLASH_SR_SIZERR) != 0) { /* Check Size error */
        error |= ERROR_SIZE; /* Report the error to the main progran */
        FLASH->SR = FLASH_SR_SIZERR; /* Clear the flag by software by writing it at 1*/
    }
    else if ((FLASH->SR & FLASH_SR_WRPERR) != 0) { /* Check Write protection error */
        error |= ERROR_WRITE_PROTECTION; /* Report the error to the main progran */
        FLASH->SR = FLASH_SR_WRPERR; /* Clear the flag by software by writing it at 1*/
    }
    else if ((FLASH->SR & FLASH_SR_PGAERR) != 0) { /* Check half-page alignment error */
        error |= ERROR_ALIGNMENT; 
        FLASH->SR = FLASH_SR_PGAERR; /* Clear it by software by writing it at 1*/
    }
    else {
        error |= ERROR_UNKNOWN; 
    }
    FLASH->PECR &= ~(FLASH_PECR_PROG | FLASH_PECR_FPRG); /* (6) */
    
    flashLock();
    return error;
}

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
* 2. Modified by yan zeyu @ 20151212
* write the flash using half_page mode.
*
***************************************************************************************************/


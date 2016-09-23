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

static uint8_t RAM_FlashWaitForLastOperation(uint32_t Timeout)
{ 
    /* Wait for the FLASH operation to complete by polling on BUSY flag to be reset.
       Even if the FLASH operation fails, the BUSY flag will be reset and an error
       flag will be set */
       
    while(flashGetFlag(FLASH_SR_BSY) && (Timeout != 0x00)) { 
        Timeout--;
    }
    
    if(Timeout == 0x00 ) {
      return 0;
    }
    
    if( (flashGetFlag(FLASH_SR_RDERR)      != RESET) || 
        (flashGetFlag(FLASH_SR_WRPERR)     != RESET) || 
        (flashGetFlag(FLASH_SR_PGAERR)     != RESET) || 
#if defined(STM32L031xx) || defined(STM32L041xx)
#else
        (flashGetFlag(FLASH_SR_OPTVERR)    != RESET) || 
#endif
        (flashGetFlag(FLASH_SR_SIZERR)     != RESET) || 
        (flashGetFlag(FLASH_SR_FWWERR)     != RESET) || 
        (flashGetFlag(FLASH_SR_NOTZEROERR) != RESET) )
    {

      return 0;
    }
  
    /* If there is an error flag set */
    return 1;
}

uint16_t FlashHalfPageWrite(uint32_t flash_addr, uint32_t *data) {    
    uint32_t count; 
    uint8_t status;

    FlashUnlock();
    /* Wait for last operation to be completed */
    status = RAM_FlashWaitForLastOperation(FLASH_TIMEOUT);

    if(status == 1) {
        /* Proceed to program the new half page */
        SET_BIT(FLASH->PECR, FLASH_PECR_FPRG);
        SET_BIT(FLASH->PECR, FLASH_PECR_PROG);

        count = 0;
        /* Write one half page,
        Address doesn't need to be increased */ 

        /* Disable all IRQs */
        __disable_irq();

        while(count < 16) {
            *(__IO uint32_t*) flash_addr = *data;
            data++;
            count++;
        }

        /* Enable IRQs */
        __enable_irq();
  
        /* Wait for last operation to be completed */
        status = RAM_FlashWaitForLastOperation(FLASH_TIMEOUT);

        /* If the write operation is completed, disable the PROG and FPRG bits */
        CLEAR_BIT(FLASH->PECR, FLASH_PECR_PROG);
        CLEAR_BIT(FLASH->PECR, FLASH_PECR_FPRG);
    }
    FlashLock();
    /* Return the write status */
    return 1;
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


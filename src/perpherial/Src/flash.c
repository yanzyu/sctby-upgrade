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
#include "REG_SysTick.h"

/***************************************************************************************************
 * PRIVATE FUNCTIONS DECLEAR
 *
***************************************************************************************************/

static void flashErase(uint32_t page_addr) {   
    /* Set the ERASE bit */
    SET_BIT(FLASH->PECR, FLASH_PECR_ERASE); 
    /* Set PROG bit */
    SET_BIT(FLASH->PECR, FLASH_PECR_PROG);
    /* Write 00000000h to the first word of the program page to erase */
    *(__IO uint32_t *)page_addr = 0x00000000;
}

static uint8_t FlashWaitOpDone(uint32_t Timeout) {
    uint32_t tickstart = REG_GetTick();   
     
    while(flashGetFlag(FLASH_SR_BSY) != RESET) { 
        if(Timeout != 0xFFFFFFFF) {
            if((Timeout == 0)||((REG_GetTick() - tickstart ) > Timeout)) {
                return 0;
            }
        } 
    }
    /* Check FLASH End of Operation flag  */
    if (flashGetFlag(FLASH_SR_EOP)) {
        /* Clear FLASH End of Operation pending bit */
        FLASH->SR = (FLASH_SR_EOP);
    }
  
#if defined(STM32L031xx) || defined(STM32L041xx)
    if((flashGetFlag(FLASH_SR_WRPERR) != RESET) || (flashGetFlag(FLASH_SR_PGAERR)  != RESET) || \
        (flashGetFlag(FLASH_SR_SIZERR) != RESET) || \
        (flashGetFlag(FLASH_SR_RDERR)  != RESET) || (flashGetFlag(FLASH_SR_FWWERR)  != RESET) || \
        (flashGetFlag(FLASH_SR_NOTZEROERR) != RESET))
#else
    if((flashGetFlag(FLASH_SR_WRPERR) != RESET) || (flashGetFlag(FLASH_SR_PGAERR)  != RESET) || \
        (flashGetFlag(FLASH_SR_SIZERR) != RESET) || (flashGetFlag(FLASH_SR_OPTVERR) != RESET) || \
        (flashGetFlag(FLASH_SR_RDERR)  != RESET) || (flashGetFlag(FLASH_SR_FWWERR)  != RESET) || \
        (flashGetFlag(FLASH_SR_NOTZEROERR) != RESET))
#endif
    {
        return 0;
    }

    /* There is no error flag set */
    return 1;  
}

/***************************************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
 *
***************************************************************************************************/
uint8_t FlashUnlock() {
    if((FLASH->PECR & FLASH_PECR_PRGLOCK) != RESET) {
        /* Unlocking FLASH_PECR register access */
        if((FLASH->PECR & FLASH_PECR_PELOCK) != RESET) {  
            FLASH->PEKEYR = FLASH_PEKEY1;
            FLASH->PEKEYR = FLASH_PEKEY2;
        }
        /* Unlocking the program memory access */
        FLASH->PRGKEYR = FLASH_PRGKEY1;
        FLASH->PRGKEYR = FLASH_PRGKEY2;  
    } else {
        return 0;
    }
    return 1; 
}

uint8_t FlashLock() {
    /* lock the flash */
    SET_BIT(FLASH->PECR, FLASH_PECR_PRGLOCK);
    return 1; 
}

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
uint16_t FlashPageErase(uint32_t pageStart, uint32_t length) {
    uint32_t index = 0;
    uint8_t status;
    FlashUnlock();
    if (FlashWaitOpDone(FLASH_TIMEOUT) == 1) {
        if ((pageStart % FLASH_PAGE_SIZE) != 0) {
            FlashLock();
            return 0;
        }
        /* Erase by sector by sector to be done*/
        for(index = pageStart; index < (length+ pageStart); index += FLASH_PAGE_SIZE) {        
            flashErase(index);

            /* Wait for last operation to be completed */
            status = FlashWaitOpDone(FLASH_TIMEOUT);
            
            /* If the erase operation is completed, disable the ERASE Bit */
            CLEAR_BIT(FLASH->PECR, FLASH_PECR_PROG);
            CLEAR_BIT(FLASH->PECR, FLASH_PECR_ERASE);

            if (status != 1) {
                FlashLock();
                return 0;
            }
        }
    }
    FlashLock();
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


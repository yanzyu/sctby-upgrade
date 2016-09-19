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

volatile uint16_t error;

/***************************************************************************************************
 * PRIVATE FUNCTIONS DECLEAR
 *
***************************************************************************************************/
static uint8_t waitOpDone() {
    /* Wait till no operation is on going */
    while ((FLASH->SR & FLASH_SR_BSY) != 0) /* (1) */  
    {
    /* For robust implementation, add here time-out management */
    }
    return 0;
}

uint8_t flashUnlock() {
    waitOpDone();
    /* unlocks the NVM */
    if((FLASH->PECR & FLASH_PECR_PRGLOCK) != 0) {
        /* Unlocking FLASH_PECR register access */
        if((FLASH->PECR & FLASH_PECR_PELOCK) != 0) {  
            FLASH->PEKEYR = FLASH_PEKEY1;
            FLASH->PEKEYR = FLASH_PEKEY2;
        }
        /* Unlocking the program memory access */
        FLASH->PRGKEYR = FLASH_PRGKEY1;
        FLASH->PRGKEYR = FLASH_PRGKEY2;  
    } else {
        return 1;
    }

    return 0; 
}

uint8_t flashLock() {
    waitOpDone();
    /* lock the flash */
    FLASH->PECR |= FLASH_PECR_PELOCK;
    return 0; 
}

static uint16_t FlashErase(uint32_t page_addr)
{   
    /* (1) Set the ERASE and PROG bits in the FLASH_PECR register 
         to enable page erasing */
    /* (2) Write a 32-bit word value in an address of the selected page 
         to start the erase sequence */
    /* (3) Wait until the BSY bit is reset in the FLASH_SR register */
    /* (4) Check the EOP flag in the FLASH_SR register */
    /* (5) Clear EOP flag by software by writing EOP at 1 */
    /* (6) Reset the ERASE and PROG bits in the FLASH_PECR register 
         to disable the page erase */
    FLASH->PECR |= FLASH_PECR_ERASE | FLASH_PECR_PROG; /* (1) */    
    *(__IO uint32_t *)page_addr = (uint32_t)0; /* (2) */    
    while ((FLASH->SR & FLASH_SR_BSY) != 0) { /* (3) */ 
        /* For robust implementation, add here time-out management */
    }  
    if ((FLASH->SR & FLASH_SR_EOP) != 0) { /* (4) */ 
        FLASH->SR = FLASH_SR_EOP; /* (5)*/
    }  /* Manage the error cases */
    else if ((FLASH->SR & FLASH_SR_FWWERR) != 0) { /* Check Fetch while Write error */
        error |= ERROR_FETCH_DURING_ERASE; /* Report the error to the main progran */
        FLASH->SR = FLASH_SR_FWWERR; /* Clear the flag by software by writing it at 1*/
    } else if ((FLASH->SR & FLASH_SR_SIZERR) != 0) { /* Check Size error */ 
        error |= ERROR_SIZE; /* Report the error to the main progran */
        FLASH->SR = FLASH_SR_SIZERR; /* Clear the flag by software by writing it at 1*/
    } else if ((FLASH->SR & FLASH_SR_WRPERR) != 0) { /* Check Write protection error */
        error |= ERROR_WRITE_PROTECTION; /* Report the error to the main progran */
        FLASH->SR = FLASH_SR_WRPERR; /* Clear the flag by software by writing it at 1*/
    } else {
        //error;
        //error |= ERROR_UNKNOWN; /* Report the error to the main progran */
    }
    FLASH->PECR &= ~(FLASH_PECR_ERASE | FLASH_PECR_PROG); /* (6) */
    return error;
}


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
uint16_t flashPageErase(uint32_t pageStart, uint32_t length) {
    uint32_t i;
    if (length % FLASH_PAGE_SIZE != 0) {
        return ERROR_SIZE;
    }
    flashUnlock();
    
    for (i = 0; i < length / FLASH_PAGE_SIZE; i++) {
        FlashErase(pageStart + i * FLASH_PAGE_SIZE);
    }
    
    flashLock();
    return 0;
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


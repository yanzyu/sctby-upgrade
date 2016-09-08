/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    main.h
* @data         2015/11/30
* @auther       yan zeyu
* @module       main module
* @brief        main module
***************************************************************************************************/
/***************************************************************************************************
 * INCLUDES
 */
#include "main.h"

typedef void(*func)(void);
	
/***************************************************************************************************
 * PRIVATE FUNCTION
 */
/***************************************************************************************************
 * @fn      flashProtectionDisable()
 *
 * @brief   disable the flash section protection.
 *          disable all the section.
 *
 * @author  yan zeyu
 *
 * @param   destination: the address to be written into.
 * 					p_source:    word array to be written.
 *          wordsLength: the length of the array.
 * @return  state of option
 */
#ifdef FLASH_DISABLE
static void flashProtectionDisable(void)
{
    uint32_t ProtectedPages;
    /*Variable used to handle the Options Bytes*/
    FLASH_OBProgramInitTypeDef OptionsBytesStruct;

    /* Unlock the Options Bytes *************************************************/
    HAL_FLASH_OB_Unlock();

    /* Get pages write protection status ****************************************/
    HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);
    
    /* Get pages already write protected ****************************************/
    ProtectedPages = OptionsBytesStruct.WRPSector | FLASH_SECTORS_TO_BE_PROTECTED;
    
    /* Check if there is write protected pages ********************************/
    if((OptionsBytesStruct.WRPSector & FLASH_SECTORS_TO_BE_PROTECTED) == FLASH_SECTORS_TO_BE_PROTECTED)
    {

        /* Restore write protected pages */
        OptionsBytesStruct.OptionType   = OPTIONBYTE_WRP;
        OptionsBytesStruct.WRPState     = OB_WRPSTATE_DISABLE;
        OptionsBytesStruct.WRPSector 	= ProtectedPages;
         
        if(HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
        {
            /* Error occurred while options bytes programming. **********************/
            while (1)
            {		
            }
        }
        /* Generate System Reset to load the new option byte values ***************/
        HAL_FLASH_OB_Launch();
    }  
    
    /* Lock the Options Bytes *************************************************/
    HAL_FLASH_OB_Lock();
}
#endif

#define FUNC_ADDR 0x8000100

int FUCK  __attribute__((section("APP1_Led_Toggle")));

static void LedToggle(void);
void LedToggle(void) {
    BSP_LED_Toggle(LED3);
}

/***************************************************************************************************
 * MAIN FUNCTION
 */
/***************************************************************************************************
 * @fn      main()
 *
 * @brief   main function.
 *          init the hardware, disable the flash protection, process the OTA function.
 *
 * @author  yan zeyu
 *
 * @param   destination: the address to be written into.
 * 					p_source:    word array to be written.
 *          wordsLength: the length of the array.
 * @return  state of option
 */
int main(void)
{
    HAL_Init();

    /* Configure the system clock to have a system clock 2 Mhz */
    clkConfig();
    
    FUCK = 0;

    /* Disable the flash protection */
    //flashProtectionDisable();
  
    /* bootloader process */
    //ota();
    
    BSP_LED_Init(LED3);
 
    
    while (1) {  
        LedToggle();
        HAL_Delay(1000);    
    }
}

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/

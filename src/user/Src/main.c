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
#include "appConfig.h"
#include "clock.h"
#include "led.h"
#include "stm32l0538_discovery.h"


uint32_t timetick;

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
Led_t *led;
moduleDesc_t *desc_t; 
int main(void)
{
    desc_t = (moduleDesc_t*)(DESC_RO_Base);   
    led = (Led_t*)desc_t->module[MODULE_ID_LED].entry;
    
    HAL_Init();
    clkConfig();
        
    led->Init();
    led->On();
    
    timetick = HAL_GetTick();
    while (1) {
        //HAL_Delay(2000);
        if ((HAL_GetTick() - timetick) > 1000) {
            led->Toggle();
            timetick = HAL_GetTick();
        }
    }
}

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/

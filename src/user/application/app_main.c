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
#include "app_main.h"
#include "appConfig.h"
#include "scatterConfig.h"
#include "led.h"
#include "bsw.h"


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

int main(void)
{
    moduleDesc_t *desc_t; 
    Led_t *led;
    Bsw_t *bsw;
    
    desc_t = (moduleDesc_t*)(DESC_RO_Base);    
    led = (Led_t*)desc_t->module[MODULE_ID_LED].entry;
    bsw = (Bsw_t*)desc_t->module[MODULE_ID_BSW].entry;
    
    bsw->HalInit();
    bsw->ClkConfig();
    bsw->Uart1Init();
    
    led->Init();
    
    timetick = bsw->GetTick();
    while (1) {
        if ((bsw->GetTick() - timetick) > 1000) {
            led->Toggle();
            bsw->UartSend("hhhh\r\n", 6);
            timetick = bsw->GetTick();
        }
    }
}

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/
>>>>>>> 7a247bd72c95863e788da847370563d82a923df6:src/user/application/app_main.c

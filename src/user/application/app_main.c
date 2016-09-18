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

const moduleDesc_t *desc_t = (moduleDesc_t*)(DESC_RO_Base);
const Bsw_t *bsw;
const Led_t *led;
uint8_t buf[9];    
  
void delayMs(uint32_t timeout) {
    uint32_t timetick = bsw->GetTick();
    while ((bsw->GetTick() - timetick) < timeout) {
    }
}

uint8_t *num2str(uint32_t n) {
    uint8_t i;
    
    for (i = 0; i < 8; i++) {
        buf[i] = ((n >> 4 * (7 - i)) & 0x0f) + 0x30;
    }
    buf[8] = '\0';
        
    return buf;
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
    uint32_t timetick;
    uint32_t i;
    bsw = (Bsw_t*)desc_t->module[MODULE_ID_BSW].entry;
    led = (Led_t*)desc_t->module[MODULE_ID_LED].entry;
    
    bsw->HalInit();
    bsw->ClkConfig();
    bsw->Uart1Init();
    
    led->Init();

    timetick = bsw->GetTick();
    while((bsw->GetTick() - timetick) < 2000) {
        delayMs(100);
        led->Toggle();
    }
    
    for (i = 0; i < desc_t->moduleCnt; i++) {
        bsw->UartSendString("\r\nmodule version:  \t0x");
        bsw->UartSend(num2str(desc_t->module[i].ver), 9);
        bsw->UartSendString("\r\nmodule crc check:\t0x");
        bsw->UartSend(num2str(desc_t->module[i].crc), 9);
    }
    
    
    while (1) {
        delayMs(1000);
        led->Toggle();
    }
}

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/

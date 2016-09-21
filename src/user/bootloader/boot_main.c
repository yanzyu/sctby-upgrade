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
#include "boot_main.h"
#include "clock.h"
#include "jump.h"
#include "stm32l0538_discovery.h"
#include "uart.h"
#include "crc.h"
#include "ota.h"
#include "flash.h"

void setVer(void);
void setCrc(void);

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
    clkConfig();
    BSP_LED_Init(LED3);
    BSP_LED_Init(LED4);
    InitUart1();
    crc32Init();
     
    ota();

    while (1) {
       
    }
}

uint32_t cnt;


/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/

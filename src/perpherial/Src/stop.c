/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    stop.c
* @data         2015/11/27
* @auther       yan zeyu
* @module       flash module
* @brief        flash module
***************************************************************************************************/
/***************************************************************************************************
 * INCLUDES
 */
#include "stop.h"

/***************************************************************************************************
 * LOCAL VARIABLE
 */
RTC_HandleTypeDef RtcHandle;

/***************************************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
 *
***************************************************************************************************/
/***************************************************************************************************
 * @fn      wakeUpConfig()
 *
 * @brief   Config the RTC wakeup function.
 *
 * @author  yan zeyu
 *
 * @param   none
 *
 * @return  state of option
 */
HAL_StatusTypeDef wakeUpConfig(void)
{
    /* Enable Ultra low power mode */
    HAL_PWREx_EnableUltraLowPower();

    /* Enable the fast wake up from Ultra low power mode */
    HAL_PWREx_EnableFastWakeUp();

    /* Select HSI as system clock source after Wake Up from Stop mode */
    __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);
    
    /* Configure RTC prescaler and RTC data registers */
    /* RTC configured as follow:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */
    RtcHandle.Instance = RTC;
    RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
    RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if(HAL_RTC_Init(&RtcHandle) != HAL_OK)
    /* Initialization Error */
        return HAL_ERROR;

    return HAL_OK;
}

/***************************************************************************************************
 * @fn      wakeUpDisable()
 *
 * @brief   Disable the RTC wakeup function.
 *
 * @author  yan zeyu
 *
 * @param   none
 *
 * @return  state of option
 */
HAL_StatusTypeDef wakeUpDisable(void)
{
	if(HAL_RTCEx_DeactivateWakeUpTimer(&RtcHandle) != HAL_OK)
		return HAL_ERROR;
	return HAL_OK;
}

/***************************************************************************************************
 * @fn      HAL_RTCEx_WakeUpTimerEventCallback()
 *
 * @brief   RTC Wake Up callback
 *
 * @author  yan zeyu
 *
 * @param   none
 *
 * @return  state of option
 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
  /* Clear Wake Up Flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
}

/***************************************************************************************************
 * @fn      setWakeUpTime()
 *
 * @brief   Set the period of wakeup interrupt.
 *
 * @author  yan zeyu
 *
 * @param   period: auto wakeup period
 *
 * @return  state of option
 */
HAL_StatusTypeDef setWakeUpPeriod(uint32_t period)
{
    HAL_RTCEx_DeactivateWakeUpTimer(&RtcHandle);
    
	if(HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, period, RTC_CR_WUCKSEL_2) != HAL_OK)
		return HAL_ERROR;

	return HAL_OK;
}

/***************************************************************************************************
 * @fn      enterStopMode()
 *
 * @brief   make the mcu enter stop mode.
 *
 * @author  yan zeyu
 *
 * @param   none.
 *
 * @return  none.
 */
void enterStopMode(void)
{
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

/***************************************************************************************************
 * @fn      wakeUpTimeIRQHandler()
 *
 * @brief   auto-wakeup interrupt handle.
 *
 * @author  yan zeyu
 *
 * @param   none.
 *
 * @return  none.
 */
void wakeUpTimeIRQHandler(void)
{
	HAL_RTCEx_WakeUpTimerIRQHandler(&RtcHandle);
}

/***************************************************************************************************
 * @fn      HAL_RTC_MspInit()
 *
 * @brief   This function configures the hardware resources used in this example: 
 *           - Peripheral's clock enable
 *           - NVIC configuration for RTC interrupt request enable
 
 * @author  yan zeyu
 *
 * @param   hrtc: RTC handle pointer.
 *
 * @return  none.
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

    /*##-1- Configue LSE or LSI as RTC clock soucre ############################*/ 

    RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
    while(1);
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
    while(1);
    }

    /*##-2- Enable RTC peripheral Clocks #######################################*/ 
    /* Enable RTC Clock */ 
    __HAL_RCC_RTC_ENABLE(); 

    /*##-3- Configure the NVIC for RTC Alarm ###################################*/
    HAL_NVIC_SetPriority(RTC_IRQn, 0x0, 0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
}

/***************************************************************************************************
 * @fn      HAL_RTC_MspDeInit()
 *
 * @brief   This function freeze the hardware resources used in this example:
 *          - Disable the Peripheral's clock
 
 * @author  yan zeyu
 *
 * @param   hrtc: RTC handle pointer.
 *
 * @return  none.
 */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
    /*##-1- Reset peripherals ##################################################*/
    __HAL_RCC_RTC_DISABLE();     
}

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/


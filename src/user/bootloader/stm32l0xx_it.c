/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    processing.c
* @data         2015/11/29
* @auther       yan zeyu
* @module       custom_protocol module
* @brief        custom_protocol module
***************************************************************************************************/
/***************************************************************************************************
 * INCLUDES
 */
#include "stm32l0xx_it.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0+ Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

void WWDG_IRQHandler(void)
{
    /* Check if Early Wakeup Interrupt is enable */
    if (((WWDG->CFR & WWDG_CFR_EWI) == WWDG_CFR_EWI) != RESET) {
    /* Check if WWDG Early Wakeup Interrupt occurred */
        if (((WWDG->SR & WWDG_SR_EWIF) == WWDG_SR_EWIF) != RESET) {
            /* Early Wakeup callback */
            /* Clear the WWDG Data Ready flag */
            WWDG->SR = ~WWDG_SR_EWIF;
        }
    }
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}


/******************************************************************************/
/*                 STM32L0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l0xx.s).                                               */
/******************************************************************************/


extern void DioIsr( uint8_t a_ucId );


extern void sx1278Dio0_Isr( void );
extern void sx1278Dio1_Isr( void );
extern void sx1278Dio2_Isr( void );
extern void sx1278Dio3_Isr( void );
extern void sx1278Dio4_Isr( void );
/**
  * @brief  This function handles EXTI4_15 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_1_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != RESET)
    {
            __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
    #if ( defined( APP_USE_TERMINAL ) && ( APP_MODULE_ON == APP_USE_TERMINAL ) )
            subEXIT0_IRQHandler();
    #endif
    #if ( defined( APP_USE_KEY ) && ( APP_MODULE_ON == APP_USE_KEY ) )
            subEXIT0_IRQHandler();
    #endif		
		
    }

    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_1) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
#if ( defined( APP_USE_DOORSNR ) && ( APP_MODULE_ON == APP_USE_DOORSNR ) )
        subEXIT1_IRQHandler();
#endif
#if ( defined( APP_USE_TESTCASE ) && ( APP_MODULE_ON == APP_USE_TESTCASE ) )
        subEXIT1_IRQHandler();
#endif
#if ( defined( APP_USE_KEY ) && ( APP_MODULE_ON == APP_USE_KEY ) )
        subEXIT1_IRQHandler();
#endif		
		
    }
}

void EXTI2_3_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_2) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
#if ( defined( APP_USE_KEY ) && ( APP_MODULE_ON == APP_USE_KEY ) )
        subEXIT2_IRQHandler();
#endif		
    }
}

/**
  * @brief  This function handles EXTI4_15 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{
    
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_12) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12); 

//            WRITE_REG( GPIOB->BRR, GPIO_PIN_6);
//            DelayMs(500);
//            WRITE_REG( GPIOB->BSRR, GPIO_PIN_6);
        sx1278Dio0_Isr();
    }
    
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_11) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_11);

        sx1278Dio1_Isr();
    }
    
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_10) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_10);

        sx1278Dio2_Isr();
    }
    
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_9) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);

        sx1278Dio3_Isr();
    }
    
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_8) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_8);

        sx1278Dio4_Isr();
    }
    
    if ( __HAL_GPIO_EXTI_GET_IT( GPIO_PIN_7 ) != RESET ){
        __HAL_GPIO_EXTI_CLEAR_IT( GPIO_PIN_7 );

#if ( defined( APP_USE_CURRENTLIMIT ) && ( APP_MODULE_ON == APP_USE_CURRENTLIMIT ) )
        subEXIT7_IRQHandler();
#endif

#if ( defined( APP_USE_WATER_LEVEL ) && ( APP_MODULE_ON == APP_USE_WATER_LEVEL ) )
        subEXIT7_IRQHandler();
#endif

#if ( defined( APP_USE_NTRACKER ) && ( APP_MODULE_ON == APP_USE_NTRACKER ) )
        subEXIT7_IRQHandler();
#endif		
		

    }  
}



/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/


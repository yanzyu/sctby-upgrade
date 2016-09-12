#include "uart.h"
#include "stm32l0xx_hal.h"

__IO uint8_t *pRX, *pTX;
__IO uint32_t RxCnt, RxTick, TxCnt, TxNum;
__IO uint8_t RxFlag, TxFlag;
__IO uint8_t Err;

/**
  * Brief   This function configures USART1.
  * Param   None
  * Retval  None
  */
void InitUart1(void) {
    /* config the GPIO of uart */
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;  // Enable the peripheral clock of GPIOA 
    /* GPIO configuration for USART1 signals */
    /* (1) Select AF mode (10) on PA9 and PA10 */
    GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE9|GPIO_MODER_MODE10))\
        | (GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1);
    /* (2) AF4 for USART1 signals */
    GPIOA->AFR[1] = (GPIOA->AFR[1] &~ (0x00000FF0))\
        | (4 << (1 * 4)) | (4 << (2 * 4));
    
    /* Enable the peripheral clock USART1 */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    /* Configure USART1 */
    /* (1) oversampling by 16, 9600 baud */
    USART1->BRR = 32000000 / UART_BAUD;
    /* (2) 8 data bit, 1 start bit, 1 stop bit, no parity, TX/RX mode, enable RX interrupt */
    USART1->CR1 =  USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

    /* polling idle frame Transmission */
    while((USART1->ISR & USART_ISR_TC) != USART_ISR_TC) { 
    /* add time out here for a robust application */
    }
    USART1->ICR = USART_ICR_TCCF;/* clear TC flag */
    USART1->CR1 |= USART_CR1_TCIE;/* enable TC interrupt */

    /* Configure IT */
    /* (3) Set priority for USART1_IRQn */
    NVIC_SetPriority(USART1_IRQn, 0);
    /* (4) Enable USART1_IRQn */
    NVIC_EnableIRQ(USART1_IRQn);
}

/**
  * Brief   This function send data in interrupt mode.
  * Param   None
  * Retval  None 
  */
uint8_t uartSend_IT(uint8_t *buf, uint32_t n) {
    TxCnt = 0;
    TxNum = n;
    TxFlag = 1;
    Err = 0;
    pTX = buf;
    // start the uart transmit
    USART1->TDR = buf[TxCnt++];
    
    while(TxFlag) { }   // wait
    return Err;
}

/**
  * Brief   This function receive data in interrupt mode.
  * Param   None
  * Retval  None
  */
uint32_t uartReceive_Pkt(uint8_t *buf, uint32_t timeout) {
    uint32_t startTick, timeTick;
    
    RxCnt = 0;
    RxFlag = 1;
    pRX = buf;
    startTick = HAL_GetTick();
    
    while (1) {
        timeTick = HAL_GetTick();
        if (((timeTick - startTick) > timeout)) {
            break;
        } else if (((RxCnt > 0) && ((timeTick - RxTick) > 3) && ((timeTick - RxTick) < 1000))) {
            break;
        }
    }
    RxFlag = 0;
    return RxCnt;    
}

/**
  * Brief   This function handles USART1 interrupt request.
  * Param   None
  * Retval  None
  */
void USART1_IRQHandler(void) {
    if((USART1->ISR & USART_ISR_TC) == USART_ISR_TC) {
        if (TxCnt == TxNum) {
            TxFlag = 0;
            USART1->ICR = USART_ICR_TCCF; /* Clear transfer complete flag */
            pTX = NULL;
        } else {
            /* clear transfer complete flag and fill TDR with a new char */
            USART1->TDR = pTX[TxCnt++];
        }
    } else if ((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE) {
        if (RxFlag) {
            pRX[RxCnt++] = (uint8_t)(USART1->RDR);
            RxTick = HAL_GetTick();
        }
    } else {
        /* Report an error */
        TxFlag = 0;
        Err = 1;
        NVIC_DisableIRQ(USART1_IRQn); /* Disable USART1_IRQn */
    }
}





#include "appConfig.h"
#include "scatterConfig.h"
#include "led.h"
#include "stm32l0xx.h"

/****************************************************************************
* PUBLIC FUNCTION DECLARATION
*
****************************************************************************/
void ledInit(void);
void ledOn(void);
void ledOff(void);
void ledToggle(void);
void ledBlink(void);

const Led_t Led __attribute__((at(LED_RO_Base))) = {
    ledInit,
    ledOn,
    ledOff,
    ledToggle,
};



/****************************************************************************
* PUBLIC FUNCTION DEFINATION
*
****************************************************************************/
void ledInit() {
/* (1) Enable the peripheral clock of GPIOB */
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    /* (2) Set output mode, PB4 */
    GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE4)) | (GPIO_MODER_MODE4_0);
    /* (3) Set PULLUP mode */
    GPIOB->PUPDR = (GPIOB->PUPDR & ~(GPIO_PUPDR_PUPD4)) | (GPIO_PUPDR_PUPD4_0);
    /* (4) Set FAST mode */
    GPIOB->OSPEEDR = (GPIOB->OSPEEDR | GPIO_OSPEEDER_OSPEED4);
    /* (5) Set push-pull mode, PB4 */
    GPIOB->OTYPER = (GPIOB->OTYPER & ~(GPIO_OTYPER_OT_4));
}

void ledOn() {
    GPIOB->BSRR = LED_PIN;
}

void ledOff() {
    GPIOB->BRR = LED_PIN;
}

void ledToggle() {
    GPIOB->ODR ^= LED_PIN;
}




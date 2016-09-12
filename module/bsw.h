#ifndef __BSW_H
#define __BSW_H

#include "crc.h"
#include "uart.h"
#include "stop.h"
#include "jump.h"
#include "clock.h"
#include "flash.h"
#include "stm32l0xx_hal.h"

typedef struct {
    /* hal interface */
    void (*HalInit)(void);
    
    /* clock interface */
    void (*ClkConfig)(void);
    
    /* systick interface */
    uint32_t (*GetTick)(void);
    
    /* uart interface */
    void (*Uart1Init)(void);
    uint8_t (*UartSend)(uint8_t *buf, uint32_t n);
    uint32_t (*UartReceive)(uint8_t *buf, uint32_t timeout);
  
} Bsw_t;    // led type defination



#endif


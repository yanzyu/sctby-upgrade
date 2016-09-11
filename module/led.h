
#ifndef __LED_H
#define __LED_H

#define LED_PIN ((uint16_t)0x0010)  /* Pin 4 selected    */


/* interface function table declaration */

typedef struct {
    void (*Init)(void);           // open led
    void (*On)(void);           // open led
    void (*Off)(void);          // close led
    void (*Toggle)(void);       // toggle led
} Led_t;    // led type defination


#endif

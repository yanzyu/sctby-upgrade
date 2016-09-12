/*************************************************
* This program is used to check whether two file
* is the same.
*************************************************/
#ifndef __STACK_H
#define __STACK_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define REALLOC_COEF 2
#define BUFFER_SIZE  128

typedef enum {
    FALSE = 0,
    TRUE = !FALSE
} BOOL;

typedef struct { 
    uint8_t     *Data;
    uint32_t    Size;
    uint32_t    Volum;
} Stack_t;

BOOL StackInit(Stack_t *stack, uint32_t size );
void StackClose(Stack_t *stack);
BOOL StackPush(Stack_t *stack, uint8_t *buf, uint32_t len);
uint8_t StackPop(Stack_t *stack);

#endif


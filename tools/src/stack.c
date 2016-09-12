/*************************************************
* This program is used to check whether two file
* is the same.
*************************************************/
#include "stack.h"

BOOL StackInit(Stack_t *stack, uint32_t size) {
    stack->Size = 0;
    stack->Volum = size;
    stack->Data = (uint8_t*)malloc(size * sizeof(stack->Data[0]));
    if (stack->Data == NULL) {
        return FALSE;
    }
    return TRUE;
}

void StackClose(Stack_t *stack) {
    stack->Size = 0;
    stack->Volum = 0;
    if (stack->Data != NULL) {
        free(stack->Data);
    }
}

BOOL StackPush(Stack_t *stack, uint8_t *buf, uint32_t len) {
    uint32_t index;
    if ((stack->Size + len) > stack->Volum) {
        stack->Data = (uint8_t*)realloc(stack->Data, REALLOC_COEF * stack->Volum * sizeof(stack->Data[0]));
        if (stack->Data == NULL) {
            printf("failed to realloc memory\n");
            return FALSE;
        }
        stack->Volum *= REALLOC_COEF; 
    }
    for (index = 0; index < len; index++) {
        stack->Data[stack->Size++] = buf[index];
    }
    
    return TRUE;
}

uint8_t StackPop(Stack_t *stack) {
    if (stack->Size > 0) {
        return stack->Data[--stack->Size];
    }
    return 0;    
}



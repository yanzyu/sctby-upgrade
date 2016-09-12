/*************************************************
* This program is used to check whether two file
* is the same.
*************************************************/
#include "stack.h"

BOOL StackInit(Stack_t *stack, uint32_t size) {
    stack->Size = 0;
    stack->Volum = 0;
    stack->Data = (uint8_t*)malloc(size * sizeof(stack->Data[0]));
    if (stack->Data == NULL) {
        return FALSE;
    }
    return TRUE;
}

void StackClose(Stack_t *stack) {
    if (stack->Data != NULL) {
        free(stack->Data);
    }
}

BOOL StackPush(Stack_t *stack, uint8_t data) {
    if (stack->Size == stack->Volum-1) {
        stack->Data = (uint8_t*)realloc(stack->Data, sizeof(stack->Data[0]) * stack->Volum * 2);
        if (stack->Data == NULL) {
            return FALSE;
        }
        stack->Volum *= REALLOC_COEF; 
    }
    stack->Data[stack->Size++] = data;
    return TRUE;
}

uint8_t StackPop(Stack_t *stack) {
    if (stack->Size > 0) {
        return stack->Data[--stack->Size];
    }
    return 0;    
}



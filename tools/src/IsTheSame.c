/*************************************************
* This program is used to check whether two file
* is the same.
*************************************************/
#include <stdio.h>
#include "stack.h"

BOOL loadFile(char* fileName, Stack_t *stack);
BOOL checkFile(Stack_t *stackA, Stack_t *stackB);

int main(int argc, char *argv[]) {

    Stack_t stackA, stackB;
    int i;

    if (argc < 3) {
        printf("[ERROR]:please enter file name");
        return -1;
    }

    if ((StackInit(&stackA, 0x10000) != TRUE) || (StackInit(&stackB, 0x10000) != TRUE)) {
        printf("[ERROR]:failed to init the stack\n");
        return -2;
    }

    if ((loadFile(argv[1], &stackA) != TRUE) || (loadFile(argv[2], &stackB) != TRUE)) {
        printf("[ERROR]:failed to load file A\n");
        return -3;
    }
    
    if (checkFile(&stackA, &stackB) != TRUE) {
        printf("[WARNING]:file is not the same\n");
    } else {
        printf("[NOTING]:file is the same\n");
    }
    
    StackClose(&stackA);
    StackClose(&stackB);
    return 0;
}


BOOL loadFile(char* fileName, Stack_t *stack) {
    FILE *pFile;
    uint8_t buf[BUFFER_SIZE];
    uint32_t count;
    if (fileName == NULL) {
        printf("no file\n");
        return FALSE;
    }
    pFile = fopen(fileName, "rb");
    if (pFile == NULL) {
        printf("can not open file\n");
        return FALSE;
    }

    while ((count = fread(buf, 1, BUFFER_SIZE, pFile))) {
        if (StackPush(stack, buf, count) != TRUE) {
            printf("failed to save in stack\n");
            return FALSE;
        }
    }
    return TRUE;
}


BOOL checkFile(Stack_t *stackA, Stack_t *stackB) {
    uint32_t index;
    if (stackA->Size != stackB->Size) {
        return FALSE;
    }
    for (index = 0; index < stackA->Size; index++) {
        if (stackA->Data[index] != stackB->Data[index]) {
            return FALSE;
        }
    }
    return TRUE;
}



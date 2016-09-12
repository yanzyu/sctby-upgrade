/*************************************************
* This program is used to check whether two file
* is the same.
*************************************************/
#include <stdio.h>
#include "stack.h"

BOOL loadFile(char* fileName, Stack_t *stack);

int main(int argc, char *argv[]) {

    Stack_t stackA, stackB;
    int i;

    if (argc < 2) {
        printf("please enter file name");
    }

    if (StackInit(&stackA, 0x10000) != TRUE) {
        printf("[ERROR]:failed to init the stack\n");
        return -1;
    }

    if (loadFile(argv[1], &stackA) != TRUE) {
        printf("failed to load file A");
    }

    // // for test
    // while(stackA.Size > 0) {
    //     printf("%c", StackPop(&stackA));
    // }

    StackClose(&stackA);
    return 0;
}


BOOL loadFile(char* fileName, Stack_t *stack) {
    FILE *pFile;
    uint8_t data;
    if (fileName == NULL) {
        return FALSE;
    }
    pFile = fopen(fileName, "r");
    if (pFile == NULL) {
        return FALSE;
    }

    while ((data = fgetc(pFile)) != EOF) {
        // if (StackPush(stack, data) != TRUE) {
        //     return FALSE;
        // }
    }
    return TRUE;
}



#include "crc32.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct{
    unsigned int entry;
    unsigned int size;
    unsigned int ver;   /* use only byte 2 = major, byte 3 = minor */
//    unsigned int crc; /* ËÆºõÃ»ÓÐ°ì·¨ÔÚÕâÀï¼ÓÈëcrcÐ£ÑéÖµ£¬ÒòÎª³ÌÐò¸´Î»ºó£¬ÔÚ³õÊ¼»¯½×¶ÎÓ¦¸Ã»á½«Õâ¸öÖµ³õÊ¼»¯ */
} ModuleInfo_t;  // module info type
    
typedef struct{
    unsigned int firmVer;
    unsigned int moduleEntry;
    unsigned int moduleSize;
    unsigned int moduleCrc32;
    unsigned int moduleCnt;
    ModuleInfo_t *module;
} ModuleDesc_t;  // module description type

uint8_t *readData(char *fileName, uint32_t *size) {
    FILE *pFile;
    int fileSize;
    uint8_t *data;
    int n;
    int i;

    pFile = fopen(fileName, "rb");
    if (pFile == NULL) {
        printf("can not open file: %s\n", fileName);
        *size = 0;
        return NULL;
    }
    fseek(pFile, 0L, SEEK_END);
    fileSize = ftell(pFile);
    fseek(pFile, 0L, SEEK_SET);

    data = (uint8_t*)malloc(fileSize);
    if (data == NULL) {
        printf("failed to alloc memory.\n");
        fclose(pFile);
        return NULL;
    }
    n = fread(data, 1, fileSize, pFile);

    if (n == fileSize) {
        *size = fileSize;
        fclose(pFile);
        return data;
    }
   
    *size = 0;
    fclose(pFile);
    return NULL;
}

uint32_t writeData(char *fileName, const uint8_t *data, uint32_t size) {
    FILE *pFile;
    uint32_t n;
        
    pFile = fopen(fileName, "wb+");
    if (pFile == NULL) {
        printf("can not open file: %s\n", fileName);
        return NULL;
    }

    n = fwrite(data, 1, size, pFile);
    fclose(pFile);
    return n;
}

int main(int argc, char *argv[]) {
    uint8_t *data;
    ModuleDesc_t *desc;
    uint32_t crc32, size, desc_size;
       
    if (argc < 3) {
        printf("please enter the FULL_EROM file name\n");
        return 0;
    }

    data = readData(argv[1], &size);
    if ((data == NULL) || (size == 0 )) {
        return 0;
    }
    desc = (ModuleDesc_t*)data;
    crc32 = Crc32IEEE((uint8_t*)&(desc->moduleCnt), size - ((uint8_t*)&(desc->moduleCnt) - (uint8_t*)desc));
    desc->moduleSize = size - ((uint8_t*)&(desc->moduleCnt) - (uint8_t*)desc);
    desc->moduleCrc32 = crc32;
    writeData(argv[1], data, size);
    free(data);

    data = readData(argv[2], &desc_size);
    if ((data == NULL) || (desc_size == 0 )) {
        return 0;
    }
    desc = (ModuleDesc_t*)data;
    desc->moduleSize = size - ((uint8_t*)&(desc->moduleCnt) - (uint8_t*)desc);
    desc->moduleCrc32 = crc32;
    writeData(argv[2], data, desc_size);
    free(data);

    return 0;
}



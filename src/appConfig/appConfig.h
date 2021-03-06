
#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

/******************************************************************************
* defination
* 
******************************************************************************/
#define VER_MAJOR       (0x0001)
#define VER_MINOR       (0x0000)

#define FIRM_UNCHECK    (0xFFFFFFFF)

typedef struct{
    unsigned int entry;
    unsigned int size;
    unsigned int ver;   /* use only byte 2 = major, byte 3 = minor */
//    unsigned int crc; /* 似乎没有办法在这里加入crc校验值，因为程序复位后，在初始化阶段应该会将这个值初始化 */
} ModuleInfo_t;  // module info type
    
typedef struct{
    unsigned int firmVer;
    unsigned int firmEntry;
    unsigned int firmSize;
    unsigned int firmCrc32;
    unsigned int moduleCnt;
    ModuleInfo_t *module;
} ModuleDesc_t;  // module description type

/******************************************************************************
* module declaration
* step1: add module name here
* step2: add module section parameter
* step3: add the module in appConfig.c source file
******************************************************************************/
enum{
    /* INIT and DESC module */
    MODULE_ID_DESC = 0,
    MODULE_ID_INIT,
    
    /* normal module */
    MODULE_ID_BSW,
    MODULE_ID_LED,
    
    /* module counter */
    MODULE_ID_COUNT
};


#endif  //__APP_CONFIG_H

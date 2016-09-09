
#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H

/******************************************************************************
* defination
* 
******************************************************************************/
typedef struct{
    unsigned int entry;
    unsigned int size;
    unsigned int ver;   /* use only byte 2 = major, byte 3 = minor */
    unsigned int crc;
}moduleInfo_t;  // module info type

typedef struct{
    unsigned int moduleCnt;
    moduleInfo_t *module;
}moduleDesc_t;  // module description type


/******************************************************************************
* module declaration
* step1: add module name here
* step2: add module section parameter
* step3: add the module in appConfig.c source file
******************************************************************************/
enum{
    /* INIT and DESC module */
    MODULE_ID_INIT = 0,
    MODULE_ID_DESC,
    /* normal module */
    MODULE_ID_BSW,
    MODULE_ID_LED,
    
    /* module counter */
    MODULE_ID_COUNT
};


/******************************************************************************
* bootloader address base and application address base

******************************************************************************/
#define BOOT_BASE               0x08000000
#define APP_BASE                0x08003000
#define APP_SIZE                0x10000
#define APP_END                 (APP_BASE + APP_SIZE)

#if APP_BASE < BOOT_BASE
    #error "[BASE ERROR]: app_base is smaller than boot_base"
#endif
#if (((APP_BASE - BOOT_BASE) % 0x200) != 0)
    #error "[BASE ERROR]: the difference between boot_base and app_base is not the multiple of 0x200"
#endif


/******************************************************************************
* module name:          INIT
* module number:        -1
* module version:       0x01
* module description:   application entry, init the app

******************************************************************************/
/* module section address define */
#define INIT_Base       0x8003000
#define INIT_End        0x8003200
#define INIT_Size       (INIT_End - INIT_Base)
#if ((INIT_Size % 0x100) != 0)
    #error "[INIT BASE ERROR]: size is not the multiple of 0x200"
#endif
#if (INIT_End > APP_END)
    #error "[INIT END ERROR]: beyond the size limit of application"
#endif

/* module version */
#define INIT_Ver_MAJOR   (0x00)
#define INIT_Ver_MINOR   (0x01)


/******************************************************************************
* module name:          DESC
* module number:        0
* module version:       0x01
* module description:   containing the module description

******************************************************************************/
/* module section address define */
#define DESC_Base     0x8003200
#define DESC_End      0x8003400
#define DESC_Size     (DESC_End - DESC_Base)
#if (DESC_BASE < INIT_END)
    #error "[DESC BASE ERROR]: desc_base is smaller than init_end"
#endif
#if (DESC_End > APP_END)
    #error "[INIT END ERROR]: beyond the size limit of application"
#endif
#if ((DESC_Size % 0x100) != 0)
    #error "[DESC BASE ERROR]: size is not the multiple of 0x200"
#endif

/* module version */
#define DESC_Ver_MAJOR   (0x00)
#define DESC_Ver_MINOR   (0x01)


/******************************************************************************
* module name:          led toggle
* module number:        1
* module version:       0x01
* module description:   for test

******************************************************************************/
/* module section address define */
#define Module1_Base     0x8003000
#define Module1_End      0x8003200
#define Module1_Size     (Module1_End - Module1_Base)
#if ((Module1_Size % 0x100) != 0)
    #error "[Module1 BASE ERROR]: size is not the multiple of 0x200"
#endif

/* module version */
#define Module1_Ver_MAJOR   (0x00)
#define Module1_Ver_MINOR   (0x01)




#endif  //__APP_CONFIG_H

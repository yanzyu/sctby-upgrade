
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
//    unsigned int crc; /* 似乎没有办法在这里加入crc校验值，因为程序复位后，在初始化阶段应该会将这个值初始化 */
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
#define APP_BASE                0x08000000
#define APP_SIZE                0x7000
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
#define INIT_RO_Base       0x08000000
#define INIT_RO_End        0x08000200
#define INIT_RW_Base       0x20000000
#define INIT_RW_End        0x20000000
#define INIT_RO_Size       (INIT_RO_End - INIT_RO_Base)
#define INIT_RW_Size       (INIT_RW_End - INIT_RW_Base)

#if ((INIT_RO_Size % 0x100) != 0)
    #error "[INIT BASE ERROR]: size is not the multiple of 0x200"
#endif
#if (INIT_RO_Size > APP_END)
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
#define DESC_RO_Base     0x08000200
#define DESC_RO_End      0x08000300
#define DESC_RW_Base     0x20000000
#define DESC_RW_End      0x20000000
#define DESC_RO_Size     (DESC_RO_End - DESC_RO_Base)
#define DESC_RW_Size     (DESC_RW_End - DESC_RW_Base)
#if (DESC_RO_Base < INIT_END)
    #error "[DESC BASE ERROR]: desc_base is smaller than init_end"
#endif
#if (DESC_RO_End > APP_END)
    #error "[INIT END ERROR]: beyond the size limit of application"
#endif
#if ((DESC_RO_Size % 0x100) != 0)
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
#define BSW_RO_Base     0x08000400
#define BSW_RO_End      0x08001400
#define BSW_RW_Base     0x20000000
#define BSW_RW_End      0x20001000
#define BSW_RO_Size     (BSW_RO_End - BSW_RO_Base)
#define BSW_RW_Size     (BSW_RW_End - BSW_RW_Base)
#if ((BSW_RO_Size % 0x100) != 0)
    #error "[BSW BASE ERROR]: size is not the multiple of 0x200"
#endif

/* module version */
#define BSW_Ver_MAJOR   (0x00)
#define BSW_Ver_MINOR   (0x01)


/******************************************************************************
* module name:          led toggle
* module number:        1
* module version:       0x01
* module description:   for test

******************************************************************************/
/* module section address define */
#define LED_RO_Base     0x08000300
#define LED_RO_End      0x08000400
#define LED_RW_Base     0x20000000
#define LED_RW_End      0x20000000
#define LED_RO_Size     (LED_RO_End - LED_RO_Base)
#define LED_RW_Size     (LED_RW_End - LED_RW_Base)
#if ((LED_RO_Size % 0x100) != 0)
    #error "[LED BASE ERROR]: size is not the multiple of 0x200"
#endif

/* module version */
#define LED_Ver_MAJOR   (0x00)
#define LED_Ver_MINOR   (0x01)




#endif  //__APP_CONFIG_H

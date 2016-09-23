
#ifndef __SCATTER_CONFIG_H
#define __SCATTER_CONFIG_H


/******************************************************************************
* bootloader address base and application address base

******************************************************************************/
#define BOOT_BASE               0x08000000
#define APP_BASE                0x08005000
#define APP_RO_BASE             APP_BASE
#define APP_RW_BASE             0x20000000
#define APP_RO_SIZE             0x7000
#define APP_RW_SIZE             0x2000
#define APP_RO_END              (APP_RO_BASE + APP_RO_SIZE)
#define APP_RW_END              (APP_RW_BASE + APP_RW_SIZE)

#if APP_RO_BASE < BOOT_BASE
    #error "[BASE ERROR]: app_base is smaller than boot_base"
#endif
#if (((APP_RO_BASE - BOOT_BASE) % 0x200) != 0)
    #error "[BASE ERROR]: the difference between boot_base and app_base is not the multiple of 0x200"
#endif


/******************************************************************************
* module name:          INIT
* module number:        -1
* module version:       0x01
* module description:   application entry, init the app

******************************************************************************/
/* module section size define, alloc by user */
#define INIT_RO_SIZE       0x300
#define INIT_RW_SIZE       0x100

/* do not modify */
#define INIT_RO_BASE       APP_RO_BASE
#define INIT_RW_BASE       APP_RW_BASE
#define INIT_RO_END        (INIT_RO_BASE + INIT_RO_SIZE)
#define INIT_RW_END        (INIT_RW_BASE + INIT_RW_SIZE)

/* module version */
#define INIT_Ver_MAJOR   (0x00)
#define INIT_Ver_MINOR   (0x01)


/******************************************************************************
* module name:          DESC
* module number:        0
* module version:       0x01
* module description:   containing the module description

******************************************************************************/
/* module section size define, alloc by user */
#define DESC_RO_SIZE       0x100
#define DESC_RW_SIZE       0x0

/* do not modify */
#define DESC_RO_BASE       INIT_RO_END
#define DESC_RW_BASE       INIT_RW_END
#define DESC_RO_END        (DESC_RO_BASE + DESC_RO_SIZE)
#define DESC_RW_END        (DESC_RW_BASE + DESC_RW_SIZE)

/* module version */
#define DESC_Ver_MAJOR   (0x00)
#define DESC_Ver_MINOR   (0x01)


/******************************************************************************
* module name:          led toggle
* module number:        1
* module version:       0x01
* module description:   for test

******************************************************************************/
/* module section size define, alloc by user */
#define BSW_RO_SIZE     0x1000
#define BSW_RW_SIZE     0xF00

/* do not modify */
#define BSW_RO_BASE     DESC_RO_END
#define BSW_RW_BASE     DESC_RW_END
#define BSW_RO_END      (BSW_RO_BASE + BSW_RO_SIZE)
#define BSW_RW_END      (BSW_RW_BASE + BSW_RW_SIZE)

/* module version */
#define BSW_Ver_MAJOR   (0x00)
#define BSW_Ver_MINOR   (0x01)


/******************************************************************************
* module name:          led toggle
* module number:        1
* module version:       0x01
* module description:   for test

******************************************************************************/
/* module section size define, alloc by user */
#define LED_RO_SIZE     0x100
#define LED_RW_SIZE     0x0

/* do not modify */
#define LED_RO_BASE     BSW_RO_END
#define LED_RW_BASE     BSW_RW_END
#define LED_RO_END      (LED_RO_BASE + LED_RO_SIZE)
#define LED_RW_END      (LED_RW_BASE + LED_RW_SIZE)

/* module version */
#define LED_Ver_MAJOR   (0x00)
#define LED_Ver_MINOR   (0x01)



#endif  //__APP_CONFIG_H

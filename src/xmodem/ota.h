/***************************************************************************************************
 * OTA module of LoRa netword
 * receive eidt table file using XMODEM protocal
 * rebuild the firmware during receiving the table
 * version: 0.2
 * time: 2016/7/29
 * document: receive the whole packet at once, and decode the packet layer after layer
***************************************************************************************************/
#ifndef __OTA_H
#define __OTA_H

#include "stm32l0xx.h"

/***************************************************************************************************
 * MACRO
***************************************************************************************************/
#define APP_ENTRY       0x08003000
#define DEV_EUI_LEN     8
#define FIRM_VER_LEN    4

// frame head
#define FRAME_HEAD          0xAA
#define FRAME_DEV_STA_LEN   5
#define FRAME_GW_RESP_LEN   13  // maxium length
#define FRAME_XMODEM_LEN    133 // maxium length

/***************************************************************************************************
 * DATA STRUCTURE
***************************************************************************************************/
// frame type
typedef enum {
    FRAME_WRONG   =  0x00,
    FRAME_DEV_STA =  0x01,
    FRAME_GW_RESP =  0x02,
    FRAME_XMODEM  =  0x03    
} Frame_t;

// device state code
typedef enum {
    DEV_STATE_OK  =  0x01,
    DEV_TRANS_ERR =  0x02,
    DEV_FUNC_ERR  =  0x03
} DevState_t;

// gateway response code
typedef enum {
    DEV_UPGRADE   =  0x01,
    DEV_JUMP      =  0x02,
    DEV_SLEEP     =  0x03
} GwResp_t;


typedef struct{
    unsigned int entry;
    unsigned int size;
    unsigned int ver;   /* use only byte 2 = major, byte 3 = minor */
    unsigned int crc; /* 似乎没有办法在这里加入crc校验值，因为程序复位后，在初始化阶段应该会将这个值初始化 */
}moduleInfo_t;  // module info type
    
typedef struct{
    unsigned int firmVer;
    unsigned int moduleCnt;
    moduleInfo_t *module;
}moduleDesc_t;  // module description type

/***************************************************************************************************
 * PUBLIC FUNCTION
***************************************************************************************************/
/***************************************************************************************************
 * Name: XmodemReceive()
 * Doc: run one xmodem transmission.
 * Input: 
 * Output: 
*/
void ota(void);

#endif

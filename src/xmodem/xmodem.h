/***************************************************************************************************
 * OTA module of LoRa netword
 * receive eidt table file using XMODEM protocal
 * rebuild the firmware during receiving the table
 * version: 0.2
 * time: 2016/7/29
 * document: receive the whole packet at once, and decode the packet layer after layer
***************************************************************************************************/
#ifndef __XMODEM_H
#define __XMODEM_H

#include "stm32l0xx.h"

/***************************************************************************************************
 * MACRO
***************************************************************************************************/

/***************************************************************************************************
 * DATA STRUCTURE
***************************************************************************************************/
typedef enum {
    xFalse = 0,
    xTrue = !xFalse
} XmodemBool;

typedef uint8_t (*XmodemRead)(uint8_t *pBuf, uint32_t timeout);
typedef void    (*XmodemWrite)(uint8_t *pBuf, uint8_t len);
typedef XmodemBool (*XmodemCallBack)(void);

// OtaState shows the result of OTA function
typedef enum {
    XmodemOK,   // operation ok state
    XmodemEnd,
    XmodemFalid,

    XmodemWrongPacket,
    XmodemDuplicatePacket
} XmodemState_t;

// Xmodem module init config
typedef struct {
    uint32_t    RtNum;          // Retransmission times number
    uint32_t    TimeOut;        // Receive TimeOut
    XmodemRead  Read;
    XmodemWrite Write;
    XmodemCallBack CallBack;    // Operation after receiving a xmodem packet
} XmodemInit_t;

/***************************************************************************************************
 * PUBLIC FUNCTION
***************************************************************************************************/
/***************************************************************************************************
 * Name: XmodemReceive()
 * Doc: run one xmodem transmission.
 * Input: 
 * Output: 
*/
XmodemState_t XmodemReceive(const XmodemInit_t *InitStruct);

#endif

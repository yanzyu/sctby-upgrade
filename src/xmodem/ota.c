/***************************************************************************************************
 * OTA module of LoRa netword
 * receive eidt table file using XMODEM protocal
 * rebuild the firmware during receiving the table
***************************************************************************************************/
/***************************************************************************************************
 * INCLUDE
***************************************************************************************************/
#include "ota.h"
#include "xmodem.h"
#include "flash.h"
#include "crc.h"
#include "jump.h"

/***************************************************************************************************
 * MACRO
***************************************************************************************************/
// RF packet define
#define RF_HeadSize  0
#define RF_MinSize   5
#define RF_MaxSize   256


// flash address
#define EXTRA_FLASH_ADDR      0x8008000 // the start address of the extra address for rebuilding the firmware
#define EXTRA_FLASH_SIZE      0x5000    // the space of flash reversed for rebuilding the firmware
#define FIRMWARE_ADDR         0x8003000 // the run-time firmware start address
#define FIRMWARE_SIZE         0x5000    // the space of flash reversed for the firmware

// ota request and response size
#define OTA_REQUEST_SIZE      5         // ota request command size, 5 bytes
#define OTA_RESPONSE_SIZE     13        // ota response command size, 13 bytes
#define OTA_REQUEST_RT        5         // ota request retransmission times
#define OTA_CMD_FLAG          1

/***************************************************************************************************
 * DATA STRUCTURE
***************************************************************************************************/
// boolean define
typedef enum {
    oFalse = 0,
    oTrue  = !oFalse
} OtaBool;

typedef enum {
    OtaOK,
    OtaEnd,
    OtaNoResp
} OtaState_t;

/***************************************************************************************************
 * GLOBAL VARIABLE
***************************************************************************************************/
// show the xmodem state
static OtaBool IsOtaStart;
static uint32_t MaxOtaTime = 0xff000;
static uint32_t pFirm;    // 指向新固件当前的地址
static uint8_t RxTxBuf[RF_MaxSize];

static uint8_t ModuleID;
static uint8_t ModuleVer;
static uint8_t ModuleCrc;

/***************************************************************************************************
 * PRIVATE FUNCTION DECLARATION
***************************************************************************************************/
static void initOta(void);
static uint8_t otaRead(uint8_t *pBuf, uint32_t timeOut);   
static void otaWrite(uint8_t *pBuf, uint8_t length);    // maxium length is 255
static OtaState_t requestOta(void);

/***************************************************************************************************
 * PUBLIC FUNCTION
***************************************************************************************************/
/***************************************************************************************************
 * Name: ota()
 * Doc: receiving the update edit table, rebuild and program the new firmware
 * Input: none
 * Output: OTA state
*/
void ota(void){
    OtaState_t      oState;
    XmodemState_t   xState;
    XmodemInit_t    xInitStrcut;
    
    //uint32_t startTick = HAL_GetTick();
    
    initOta();  // init the variables using in ota

    while (oTrue) {
    
        oState = requestOta();   // receive the module information in this step
        
        if (oState == OtaNoResp) {
            //check if reach the max time, using RTC?
//            if ((HAL_GetTick() - startTick) > MaxOtaTime) {
//                error();
//            }
            // sleep
            continue;
        } else if (oState == OtaEnd) {
            // check the firmware
            // jump to the application
        } else if (oState == OtaOK){
            // config the xmodem transmission
            // prepare the flash
            xState = XmodemReceive(&xInitStrcut);
        }
        
        
        // update ota state
        // updateState();
    }
}

/***************************************************************************************************
 * PRIVATE FUNCTION
***************************************************************************************************/
// init the metadata
void initOta() {
    // init the ota metadata
    IsOtaStart  = oFalse;
    pFirm       = NULL;
}

uint8_t otaRead(uint8_t *pBuf, uint32_t timeOut) {
    uint8_t n;
    // only for test
    n = (uint8_t)uartReceive_Pkt(pBuf, timeOut);
    
    // TODO: identify the pkt
    //identify(pBuf, n);
    
    return n;
}

void otaWrite(uint8_t *pBuf, uint8_t length) {
    // only for test, need to add packet head
    uartSend_IT(pBuf, length);
}

// request the gateway to start ota
OtaState_t requestOta(void) {
    uint8_t rt = 0;
    uint8_t n;
    
    // construct request frame
    RxTxBuf[0] = 0x00;
    RxTxBuf[1] = 0x01;
    RxTxBuf[2] = 0x00;
    RxTxBuf[3] = 0x00;
    
    while (oTrue) {
        if ((rt++) >= OTA_REQUEST_RT) {
            return oFalse;
        }
        otaWrite(RxTxBuf, OTA_REQUEST_SIZE);
             
        // receive a packet(command)
        n = otaRead(RxTxBuf, 3000);
        // for test: 1bytes moduleID, 4bytes moduleVer, 4bytes moduleCRC
        if (n != 9) {
            continue;
        } else {
            ModuleID = RxTxBuf[0];
            ModuleVer = (RxTxBuf[1] << 24) + (RxTxBuf[2] << 16) + \
                (RxTxBuf[3] << 8) + RxTxBuf[4];
            ModuleCrc = (RxTxBuf[5] << 24) + (RxTxBuf[6] << 16) + \
                (RxTxBuf[7] << 8) + RxTxBuf[8];
        }
    }
}


// send data to the sever
uint8_t send(uint8_t *buf, uint8_t len) {
    if(uartSend_IT(buf, (uint32_t)len) != HAL_OK) {
        return 0;
    }
    return len;
}

// recevie data from the sever
pktRx receive(uint8_t *pkt, uint32_t timeout) {
    pktRx rx;
    uint8_t n;
    n = (uint8_t)uartReceive_Pkt(pkt, timeout);

    if (identify(pkt, n) != TRUE) {
        rx.n = 0;
        rx.pkt = NULL;
        return rx;
    }

    rx.n = n - RF_HeadSize;
    rx.pkt = pkt + RF_HeadSize;
    return rx;    
}

// send command to the sever
BOOL sendCmd(uint8_t cmd) {
    if(uartSend_IT(&cmd, 1) != HAL_OK) {
        return FALSE;
    }
    return TRUE;
}

// check if the packet belone to this sensor node
OtaBool identify(uint8_t *pkt, uint8_t n) {
    // if (n < RF_MinSize) {
    //     return FALSE;
    // }
    // if (is_not_my_packet) {
    //     return FALSE;
    // }

    return TRUE;
}



// erase the whole extra flash to store the new firmware
BOOL flashReset() {
    if (flashPageErase(EXTRA_FLASH_ADDR, EXTRA_FLASH_SIZE) != FLASHIF_OK) {
       return FALSE;
    }
    return TRUE;
}

// write data into FLASH wich content the reconstruct firmware
BOOL writeFirm(uint32_t data) {
    flashWriteBuf[flashBufIndex++] = data;  // append a data
    if (flashBufIndex == HALF_PAGE_WORD) {
        if (flashHalfPageWrite(pFirm, flashWriteBuf) != HAL_OK) {
            BSP_LED_On(LED4);
            return FALSE;
        }
        pFirm += HALF_PAGE_SIZE;    // point to next half page
        flashBufIndex = 0;          // reset the index
    }
    return TRUE;
}

// copy data from old firmware into FLASH wich content the reconstruct firmware
BOOL cpyFirm(uint16_t offset, uint16_t length) {
    uint32_t Offset;
    uint32_t *pSource;
    uint16_t i;
    
    Offset = (uint32_t)offset * 4;
    if ((Offset + length) > FIRMWARE_SIZE) {
        return FALSE;
    }
   
    pSource = (uint32_t*)(FIRMWARE_ADDR + Offset);
    for (i = 0; i < length; i++) {
        if (writeFirm(pSource[i]) != TRUE) {
            return FALSE;
        }
    }
    
    return TRUE;
}

// if the last data in buffer is less than 64 bytes and have not been written into flash
// write the remainning data into the flash
BOOL flushBuffer(void) { 
    // if flashBufIndex == 0, all the data have been written
    while(flashBufIndex) {
        flashWriteBuf[flashBufIndex++] = 0;  // append a data
        if (flashBufIndex == HALF_PAGE_WORD) {
            if (flashHalfPageWrite(pFirm, flashWriteBuf) != HAL_OK) {
                BSP_LED_On(LED4);
                return FALSE;
            }
            pFirm += HALF_PAGE_SIZE;    // point to next half page
            flashBufIndex = 0;          // reset the index
        }
    }
    return TRUE;
}

// check the firmware
BOOL checkFirm(uint32_t addr) {
    if (NewFirmCRC != crc32((uint32_t*)addr, NewFirmSize)) {
        return FALSE;
    }
    return TRUE;
}

BOOL loadFirm(void) {
    uint32_t i, n, pSource;
    // erase the old firmware
    if (flashPageErase(FIRMWARE_ADDR, FIRMWARE_SIZE) != FLASHIF_OK) {
        return FALSE;
    }
    
    if (NewFirmSize % HALF_PAGE_SIZE == 0) {
        n = NewFirmSize / HALF_PAGE_SIZE;
    } else {
        n = NewFirmSize / HALF_PAGE_SIZE + 1;
    }
    
    pSource = FIRMWARE_ADDR;
    for(i = 0; i < n; i++) {
        memcpy(flashWriteBuf, (uint8_t*)(EXTRA_FLASH_ADDR + i * HALF_PAGE_SIZE), HALF_PAGE_SIZE);
        if (flashHalfPageWrite(pSource, flashWriteBuf) != HAL_OK) {
                BSP_LED_On(LED4);
                return FALSE;
        }
        pSource += HALF_PAGE_SIZE;
    }

    return TRUE;
}






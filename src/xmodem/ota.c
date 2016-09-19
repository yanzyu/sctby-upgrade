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
#include "string.h"
#include "uart.h"
#include "stm32l0xx_hal.h"

/***************************************************************************************************
 * MACRO
***************************************************************************************************/
// RF packet define
#define RF_HeadSize  0
#define RF_MinSize   11
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

/***************************************************************************************************
 * GLOBAL VARIABLE
***************************************************************************************************/
// show the xmodem state
static OtaBool IsOtaStart;
static uint32_t MaxOtaTime = 0xff000;

static uint8_t DevEUI[DEV_EUI_LEN] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04};
static uint8_t FirmVer[FIRM_VER_LEN];

static DevState_t DevState = DEV_STATE_OK;

static uint32_t SleepTick;
static uint32_t ModuleAddr;
static uint32_t ModuleSize;
static uint32_t ModuleCrc32;
static uint32_t FlashIndex;

XmodemInit_t    XmodemInitStrcut;

/***************************************************************************************************
 * PRIVATE FUNCTION DECLARATION
***************************************************************************************************/
static void initOta(void);
static Frame_t otaRead(uint8_t *pBuf, uint32_t timeOut);
static OtaBool identify(uint8_t *pkt, uint8_t n);
static void otaWrite(Frame_t type, uint8_t *pBuf, uint8_t n);    // maxium length is 255
static void sendState(void);

static XmodemBool XmodemRead(uint8_t *pBuf, uint32_t timeout);   // Read function
static void XmodemWrite(uint8_t *pBuf, uint8_t len);          // Write function
static XmodemBool XmodemCallBack(uint8_t *pBuf);              // Operation after receiving a xmodem packet

/***************************************************************************************************
 * PUBLIC FUNCTION
***************************************************************************************************/
/***************************************************************************************************
 * Name: ota()
 * Doc: receiving the update edit table, rebuild and program the new firmware
 * Input: none
 * Output: OTA state
*/
void ota(void) {
    XmodemState_t   xState;
    
    uint8_t rt = 0, buf[FRAME_GW_RESP_LEN];
    
    //uint32_t startTick = HAL_GetTick();
    
    initOta();  // init the variables using in ota
      
    while (oTrue) {
        if ((rt++) >= OTA_REQUEST_RT) {
            
            // enter error mode
            //return OtaNoResp;
        }
        sendState();    // requir update
        if (otaRead(buf, 3000) == FRAME_GW_RESP) {
            if (buf[0] == DEV_UPGRADE) {
                // config the xmodem transmission
                ModuleAddr = (uint32_t)(buf[1] << 24) | (uint32_t)(buf[2] << 16) \
                    | (uint32_t)(buf[3] << 8) | (uint32_t)(buf[4]);
                ModuleSize = (uint32_t)(buf[5] << 24) | (uint32_t)(buf[6] << 16) \
                    | (uint32_t)(buf[7] << 8) | (uint32_t)(buf[8]);
                ModuleCrc32 = (uint32_t)(buf[9] << 24) | (uint32_t)(buf[10] << 16) \
                    | (uint32_t)(buf[11] << 8) | (uint32_t)(buf[12]);
                FlashIndex = ModuleAddr;       
                
                // prepare the flash
                flashPageErase(ModuleAddr, ModuleSize);
                
                // xmodem transfer
                xState = XmodemReceive(&XmodemInitStrcut);
                if (xState != XmodemOK) {
                    // enter error mode
                }
                
                // check the module
                if (crc32((uint32_t*)ModuleAddr, ModuleSize) != ModuleCrc32) {
                    // enter error mode
                }
            } else if (buf[0] == DEV_JUMP) {
                while (oTrue) { }
                // check the firmware
                // jump to the application        
            } else if (buf[0] == DEV_SLEEP) {
                while (oTrue) { }
                // sleep named time
            } else {
                // sleep some time
                // simulate sleep
                HAL_Delay(7000);
            }
        }
    }
}

/***************************************************************************************************
 * PRIVATE FUNCTION
***************************************************************************************************/
// init the metadata
void initOta() {
    // init the ota metadata
    IsOtaStart  = oFalse;
    FlashIndex  = 0x00;
    
    // TODO: get the DevEUI
    // getDevEUI();
    
    // TODO: get the FirmVer
    // getFirmVer();
    
    XmodemInitStrcut.RtNum    = 10; 
    XmodemInitStrcut.TimeOut  = 3000;
    XmodemInitStrcut.Read     = XmodemRead;
    XmodemInitStrcut.Write    = XmodemWrite;
    XmodemInitStrcut.CallBack = XmodemCallBack;
}

// read data from gateway, check if the packet belong to this SN
// return the frame type, NOTE: the size and structure of each type of frame is known
Frame_t otaRead(uint8_t *pBuf, uint32_t timeOut) {
    uint8_t n;
    uint8_t buf[RF_MaxSize];
    // only for test
    n = (uint8_t)uartReceive_Pkt(buf, timeOut);
    
    if (identify(pBuf, n) != oTrue) {
        return FRAME_WRONG;
    }
    memcpy(pBuf, buf+RF_MinSize, n-RF_MinSize);
    return (Frame_t)*(buf+RF_MinSize-2);
}

// write data to gateway, packet the data to responsible frame
void otaWrite(Frame_t type, uint8_t *pBuf, uint8_t n) {
    uint8_t buf[RF_MaxSize];
    uint8_t i, *p = buf;
    uint16_t crc;
    
    *(p++) = FRAME_HEAD;
    for (i = 0; i < DEV_EUI_LEN; i++) {
        *(p++) = DevEUI[i]; 
    }
    *(p++) = type;
    *(p++) = n;
    for (i = 0; i < n; i++) {
        *(p++) = pBuf[i];
    }
    if (type == FRAME_XMODEM) {
        uartSend_IT(buf, n + RF_MinSize);
    } else {
        crc = crc16(pBuf, n);
        *(p++) = (uint8_t)(crc >> 8);
        *p     = (uint8_t)crc;
        uartSend_IT(buf, n + RF_MinSize + 2);
    }
}

// send the current state of device
void sendState() {
    uint8_t buf[FRAME_DEV_STA_LEN];
    
    buf[0] = DevState;
    memcpy(buf+1, FirmVer, FIRM_VER_LEN);
    otaWrite(FRAME_DEV_STA, buf, FRAME_DEV_STA_LEN);
}

// check if the packet belone to this sensor node
OtaBool identify(uint8_t *pkt, uint8_t n) {
    uint8_t i;
    uint8_t *p = pkt;
    // check packet size and 
    if ((n < RF_MinSize) || (*(p++) != FRAME_HEAD)) {
        return oFalse;
    }
    // check if the packet belong to this sensor node
    for (i = 0; i < DEV_EUI_LEN; i++) {
        if (*(p++) != DevEUI[i]) {
            return oFalse;
        }
    }
    // check the integrity
    if (*(p++) == FRAME_XMODEM) {
        if (n != (*(p)) + RF_MinSize) {
            return oFalse;
        }
    } else {
        if (n != (*(p++)) + RF_MinSize + 2) {
            return oFalse;
        }
        if (crc16(p, *(p-1)) != (uint16_t)(pkt[n-1] + (pkt[n-2] << 8))) {
            return oFalse;
        }
    }
    return oTrue;
}

/****************************************************************************
* IMPLEMENT THE XMODEM FUNCTION
****************************************************************************/
XmodemBool XmodemRead(uint8_t *pBuf, uint32_t timeout) {
    if (otaRead(pBuf, timeout) != FRAME_XMODEM) {
        return xFalse;
    } 
    return xTrue;
}

void XmodemWrite(uint8_t *pBuf, uint8_t len) {
    otaWrite(FRAME_XMODEM, pBuf, len);
}

XmodemBool XmodemCallBack(uint8_t *pBuf) {
    // received a 128-bytes update packet using xmodem
    // write the update packet into flash
    if (flashHalfPageWrite(FlashIndex, (uint32_t*)pBuf) != 0) {          
        return xFalse;
    }
    FlashIndex += HALF_PAGE_SIZE;    // point to next half page
    if (flashHalfPageWrite(FlashIndex, (uint32_t*)(pBuf + HALF_PAGE_SIZE)) != 0) {          
        return xFalse;
    }
    FlashIndex += HALF_PAGE_SIZE;    // point to next half page
    return xTrue;
}




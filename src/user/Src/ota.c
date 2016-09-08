/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    ota.c
* @data         2015/12/3
* @auther       yan zeyu
* @module       ota top module
* @brief        ota top module
***************************************************************************************************/
/***************************************************************************************************
 * INCLUDES
 */
#include "ota.h"
#include "rf.h"

/***************************************************************************************************
 * PRIVATE VARIABLE
 */
frameTypeDef rxFrame;
errorTypeDef errorState;
xmodemStateTypeDef xmodemState;

/* record the last frame information */
uint32_t     appAddress;
uint32_t     appLength;
uint32_t     appSection;
uint32_t     appCRC;

/* only for test */
uint8_t desAddress[ADDRESS_SIZE] = {0};
const uint32_t appEntry = 0x08005800;

enum{
    MODULE_ID_MODULE_DESC = 0,
    MODULE_ID_BSW,
    MODULE_ID_COUNT
};
/* entry for every module, include entry address, module space size, module ver, module space crc */
typedef struct{
    unsigned int* entry;
    unsigned int size;
    unsigned int ver;   /* use only byte 2 = major, byte 3 = minor */
    unsigned int crc;
}moduleInfo_t;


/* first module is the module description region, it's version is firmware version */
typedef struct{
    unsigned int moduleCnt;
    moduleInfo_t module[MODULE_ID_COUNT];
}moduleDescription_t;

typedef unsigned int* (*getDescAddr_h)( void );
typedef char (*verify_h)( void );
typedef void (*AppEntry_h)( void );

/* interface define */
typedef struct{
    getDescAddr_h hGetDescAddr;
    verify_h hVerify;   /* verify firmware */
    AppEntry_h hApp;    /* app main */
}moduleDescIf_t;

//((moduleDescription_t*)( ((moduleDescIf_t*)(LR_IROM_MODULE_DESC_ADDR))->hGetDescAddr()))->module[(__ModuleId)].entry

#define MODULE_ENTRY( __ModuleId ) ((void*)(*(((moduleDescription_t*)(((moduleDescIf_t*)(*((unsigned int*)(appEntry))))->hGetDescAddr()))->module[(__ModuleId)].entry)))



/***************************************************************************************************
 * PRIVATE FUNCTION
 */
static stateTypeDef firmwareCheck(void);
static stateTypeDef appCheck(void);
static void updateAppInfo(void);
static stateTypeDef appFrameHandle(void);
static stateTypeDef cmdFrameHandle(void);
static void otaErrorState(void);
void jumpToApp(void);
/***************************************************************************************************
 * @fn      firmwareCheck()
 *
 * @brief   check the whole firmware.
 *
 * @author  yan zeyu
 *
 * @param   none.
 *
 * @return  state of option
 */
stateTypeDef firmwareCheck(void)
{
    return OPERATION_OK;
}

/***************************************************************************************************
 * @fn      appCheck()
 *
 * @brief   check the last app in flash.
 *			
 * @author  yan zeyu
 *
 * @param   none.
 *
 * @return  state of option
 */
static stateTypeDef appCheck(void)
{
#if 0
    crc32Init();
    if(crcCheck32((uint8_t*)appAddress, appSection) != appCRC)
        return OPERATION_ERROR;
#endif
    return OPERATION_OK;
}

/***************************************************************************************************
 * @fn      updateAppInfo()
 *
 * @brief   record the app information of the last app.
 *
 * @author  yan zeyu
 *
 * @param   none.
 *
 * @return  none.
 */
static void updateAppInfo(void)
{
    /* get app address */
    appAddress = (uint32_t)(rxFrame.data[0] << 24) +  (uint32_t)(rxFrame.data[1] << 16)
        + (uint32_t)(rxFrame.data[2] << 8) + (uint32_t)(rxFrame.data[3] << 0);
    /* get app length */
    appLength = (uint32_t)(rxFrame.data[4] << 24) +  (uint32_t)(rxFrame.data[5] << 16)
        + (uint32_t)(rxFrame.data[6] << 8) + (uint32_t)(rxFrame.data[7] << 0);
    /* get app section */
    appSection = (uint32_t)(rxFrame.data[8] << 24) +  (uint32_t)(rxFrame.data[9] << 16)
        + (uint32_t)(rxFrame.data[10] << 8) + (uint32_t)(rxFrame.data[11] << 0);
    /* get app crc */
    appCRC = (uint32_t)(rxFrame.data[12] << 24) +  (uint32_t)(rxFrame.data[13] << 16)
        + (uint32_t)(rxFrame.data[14] << 8) + (uint32_t)(rxFrame.data[15] << 0);
}

/***************************************************************************************************
 * @fn      appFrameHandle()
 *
 * @brief   operration after receiving an app frame. 
 *
 * @author  yan zeyu
 *
 * @param   none.
 *
 * @return  state of option.
 */
static stateTypeDef appFrameHandle(void)
{
    uint32_t rtNum = 1;
    updateAppInfo();
    
    while(1)
    {
        if(rtNum++ > rtMaxNum)
        {
            errorState = TRANS_ERROR;
            otaErrorState();
        }
        sendCmd(CUS_ACK);
        /* wait the server enter xmodem state */
        if(receiveFrame(&rxFrame, FRAME_TIMEOUT) != OPERATION_TIMEOUT)
        {
            continue;
        }
        /* enter xmodem state */
        /* erase the app section */
        flashPageErase(appAddress, appSection);
        /* xmodem update */
        xmodemState = xmodemUpdateProgram(appAddress);
        if(xmodemState == FLASH_WRITE_ERROR)
        {
            errorState = FLASH_ERROR;
            otaErrorState();
        }
        if(xmodemState != XMODEM_OK)
        {
            errorState = XMODEM_ERROR;
            otaErrorState();
        }
        break;
    }
    return OPERATION_OK;
}

/***************************************************************************************************
 * @fn      cmdFrameHandle()
 *
 * @brief   operation after receiving a command frame.
 *
 * @author  yan zeyu
 *
 * @param   none.
 *
 * @return  state of option.
 */
static stateTypeDef cmdFrameHandle(void)
{
    /* app check command */
    if(rxFrame.data[0] == CUS_CHK)
    {
        if(appCheck() != OPERATION_OK)
        {
            errorState = CHECK_ERROR;
            otaErrorState();
        }
        sendCmd(CUS_ACK);
        return OPERATION_OK;
    }
        
    /* end of update command */
    if(rxFrame.data[0] == CUS_EOU)
        jumpToApp();    //including firmware check.
    
    return OPERATION_OK;
}

/***************************************************************************************************
 * @fn      otaErrorState()
 *
 * @brief   ota run in error mode.
 *          send the error command to server.
 *          if not received an app frame, enter stop mode.
 *          wake up every *** second.
 *
 * @author  yan zeyu
 *
 * @param   none.   
 *
 * @return  none.
 */
static void otaErrorState(void)
{
    stateTypeDef state;
	/* receive frame */
    while(1)
    {
        sendCmd(errorState);
        /*wait for update */
        state = receiveFrame(&rxFrame, FRAME_TIMEOUT);
        if(state == OPERATION_TIMEOUT)
        {
            /* wake up configuration */
            wakeUpConfig();
            setWakeUpPeriod(WAKEUP_PERIOD);
            /* enter the stop mode */
            enterStopMode();
            wakeUpDisable();
            /* recovery the clock */
            clkConfig();
            continue;
        }
        else if(state == OPERATION_ERROR)
            continue;
        
        /*******************************************************************************************
         * receive app address frame.
         ******************************************************************************************/
        if(rxFrame.type == APP_FRAME)
            if(appFrameHandle() != OPERATION_OK)
                continue;
        /* app check command */
            if(rxFrame.type == CMD_FRAME)
                if(cmdFrameHandle() == OPERATION_OK)
                    continue;
        /* go on receiving frame and update */
        while(1)
        {
            if(receiveFrame(&rxFrame, FRAME_TIMEOUT) != OPERATION_OK)
                continue;
            
            /*******************************************************************************************
             * receive app address frame.
             ******************************************************************************************/
            if(rxFrame.type == APP_FRAME)
                if(appFrameHandle() == OPERATION_OK)
                    continue;
            
            /*******************************************************************************************
             * receive command. 
             ******************************************************************************************/
            /* app check command */
            if(rxFrame.type == CMD_FRAME)
                if(cmdFrameHandle() == OPERATION_OK)
                    continue;
        }
    }
}

/***************************************************************************************************
 * @fn      jumpToApp()
 *
 * @brief   check the firmware and jump to the user application.
 *          send ACK or NAK after firmware check.
 *
 * @author  yan zeyu
 *
 * @param   pageStart: the address of the first page to be erased.
 *                  wordsLength: the number of words need to be erased.
 * @return  state of option
 */
void jumpToApp(void)
{
    unsigned int jumpAddr;
    moduleDescription_t *pModuleDesc; 
    
    moduleDescIf_t *pModuleDescIf = (moduleDescIf_t*)MODULE_ENTRY(MODULE_ID_MODULE_DESC);
    pModuleDesc = (moduleDescription_t*)pModuleDescIf->hGetDescAddr();
    
    jumpAddr = (unsigned int)pModuleDesc->module[MODULE_ID_BSW].entry;
    
    
   if(firmwareCheck() != OPERATION_OK)
   {
       errorState = CHECK_ERROR;
       otaErrorState();
   }
   sendCmd(CUS_ACK);
   
   
   
   
   jump(jumpAddr);
}

/***************************************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
 *
***************************************************************************************************/
/***************************************************************************************************
 * @fn      ota()
 *
 * @brief   ota bootloader main processing.
 *
 * @author  yan zeyu
 *
 * @param   pageStart: the address of the first page to be erased.
 *                  wordsLength: the number of words need to be erased.
 * @return  state of option
 */
void ota(void)
{
    uint32_t rtNum = 1;
    uint8_t rtNoRsp = 1;
    
    
    /* send the version and receive the first app frame / EOU command */
    while(1)
    {
        if(rtNum++ > rtMaxNum)
        {
            if( rtNoRsp == 1 ){
                jumpToApp();
            }else{
                errorState = TRANS_ERROR;
                otaErrorState();
            }
        }
        rfInit();
        /* if not send successfully, try again */
        if(sendVersion() != OPERATION_OK)
            continue;
        /* if not receive successfully, try again */
        if(receiveFrame(&rxFrame, FRAME_TIMEOUT) != OPERATION_OK)
            continue;
        /* if receive end of update command, jump to app */
        if((rxFrame.type == CMD_FRAME) && (rxFrame.data[0] == CUS_EOU))
            cmdFrameHandle();
        /* nor the command frame or app frame, retry */
        if(rxFrame.type != APP_FRAME)
            continue;
                 
        appFrameHandle();
        break;
    }
    
    /* receive frame */
    while(1)
    {
        if(receiveFrame(&rxFrame, FRAME_TIMEOUT) != OPERATION_OK)
            continue;
        
        /*******************************************************************************************
         * receive app address frame.
         ******************************************************************************************/
        if(rxFrame.type == APP_FRAME)
            if(appFrameHandle() == OPERATION_OK)
                continue;
        
        /*******************************************************************************************
         * receive command. 
         ******************************************************************************************/
        /* app check command */
        if(rxFrame.type == CMD_FRAME)
            if(cmdFrameHandle() == OPERATION_OK)
                continue;
    }
}

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by yan zeyu @ 20151127
*   context: here write modified history
*
***************************************************************************************************/


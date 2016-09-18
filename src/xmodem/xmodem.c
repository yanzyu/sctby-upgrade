/***************************************************************************************************
 * OTA module of LoRa netword
 * receive eidt table file using XMODEM protocal
 * rebuild the firmware during receiving the table
***************************************************************************************************/
/***************************************************************************************************
 * INCLUDE
***************************************************************************************************/
#include "xmodem.h"
#include "crc.h"

/***************************************************************************************************
 * MACRO
***************************************************************************************************/
// xmodem control charactor
#define XMODEM_C  	 0x43	// ASCII 'C', start a xmodem transfer with crc16
#define	XMODEM_SOH	 0x01
#define	XMODEM_EOT	 0x04
#define	XMODEM_ACK	 0x06
#define	XMODEM_NAK	 0x15
#define	XMODEM_CAN 	 0x18

// xmodem packet size
#define XMODEM_PktSize  133 // bytes of a xmodem packet, 133 bytes
#define XMODEM_DataSize 128 // bytes of xmodem data, 128 bytes

/***************************************************************************************************
 * GLOBAL VARIABLE
***************************************************************************************************/
// show the xmodem state
static XmodemBool     IsStart;
static uint8_t        RtNum;
static uint8_t        PktNum;   // xmodem packet number has been received
static uint32_t       TimeOut;

static XmodemRead     Read;   // Read Function
static XmodemWrite    Write;  // Write Function
static XmodemCallBack CallBack;    // Operation after receiving a xmodem packet

/***************************************************************************************************
 * PRIVATE FUNCTION DECLARATION
***************************************************************************************************/
static void Init(const XmodemInit_t *InitStruct);
static XmodemState_t receive(uint8_t *pkt);
static XmodemState_t checkPkt(uint8_t *pkt, uint8_t n);
static void sendCmd(uint8_t cmd);

/***************************************************************************************************
 * PUBLIC FUNCTION
***************************************************************************************************/
/***************************************************************************************************
 * Name: ota()
 * Doc: receiving the update edit table, rebuild and program the new firmware
 * Input: none
 * Output: OTA state
*/
XmodemState_t XmodemReceive(const XmodemInit_t *InitStruct) {
    XmodemState_t state;
    uint32_t timeTick;
    uint8_t pktBuf[XMODEM_PktSize];
               
    Init(InitStruct);
    // read the edit table and rebuild the new firmware
    while(xTrue) {
        state = receive(pktBuf);
        // if xmodem receive error, return
        if (state == XmodemFalid) {
            sendCmd(XMODEM_CAN);
            return XmodemFalid;
        }
        // if received end command, break and check the firmware
        if (state == XmodemEnd) {
            return XmodemOK;
        }      
        // if receive a xmodem packet successfully, response ACK and construct the commands
        if (state == XmodemOK) {
            if (CallBack() != xTrue) {
                sendCmd(XMODEM_CAN);
                return XmodemFalid;
            }
            sendCmd(XMODEM_ACK);
        }
    }
}

void Init(const XmodemInit_t *InitStruct) {
    PktNum  =   0;
    RtNum   =   InitStruct->RtNum;
    TimeOut =   InitStruct->TimeOut;
    Read    =   InitStruct->Read;
    Write   =   InitStruct->Write;
    CallBack =  InitStruct->CallBack;
}

/***************************************************************************************************
 * Name: xmodemRead()
 * Doc: read the a xmodem packet, return the state and the xmodem data
 * Input: pointer of the buffer to contain the 128-bytes xmodem data
 * Output: xmodem state
*/
XmodemState_t receive(uint8_t *pkt) {
    uint8_t rtTime = 0;
    uint8_t recvBytes;
    XmodemState_t state;
   
    while(xTrue) {
        // retransmission failed, return
        if (rtTime++ >= RtNum) {
            return XmodemFalid;
        }
        // if not start xmodem, send 'C' to start
        if (IsStart == xFalse) {
            sendCmd(XMODEM_C);
        }
        // if not receive data, return 
        recvBytes = Read(pkt, TimeOut);
        if (recvBytes == 0) {
            if (IsStart) {
                sendCmd(XMODEM_NAK);    // timeout while receiving a packet, send NAK
            }
            continue;
        }
        // xomdem transimission start
        if (IsStart != xTrue) {
            IsStart = xTrue; 
        }
   
        // check the packet
        state = checkPkt(pkt, recvBytes);
        if (state == XmodemDuplicatePacket) {
            sendCmd(XMODEM_ACK);
            continue;
        } else if (state == XmodemWrongPacket) {
            sendCmd(XMODEM_NAK);
            continue;
        } else {
            return state;
        }
    }
}

XmodemState_t checkPkt(uint8_t *pkt, uint8_t n) {
    uint16_t crc;
    if (pkt[0] == XMODEM_EOT)     return XmodemEnd;
    if (n < XMODEM_PktSize)       return XmodemWrongPacket;
    // is not a xmodem packet
    if (pkt[0] != XMODEM_SOH)     return XmodemWrongPacket;
    // check the packet number
    if (pkt[1] + pkt[2] != 0xff)  return XmodemWrongPacket;
    if (pkt[1] == PktNum)         return XmodemDuplicatePacket;
    if (pkt[1] != PktNum + 1)     return XmodemFalid;

    // crc check
    crc = ((uint16_t)pkt[XMODEM_PktSize-2] << 8) + (uint16_t)pkt[XMODEM_PktSize-1];
    if (crc != crc16(pkt+3, XMODEM_DataSize))   return XmodemWrongPacket; 
    // update the pktNum
    PktNum = pkt[1];
    return XmodemOK;
}

// send command to the sever
void sendCmd(uint8_t cmd) {
    Write(&cmd, 1);    
}


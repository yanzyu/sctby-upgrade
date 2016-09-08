/***************************************************************************************************
 * Copyright (c) 2015-2020 Intelligent Network System Ltd. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
////////////////////////////////////////////////////////////////////////////////////////////////////
                                                                        //                          
 ////////                                ////         ///              ///        ///////           
  //   //                                 //          ///               //          //              
  //  /                                   //                            //          //              
  //  /                                   //                            //          //              
  /////     //////     //////  /// ////   //        /////    // ////    // /////    //     // ////  
  //  /    ///   //   //   //   //  //    //          ///     ///  //   // //       //      ///  // 
  //  /       /////   //        //  /     //          ///     //   //   ////        //      //   // 
  //        /// ///    /////     ////     //          ///     //   //   /////       //      //   // 
  //    /  //   ///        //    ////     //    /     ///     //   //   // ///      //      //   // 
  //   //  //   ///   //   //     //      //   //     ///     //   //   //  //      //      //   // 
 ////////   ////////  ///////     //     ////////   ///////  //// //// //// ////  ///////  //// ////
                                  /                                                                 
////////////////////////////////////////////////////////////////////////////////////////////////////

***************************************************************************************************/
/***************************************************************************************************
* @file name    app_dtu.c
* @data         2015/12/10
* @auther       chuanpengl
* @module       dtu module
* @brief        dtu module
***************************************************************************************************/

/***************************************************************************************************
 * INCLUDES
 */
#include "rf.h"
#include "sx1278_if.h"
#include <string.h>
#include "crc.h"

/***************************************************************************************************
 * DEBUG SWITCH MACROS
 */

/***************************************************************************************************
 * MACROS
 */
#define UNIQUE_LENGTH       (12)
#define MSG_LL_LEN( msg )   ((msg).len + 6)
#define uniqueAddr          ((wlUint8*)0x1FF80050)

/***************************************************************************************************
 * TYPEDEFS
 */
typedef struct
{
    wlUint8 header;
    wlUint8 len;
    wlUint8 dstAddr[12];
    wlUint8 srcAddr[12];
    wlUint8 data[255- 30];      /* max size data */
    wlUint8 crc[4];             /* crc member is followed by data, but data may be less then 255 - 30 */
}msgLl_t;

/***************************************************************************************************
 * LOCAL FUNCTIONS DECLEAR
 */
/***************************************************************************************************
 * @fn      dtu_Ind__()
 *
 * @brief   rf receive
 *
 * @author  chuanpengl
 *
 * @param   a_eMode  - mode, a_eType  - type
 *
 * @return  none
 */
static void dtu_Ind__( dtuMode_e a_eMode, dtuUint32 a_eType );

/***************************************************************************************************
 * @fn      dtuRecv__
 *
 * @brief   set data to host, host can be uart host, or other application
 *
 * @author  chuanpengl
 *
 * @param   none
 *
 * @return  none
 */
static void dtuRecv__( dtuUint8 *a_pu8Data, dtuUint16 a_u16Length );


/***************************************************************************************************
 * @fn      dtuSend__
 *
 * @brief   get data from host, host can be uart host, or other application
 *
 * @author  chuanpengl
 *
 * @param   none
 *
 * @return  get data length in fact
 */
static dtuUint16 dtuSend__( dtuUint8 *a_pu8Data, dtuUint16 a_u8Length );

/***************************************************************************************************
 * @fn      dtuGetUniqueId__
 *
 * @brief   get unique id
 *
 * @author  chuanpengl
 *
 * @param   none
 *
 * @return  none
 */
static void dtuGetUniqueId__( void );

/***************************************************************************************************
 * CONSTANTS
 */



/***************************************************************************************************
 * GLOBAL VARIABLES
 */


/***************************************************************************************************
 * STATIC VARIABLES
 */
static sx1278If_t *gs_pRfIf = wlNull;
static wlBool gs_bContinue = wlTrue;
static msgLl_t gs_au8Buffer;
static wlUint8 gs_u8Unique[UNIQUE_LENGTH];
static wlUint8 gs_u8ServerUnique[UNIQUE_LENGTH];
/***************************************************************************************************
 * EXTERNAL VARIABLES
 */


 
/***************************************************************************************************
 *  GLOBAL FUNCTIONS IMPLEMENTATION
 */
/***************************************************************************************************
 * @fn      rfInit()
 *
 * @brief   rf init
 *
 * @author  chuanpengl
 *
 * @param   none
 *
 * @return  none
 */
void rfInit( void )
{
    gs_pRfIf = dtu_getInterface();
    gs_pRfIf->hInit(dtu_Ind__, dtuRecv__, dtuSend__);
    dtuGetUniqueId__(); /* get unique */
    memset( gs_u8ServerUnique, 0x00, UNIQUE_LENGTH );/* reset server unique id */
}


/***************************************************************************************************
 * @fn      rfSend()
 *
 * @brief   rf send
 *
 * @author  chuanpengl
 *
 * @param   a_u8Buf  - data buffer,
 *          a_u8Len  - buffer length for send
 *          a_u16Timeout  - timeout value, unit = ms
 *
 * @return  < 0  - error, = 0 -- success
 */
wlInt32 rfSend( wlUint8 *a_u8Buf, wlUint8 a_u8Len, wlUint16 a_u16Timeout)
{
    wlInt32 ret = 0;
    wlUint32 crc = 0;
    
    gs_pRfIf->hInitRfMode(DTU_MODE_RF_NORMAL);
    
    /* add link layer info */
    gs_au8Buffer.header = 0xFA;
    gs_au8Buffer.len = 24 + a_u8Len;
    memcpy( gs_au8Buffer.srcAddr, gs_u8Unique, UNIQUE_LENGTH );
    memcpy( gs_au8Buffer.dstAddr, gs_u8ServerUnique, UNIQUE_LENGTH );
    /* add message */
    memcpy( gs_au8Buffer.data, a_u8Buf, a_u8Len );
    /* add crc */
    if(crc32Init() != HAL_OK)
        return -1;
    crc = crcCheck32(gs_au8Buffer.dstAddr, gs_au8Buffer.len);
    
    gs_au8Buffer.data[ a_u8Len + 0 ] = (wlUint8)((crc >> 24) & 0xFF);
    gs_au8Buffer.data[ a_u8Len + 1 ] = (wlUint8)((crc >> 16) & 0xFF);
    gs_au8Buffer.data[ a_u8Len + 2 ] = (wlUint8)((crc >> 8) & 0xFF);
    gs_au8Buffer.data[ a_u8Len + 3 ] = (wlUint8)((crc >> 0) & 0xFF);
    
    gs_pRfIf->hReqSend( MSG_LL_LEN(gs_au8Buffer) );
    gs_bContinue = wlTrue;
    while(gs_bContinue != wlFalse)
    {
        gs_pRfIf->hRoutine();
    }
    //TODO: ret deal
    return ret;
}


/***************************************************************************************************
 * @fn      rfRecv()
 *
 * @brief   rf receive
 *
 * @author  chuanpengl
 *
 * @param   a_u8Buf  - data buffer,
 *          a_u8Size  - buffer size
 *          a_u16Timeout  - timeout value, unit = ms
 *
 * @return  >= 0  - receive length, < 0  - error
 */
wlInt32 rfRecv( wlUint8 *a_u8Buf, wlUint8 a_u8Size, wlUint16 a_u16Timeout)
{
    wlInt32 ret = -1;
    wlUint32 crc;
    wlUint32 start = HAL_GetTick();
    gs_pRfIf->hInitRfMode(DTU_MODE_RF_NORMAL);

    while(1)/* timeout */
    {
        
        gs_bContinue = wlTrue;
        while(gs_bContinue != wlFalse){
            gs_pRfIf->hRoutine();
             /* timeout handle */
            if((HAL_GetTick() - start) > a_u16Timeout)
            {
                return -1;   /* timeout */
            }
        }
        
        if( gs_au8Buffer.header != 0xFA ){
            continue;
        }
        if(crc32Init() != HAL_OK){
            break;
        }
        
        crc = ((wlUint8*)&gs_au8Buffer)[gs_au8Buffer.len + 2];
        crc = (crc << 8) | (((wlUint8*)&gs_au8Buffer)[gs_au8Buffer.len + 3]);
        crc = (crc << 8) | (((wlUint8*)&gs_au8Buffer)[gs_au8Buffer.len + 4]);
        crc = (crc << 8) | (((wlUint8*)&gs_au8Buffer)[gs_au8Buffer.len + 5]);
        
        if( crc != crcCheck32(gs_au8Buffer.dstAddr, gs_au8Buffer.len) ){
            continue;
        }
        
        if( 0 != memcmp(gs_u8Unique, gs_au8Buffer.dstAddr, UNIQUE_LENGTH )){
            continue;
        }
        
        for( wlUint8 i = 0; i < 12; i++ ){
            if( gs_u8ServerUnique[i] != 0 ){
                break;
            }else{
                if( i == 11 ){
                    memcpy( gs_u8ServerUnique, gs_au8Buffer.srcAddr, UNIQUE_LENGTH );
                }
            }
        }
        
        if( 0 != memcmp( gs_u8ServerUnique, gs_au8Buffer.srcAddr, UNIQUE_LENGTH ) ){
            continue;
        }
        
        ret = gs_au8Buffer.len - 24;
        memcpy( a_u8Buf, gs_au8Buffer.data, ret );
        break;
    }
    
    return ret;
}



/***************************************************************************************************
 * LOCAL FUNCTIONS IMPLEMENTATION
 */
/***************************************************************************************************
 * @fn      dtu_Ind__()
 *
 * @brief   rf receive
 *
 * @author  chuanpengl
 *
 * @param   a_eMode  - mode, a_eType  - type
 *
 * @return  none
 */
void dtu_Ind__( dtuMode_e a_eMode, dtuUint32 a_eType )
{
    if( (a_eType & DTU_IND_RX_OK) == DTU_IND_RX_OK ){
        gs_bContinue = wlFalse;
    }else if( (a_eType & DTU_IND_TX_OK) == DTU_IND_TX_OK ){
        gs_bContinue = wlFalse;
    }else if( (a_eType & DTU_IND_SLEEP_OK) == DTU_IND_SLEEP_OK ){
        gs_bContinue = wlFalse;
    }else{
    }
}

/***************************************************************************************************
 * @fn      dtuRecv__
 *
 * @brief   set data to host, host can be uart host, or other application
 *
 * @author  chuanpengl
 *
 * @param   none
 *
 * @return  none
 */
void dtuRecv__( dtuUint8 *a_pu8Data, dtuUint16 a_u16Length )
{
    memcpy( (void*)&gs_au8Buffer, a_pu8Data, a_u16Length );
}


/***************************************************************************************************
 * @fn      dtuSend__
 *
 * @brief   get data from host, host can be uart host, or other application
 *
 * @author  chuanpengl
 *
 * @param   none
 *
 * @return  get data length in fact
 */
dtuUint16 dtuSend__( dtuUint8 *a_pu8Data, dtuUint16 a_u8Length )
{
    if( a_u8Length > 0 )
    {
        memcpy( a_pu8Data, (void*)&gs_au8Buffer, a_u8Length );
    }
    
    return a_u8Length;
}

/***************************************************************************************************
 * @fn      dtuGetUniqueId__
 *
 * @brief   get unique id
 *
 * @author  chuanpengl
 *
 * @param   none
 *
 * @return  none
 */
void dtuGetUniqueId__( void )
{
    gs_u8Unique[0] = uniqueAddr[0];
    gs_u8Unique[1] = uniqueAddr[1];
    gs_u8Unique[2] = uniqueAddr[2];
    gs_u8Unique[3] = uniqueAddr[3];
    gs_u8Unique[4] = uniqueAddr[4];
    gs_u8Unique[5] = uniqueAddr[5];
    gs_u8Unique[6] = uniqueAddr[6];
    gs_u8Unique[7] = uniqueAddr[7];
    gs_u8Unique[8] = uniqueAddr[20];
    gs_u8Unique[9] = uniqueAddr[21];
    gs_u8Unique[10] = uniqueAddr[22];
    gs_u8Unique[11] = uniqueAddr[23];
}

/***************************************************************************************************
* HISTORY LIST
* 1. Create File by chuanpengl @ 20150708
*   context: here write modified history
*
***************************************************************************************************/

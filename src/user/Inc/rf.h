/***************************************************************************************************
 * Copyright (c) 2015-2020 XXXXXXXXXXXXXXXX. All Rights Reserved. 
 * 
 * This software is the confidential and proprietary information of Founder. You shall not disclose
 * such Confidential Information and shall use it only in accordance with the terms of the 
 * agreements you entered into with Founder. 
***************************************************************************************************/
/***************************************************************************************************
* @file name    rf.h
* @data         10/12/2015
* @author       chuanpengl
* @module       module name
* @brief        file description
***************************************************************************************************/
#ifndef __RF_H__
#define __RF_H__
/***************************************************************************************************
 * INCLUDES
 */

#ifdef __cplusplus
extern "C"{
#endif

/***************************************************************************************************
 * DEBUG SWITCH MACROS
 */

/***************************************************************************************************
 * MACROS
 */



/***************************************************************************************************
 * TYPEDEFS
 */
typedef char wlChar;
typedef unsigned char wlUint8;
typedef signed char wlInt8;
typedef unsigned short wlUint16;
typedef signed short wlInt16;
typedef unsigned int wlUint32;
typedef signed int wlInt32;
typedef enum{wlFalse = 0, wlTrue = !wlFalse}wlBool;
#define wlNull     ((void*)0)


/***************************************************************************************************
 * CONSTANTS
 */


/***************************************************************************************************
 * GLOBAL VARIABLES DECLEAR
 */


/***************************************************************************************************
 * GLOBAL FUNCTIONS DECLEAR
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
void rfInit( void );

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
wlInt32 rfSend( wlUint8 *a_u8Buf, wlUint8 a_u8Len, wlUint16 a_u16Timeout);

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
wlInt32 rfRecv( wlUint8 *a_u8Buf, wlUint8 a_u8Size, wlUint16 a_u16Timeout);

#ifdef __cplusplus
}
#endif

#endif /* __RF_H__ */
 
/***************************************************************************************************
* HISTORY LIST
* 1. Create File by chuanpengl @ 26/11/2015
*   context: here write modified history
*
***************************************************************************************************/

/*
* Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
* foss@huawei.com
*
* If distributed as part of the Linux kernel, the following license terms
* apply:
*
* * This program is free software; you can redistribute it and/or modify
* * it under the terms of the GNU General Public License version 2 and 
* * only version 2 as published by the Free Software Foundation.
* *
* * This program is distributed in the hope that it will be useful,
* * but WITHOUT ANY WARRANTY; without even the implied warranty of
* * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* * GNU General Public License for more details.
* *
* * You should have received a copy of the GNU General Public License
* * along with this program; if not, write to the Free Software
* * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
*
* Otherwise, the following license terms apply:
*
* * Redistribution and use in source and binary forms, with or without
* * modification, are permitted provided that the following conditions
* * are met:
* * 1) Redistributions of source code must retain the above copyright
* *    notice, this list of conditions and the following disclaimer.
* * 2) Redistributions in binary form must reproduce the above copyright
* *    notice, this list of conditions and the following disclaimer in the
* *    documentation and/or other materials provided with the distribution.
* * 3) Neither the name of Huawei nor the names of its contributors may 
* *    be used to endorse or promote products derived from this software 
* *    without specific prior written permission.
* 
* * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*/
#ifndef _TAF_STD_LIB_H_
#define _TAF_STD_LIB_H_

/*****************************************************************************
  1 ??????????????
*****************************************************************************/
#include  "PsTypeDef.h"
#include  "MnMsgApi.h"
#include  "MnErrorCode.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#pragma pack(4)


/*****************************************************************************
  2 ??????
*****************************************************************************/
/* Added by f62575 for V9R1 STK????, 2013-6-26, begin */
#define TAF_STD_7BIT_MASK                                    (0x7f)
/* Added by f62575 for V9R1 STK????, 2013-6-26, end */

/* Added by s00217060 for VoLTE_PhaseIII  ????, 2013-12-24, begin */
#define TAF_STD_MAX_GSM7BITDEFALPHA_NUM                     (128)
#define TAF_STD_NOSTANDARD_ASCII_CODE                       (0xff)
#define TAF_STD_GSM_7BIT_EXTENSION_FLAG                     (0xfe)
/* Added by s00217060 for VoLTE_PhaseIII  ????, 2013-12-24, end */

/* This is the number of days in a leap year set.
   A leap year set includes 1 leap year, and 3 normal years. */
#define TAF_STD_TIME_ZONE_QUAD_YEAR                         (366+(3*365))

/* 5 days (duration between Jan 1 and Jan 6), expressed as seconds. */

#define TAF_STD_TIME_ZONE_OFFSET_S                          (432000UL)

/* This is the year upon which all time values used by the Clock Services
** are based.  NOTE:  The user base day (GPS) is Jan 6 1980, but the internal
** base date is Jan 1 1980 to simplify calculations */

#define TAF_STD_TIME_ZONE_BASE_YEAR                          (1980)

#define TAF_STD_TIME_ZONE_MAX_YEAR                           (2100)

#define TAF_STD_SECONDS_PER_MINUTE                           (60)               /* 60s */

#define TAF_STD_MINUTES_PER_HOUR                             (60)               /* 60m */

#define TAF_STD_HOURS_PER_DAY                                (24)               /* 24HOUR */

#define TAF_STD_NORMAL_MONTH_TAB_COUNT                       (13)

#define TAF_STD_DAYS_ELAPSED_OF_A_LEAP_YEAR_SET_TAB_COUNT    (5)



#define TAF_STD_INVALID_MCC                                 (0xFFFFFFFF)
/*****************************************************************************
  3 ????????
*****************************************************************************/



/*****************************************************************************
  4 ????????????
*****************************************************************************/


/*****************************************************************************
  5 ??????????
*****************************************************************************/


/*****************************************************************************
  6 ????????
*****************************************************************************/
/*****************************************************************************
 ??????    : TAF_STD_TIME_ZONE_TYPE_STRU
 ????????  : TimeZone??????????????

 ????????  :
 1.??    ??   : 2014??12??28??s
   ??    ??   : x00314862
   ????????   : ????

*****************************************************************************/
typedef struct
{
  /* Year [1980..2100) */
    VOS_UINT16                          usYear;

  /* Month of year [1..12] */
    VOS_UINT16                          usMonth;

  /* Day of month [1..31] */
    VOS_UINT16                          usDay;

  /* Hour of day [0..23] */
    VOS_UINT16                          usHour;

  /* Minute of hour [0..59] */
    VOS_UINT16                          usMinute;

  /* Second of minute [0..59] */
    VOS_UINT16                          usSecond;


}TAF_STD_TIME_ZONE_TYPE_STRU;

/*****************************************************************************
  7 STRUCT????
*****************************************************************************/


/*****************************************************************************
  8 UNION????
*****************************************************************************/


/*****************************************************************************
  9 OTHERS????
*****************************************************************************/


/*****************************************************************************
  10 ????????
*****************************************************************************/
VOS_UINT32 TAF_STD_Itoa(
    VOS_UINT32                          ulDigit,
    VOS_CHAR                           *pcDigitStr,
    VOS_UINT32                         *pulDigitStrLength
);

VOS_UINT32 TAF_STD_AsciiNum2HexString(
    VOS_UINT8                          *pucSrc,
    VOS_UINT16                         *pusSrcLen
);

VOS_UINT16 TAF_STD_HexAlpha2AsciiString(
    VOS_UINT8                          *pucSrc,
    VOS_UINT16                          usSrcLen,
    VOS_UINT8                          *pucDst
);

/* Added by f62575 for V9R1 STK????, 2013-6-26, begin */
/*??7bit????????????????????8bit????*/
VOS_UINT32  TAF_STD_UnPack7Bit(
    const VOS_UINT8                    *pucOrgChar,
    VOS_UINT32                          ulLen,
    VOS_UINT8                           ucFillBit,
    VOS_UINT8                          *pucUnPackedChar
);

/*????????????7bit????????*/
VOS_UINT32  TAF_STD_Pack7Bit(
    const VOS_UINT8                    *pucOrgChar,
    VOS_UINT32                          ulLen,
    VOS_UINT8                           ucFillBit,
    VOS_UINT8                          *pucPackedChar,
    VOS_UINT32                         *pulLen
);

/* Added by f62575 for V9R1 STK????, 2013-6-26, end */

/* Added by s00217060 for VoLTE_PhaseIII  ????, 2013-12-16, begin */
VOS_UINT32  TAF_STD_ConvertBcdNumberToAscii(
    const VOS_UINT8                    *pucBcdNumber,
    VOS_UINT8                           ucBcdLen,
    VOS_CHAR                           *pcAsciiNumber
);
VOS_UINT32  TAF_STD_ConvertBcdCodeToAscii(
    VOS_UINT8                           ucBcdCode,
    VOS_CHAR                           *pcAsciiCode
);
VOS_UINT32  TAF_STD_ConvertAsciiNumberToBcd(
    const VOS_CHAR                     *pcAsciiNumber,
    VOS_UINT8                          *pucBcdNumber,
    VOS_UINT8                          *pucBcdLen
);
VOS_UINT32 TAF_STD_ConvertAsciiAddrToBcd(
    MN_MSG_ASCII_ADDR_STRU             *pstAsciiAddr,
    MN_MSG_BCD_ADDR_STRU               *pstBcdAddr
);
VOS_UINT32  TAF_STD_ConvertAsciiCodeToBcd(
    VOS_CHAR                            cAsciiCode,
    VOS_UINT8                          *pucBcdCode
);
VOS_UINT8 TAF_STD_ConvertDeciDigitToBcd(
    VOS_UINT8                           ucDeciDigit,
    VOS_BOOL                            bReverseOrder
);
VOS_UINT32 TAF_STD_ConvertBcdToDeciDigit(
    VOS_UINT8                           ucBcdDigit,
    VOS_BOOL                            bReverseOrder,
    VOS_UINT8                          *pucDigit
);
VOS_UINT32  TAF_STD_ConvertAsciiToDefAlpha(
    const VOS_UINT8                    *pucAsciiChar,
    VOS_UINT32                          ulLen,
    VOS_UINT8                          *pucDefAlpha
);
VOS_VOID  TAF_STD_ConvertDefAlphaToAscii(
    const VOS_UINT8                    *pucDefAlpha,
    VOS_UINT32                          ulDefAlphaLen,
    VOS_UINT8                          *pucAsciiChar,
    VOS_UINT32                         *pulAsciiCharLen
);

/* Added by s00217060 for VoLTE_PhaseIII  ????, 2013-12-16, end */

VOS_UINT32  TAF_STD_ConvertBcdCodeToDtmf(
    VOS_UINT8                           ucBcdCode,
    VOS_UINT8                          *pucDtmfCode
);

VOS_UINT32  TAF_STD_ConvertBcdNumberToDtmf(
    const VOS_UINT8                    *pucBcdNumber,
    VOS_UINT8                           ucBcdLen,
    VOS_UINT8                          *pucDtmfNumber
);

extern VOS_UINT32 TAF_STD_ConvertTimeFromSecsToTimeZone
(
    VOS_UINT32                          ulHighSystemTime,
    VOS_UINT32                          ulLowSystemTime,
    TAF_STD_TIME_ZONE_TYPE_STRU        *pstUniversalTimeandLocalTimeZone
);

extern VOS_VOID TAF_STD_ConvertSystemTimeToHighLow(
    VOS_UINT8                          *pucSysTimeByte,
    VOS_UINT32                         *pulHighSystemTime,
    VOS_UINT32                         *pulLowSystemTime
);

extern VOS_UINT32 TAF_STD_64Add32
(
    VOS_UINT32                          ulHighAddend,
    VOS_UINT32                          ulLowAddend,
    VOS_UINT32                          ulAddFactor,
    VOS_UINT32                         *pulHighRslt,
    VOS_UINT32                         *pulLowRslt
);

extern VOS_UINT32 TAF_STD_64Sub32
(
    VOS_UINT32                          ulHighMinuend,
    VOS_UINT32                          ulLowMinuend,
    VOS_UINT32                          ulSubFactor,
    VOS_UINT32                         *pulHighRslt,
    VOS_UINT32                         *pulLowRslt
);


VOS_UINT16 TAF_STD_TransformBcdMccToDeciDigit(
    VOS_UINT32                          ulBcdMcc
);

VOS_UINT8 TAF_STD_TransformBcdImsi1112ToDeciDigit(
    VOS_UINT16                          usBcdImsi1112
);

VOS_UINT16 TAF_STD_TransformBcdMncToDeciDigit(
    VOS_UINT16                          usBcdMnc
);

VOS_UINT32 TAF_STD_TransformDeciDigitToBcdMcc(
    VOS_UINT32                          ulDeciDigitMcc
);
#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of TafSpmCtx.h */



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

/******************************************************************************
   1 ??????????
******************************************************************************/
#include "v_typdef.h"
#include "ImmInterface.h"
#include "RnicProcMsg.h"
#include "RnicLog.h"
#include "RnicEntity.h"
#include "RnicDebug.h"
#include "RnicCtx.h"
#include "SdioInterface.h"
#include "RnicConfigInterface.h"
#include "product_config.h"
#include "mdrv.h"
#include "PppRnicInterface.h"
#include "RnicSndMsg.h"


/*****************************************************************************
    ??????????????????????.C??????????
*****************************************************************************/

#define THIS_FILE_ID PS_FILE_ID_RNIC_ENTITY_C

/******************************************************************************
   2 ????????????????
******************************************************************************/

/******************************************************************************
   3 ????????
******************************************************************************/

/******************************************************************************
   4 ????????????
*****************************************************************************/
VOS_UINT8                               g_ucRnicNetifRxType = 0;


/******************************************************************************
   5 ????????
******************************************************************************/

/*****************************************************************************
 ?? ?? ??  : RNIC_ProcUlDataInPdpActive
 ????????  : ??PDP????????????RNIC????????????????????
 ????????  : pstSkb     :SKBUF??????????
             pstNetCntxt:??????????
             ucRabid    :??????????
             enIpType   :IPv4??IPv6
 ????????  : ??
 ?? ?? ??  : VOS_VOID
 ????????  :
 ????????  :

 ????????      :
  1.??    ??   : 2011??12??13??
    ??    ??   : ????
    ????????   : ??????????
  2.??    ??   : 2012??11??23??
    ??    ??   : f00179208
    ????????   : DSDA Phase I: RNIC??????
  3.??    ??   : 2013??6??3??
    ??    ??   : L47619
    ????????   : V3R3 Share-PDP????????
  4.??    ??   : 2015??5??28??
    ??    ??   : l00198894
    ????????   : TSTS
*****************************************************************************/
VOS_VOID RNIC_SendULDataInPdpActive(
    IMM_ZC_STRU                        *pstImmZc,
    RNIC_SPEC_CTX_STRU                 *pstNetCntxt,
    VOS_UINT8                           ucRabId,
    ADS_PKT_TYPE_ENUM_UINT8             enIpType
)
{
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv   = VOS_NULL_PTR;
    RNIC_RMNET_ID_ENUM_UINT8            enRmNetId;
    VOS_UINT32                          ulDataLen = 0;
    VOS_UINT8                           ucSendAdsRabId;



    pstPriv   = pstNetCntxt->pstPriv;
    enRmNetId = pstNetCntxt->enRmNetId;

    /* ????Modem Id????RabId */
    if (MODEM_ID_0 == pstNetCntxt->enModemId)
    {
        ucSendAdsRabId = ucRabId;
    }
    else if (MODEM_ID_1 == pstNetCntxt->enModemId)
    {
        ucSendAdsRabId = ucRabId | RNIC_RABID_TAKE_MODEM_1_MASK;
    }
    else if (MODEM_ID_2 == pstNetCntxt->enModemId)
    {
        ucSendAdsRabId = ucRabId | RNIC_RABID_TAKE_MODEM_2_MASK;
    }
    else
    {
        IMM_ZcFree(pstImmZc);
        RNIC_DBG_MODEM_ID_UL_DISCARD_NUM(1, enRmNetId);
        pstPriv->stStats.tx_dropped++;
        return;
    }

    ulDataLen = pstImmZc->len;

    if (VOS_OK != ADS_UL_SendPacketEx(pstImmZc, enIpType, ucSendAdsRabId))
    {
        IMM_ZcFree(pstImmZc);
        RNIC_DBG_SEND_UL_PKT_FAIL_NUM(1, enRmNetId);
        pstPriv->stStats.tx_dropped++;
        return;
    }

   RNIC_DBG_SEND_UL_PKT_NUM(1, enRmNetId);

    /* ???????????????? */
    pstPriv->stStats.tx_packets++;
    pstPriv->stStats.tx_bytes += ulDataLen;

    /* ???????????? */
    pstNetCntxt->stDsFlowStats.ulPeriodSendPktNum++;
    pstNetCntxt->stDsFlowStats.ulTotalSendFluxLow += ulDataLen;

    return;
}

/*****************************************************************************
 ?? ?? ??  : RNIC_ProcVoWifiULData
 ????????  :  ????vowifi??????????
 ????????  : ??
 ?? ?? ??  : VOS_VOID
 ????????  :
 ????????  :

 ????????      :
  1.??    ??   : 2015??11??9??
    ??    ??   : n00269697
    ????????   : ??????????

  2.??    ??   : 2016??12??27??
    ??    ??   : A00165503
    ????????   : DTS2016121600573: ????VOWIFI????????
*****************************************************************************/
VOS_VOID RNIC_ProcVoWifiULData(
    struct sk_buff                     *pstSkb,
    RNIC_SPEC_CTX_STRU                 *pstNetCntxt
)
{

    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv = VOS_NULL_PTR;

    /* ??IMS????WIFI????RMNET_IMS??????????????????RNIC??CDS?????????????????? */
    if (VOS_OK == RNIC_SendCdsImsDataReq(pstSkb, pstNetCntxt))
    {
        RNIC_DBG_SEND_UL_PKT_NUM(1, RNIC_RMNET_ID_IMS1);

        /* ???????????????? */
        pstPriv   = pstNetCntxt->pstPriv;

        pstPriv->stStats.tx_packets++;
        pstPriv->stStats.tx_bytes += pstSkb->len;

        /* ???????????? */
        pstNetCntxt->stDsFlowStats.ulPeriodSendPktNum++;
        pstNetCntxt->stDsFlowStats.ulTotalSendFluxLow += pstSkb->len;
    }

    IMM_ZcFree(pstSkb);

    return;
}

/*****************************************************************************
 ?? ?? ??  : RNIC_ProcUlIpv4Data
 ????????  : RNIC????IPV4??????????
 ????????  : pstSkb     :SKBUF????????????
             pstNetCntxt:??????????
 ????????  : ??
 ?? ?? ??  : VOS_VOID
 ????????  :
 ????????  :

 ????????      :
 1.??    ??   : 2011??12??15??
   ??    ??   : ????
   ????????   : ??????????
 2.??    ??   : 2012??8??30??
   ??    ??   : l60609
   ????????   : AP????????????RNIC_TransSkbToImmZC??????????????????????
 3.??    ??   : 2012??11??23??
   ??    ??   : f00179208
   ????????   : DSDA Phase I: RNIC??????

  4.??    ??   : 2016??12??27??
    ??    ??   : A00165503
    ????????   : DTS2016121600573: ????VOWIFI????????
*****************************************************************************/
VOS_VOID RNIC_SendULIpv4Data(
    struct sk_buff                     *pstSkb,
    RNIC_SPEC_CTX_STRU                 *pstNetCntxt
)
{
    IMM_ZC_STRU                        *pstImmZc = VOS_NULL_PTR;
    RNIC_RMNET_ID_ENUM_UINT8            enRmNetId;
    VOS_UINT8                           ucRabId;
    VOS_UINT32                          ulNonEmpty = VOS_FALSE;

    pstImmZc  = (IMM_ZC_STRU *)pstSkb;
    enRmNetId = pstNetCntxt->enRmNetId;

    /* ??IMS????WIFI????RMNET_IMS??????????????????RNIC??CDS?????????????????? */
    if (RNIC_RMNET_ID_IMS1 == pstNetCntxt->enRmNetId)
    {
        if (0 == IMM_ZcQueueLen(&(pstNetCntxt->stPdpCtx.stImsQue)))
        {
            ulNonEmpty = VOS_TRUE;
        }

        IMM_ZcQueueTail(&(pstNetCntxt->stPdpCtx.stImsQue), pstSkb);

        if (VOS_TRUE == ulNonEmpty)
        {
            RNIC_TrigImsDataProcEvent(pstNetCntxt->enRmNetId);
        }

        return;
    }

    /* ??????????????RABID */
    ucRabId = RNIC_GET_SPEC_NET_IPV4_RABID(enRmNetId);
    if (RNIC_RAB_ID_INVALID == ucRabId)
    {
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);
        RNIC_DBG_RAB_ID_ERR_NUM(1, enRmNetId);
        return;
    }

    /* PDP?????????????????????? */
    RNIC_SendULDataInPdpActive(pstImmZc, pstNetCntxt, ucRabId, ADS_PKT_TYPE_IPV4);

    return;
}

/*****************************************************************************
 ?? ?? ??  : RNIC_ProcUlIpv6Data
 ????????  : RNIC????IPV6??????????
 ????????  : pstSkb     : SKBUF????????????
             pstNetCntxt: ??????????????
 ????????  : ??
 ?? ?? ??  : VOS_VOID
 ????????  :
 ????????  :

 ????????      :
 1.??    ??   : 2011??12??15??
   ??    ??   : ????
   ????????   : ??????????
 2.??    ??   : 2012??8??30??
   ??    ??   : l60609
   ????????   : AP????????????RNIC_TransSkbToImmZC??????????????????????
 3.??    ??   : 2012??11??23??
   ??    ??   : f00179208
   ????????   : DSDA Phase I: RNIC??????

  4.??    ??   : 2016??12??27??
    ??    ??   : A00165503
    ????????   : DTS2016121600573: ????VOWIFI????????
*****************************************************************************/
VOS_VOID RNIC_SendULIpv6Data(
    struct sk_buff                     *pstSkb,
    RNIC_SPEC_CTX_STRU                 *pstNetCntxt
)
{
    IMM_ZC_STRU                        *pstImmZc = VOS_NULL_PTR;
    RNIC_RMNET_ID_ENUM_UINT8            enRmNetId;
    VOS_UINT8                           ucRabId;
    VOS_UINT32                          ulNonEmpty = VOS_FALSE;

    pstImmZc  = (IMM_ZC_STRU *)pstSkb;
    enRmNetId = pstNetCntxt->enRmNetId;

    /* ??IMS????WIFI????RMNET_IMS??????????????????RNIC??CDS?????????????????? */
    if (RNIC_RMNET_ID_IMS1 == pstNetCntxt->enRmNetId)
    {
        if (0 == IMM_ZcQueueLen(&(pstNetCntxt->stPdpCtx.stImsQue)))
        {
            ulNonEmpty = VOS_TRUE;
        }

        IMM_ZcQueueTail(&(pstNetCntxt->stPdpCtx.stImsQue), pstSkb);

        if (VOS_TRUE == ulNonEmpty)
        {
            RNIC_TrigImsDataProcEvent(enRmNetId);
        }

        return;
    }

    /* ??????????????RABID */
    ucRabId = RNIC_GET_SPEC_NET_IPV6_RABID(enRmNetId);
    if (RNIC_RAB_ID_INVALID == ucRabId)
    {
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);
        RNIC_DBG_RAB_ID_ERR_NUM(1, enRmNetId);
        return;
    }

    /* PDP?????????????????????? */
    RNIC_SendULDataInPdpActive(pstImmZc, pstNetCntxt, ucRabId, ADS_PKT_TYPE_IPV6);

    return;
}

/*****************************************************************************
 ?? ?? ??  : RNIC_ProcDemDial
 ????????  : RNIC????????????
 ????????  : struct sk_buff  *pstSkb,
             pBuf -- ????????????????ID

 ????????  : ??
 ?? ?? ??  : OK
             ERROR
 ????????  :
 ????????  :

 ????????      :
  1.??    ??   : 2010??1??19??
    ??    ??   : ????
    ????????   : ??????????

  2.??    ??   : 2012??6??6??
    ??    ??   : A00165503
    ????????   : DTS2012060502819: ????????????????????, ????????????????

*****************************************************************************/
VOS_UINT32 RNIC_ProcDemDial(
    struct sk_buff                     *pstSkb
)
{
    RNIC_DIAL_MODE_STRU                *pstDialMode;
    RNIC_TIMER_STATUS_ENUM_UINT8        enTiStatus;
    VOS_UINT32                          ulIpAddr;

    /* ????IP???? */
    ulIpAddr = *((VOS_UINT32 *)((pstSkb->data) + RNIC_IP_HEAD_DEST_ADDR_OFFSET));

    /*??????????????????????????????????????????*/
    if (RNIC_IPV4_BROADCAST_ADDR == ulIpAddr)
    {
        RNIC_DBG_UL_RECV_IPV4_BROADCAST_NUM(1, RNIC_RMNET_ID_0);
        return VOS_ERR;
    }

    /* ???????????????????????????????? */
    pstDialMode = RNIC_GetDialModeAddr();

    /* ???????????????????????????? */
    enTiStatus  = RNIC_GetTimerStatus(TI_RNIC_DEMAND_DIAL_PROTECT);

    /*??????????????????????????????????????????????*/
    if (RNIC_TIMER_STATUS_STOP == enTiStatus)
    {
        /* ???????????????????? */
        if (RNIC_ALLOW_EVENT_REPORT == pstDialMode->enEventReportFlag)
        {
            if (VOS_OK == RNIC_SendDialEvent(DEVICE_ID_WAN, RNIC_DAIL_EVENT_UP))
            {
                /* ??????????????????  */
                RNIC_StartTimer(TI_RNIC_DEMAND_DIAL_PROTECT, TI_RNIC_DEMAND_DIAL_PROTECT_LEN);
                RNIC_DBG_SEND_APP_DIALUP_SUCC_NUM(1, RNIC_RMNET_ID_0);
                RNIC_NORMAL_LOG(ACPU_PID_RNIC, "RNIC_ProcDemDial: Send dial event succ.");
            }
            else
            {
                RNIC_DBG_SEND_APP_DIALUP_FAIL_NUM(1, RNIC_RMNET_ID_0);
                RNIC_WARNING_LOG(ACPU_PID_RNIC, "RNIC_ProcDemDial: Send dial event fail.");
            }

            RNIC_MNTN_TraceDialConnEvt();
        }
    }

    return VOS_OK;
}

/* Modified by l60609 for L-C??????????, 2014-1-14, begin */
/*****************************************************************************
 ?? ?? ??  : RNIC_RcvOutsideModemUlData
 ????????  : CDMA??????????TCP/IP????????????????
 ????????  : struct sk_buff                     *pstSkb
             RNIC_SPEC_CTX_STRU                 *pstNetCntxt
 ????????  : ??
 ?? ?? ??  : ??
 ????????  :
 ????????  :

 ????????      :
  1.??    ??   : 2014??1??7??
    ??    ??   : m00217266
    ????????   : ??????????

*****************************************************************************/
VOS_VOID RNIC_RcvOutsideModemUlData(
    struct sk_buff                     *pstSkb,
    RNIC_SPEC_CTX_STRU                 *pstNetCntxt
)
{
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv = VOS_NULL_PTR;
    VOS_UINT16                          usEthType;
    VOS_UINT8                           ucPdnId;
    VOS_ULONG                           ulRslt;
    VOS_UINT32                          ulDataLen;
    RNIC_RMNET_ID_ENUM_UINT8            enRmNetId;

    pstPriv     = pstNetCntxt->pstPriv;
    enRmNetId   = pstNetCntxt->enRmNetId;
    ulDataLen   = pstSkb->len;
    ulRslt      = SDIO_ERR;

    /* ?????????????? */
    usEthType   = VOS_NTOHS(((RNIC_ETH_HEADER_STRU *)(pstSkb->data))->usEtherType);

    /* IP???????????? */
    switch(usEthType)
    {
        case RNIC_ETH_TYPE_IP:
            ucPdnId = RNIC_GET_SPEC_NET_IPV4_PDNID(enRmNetId);
            break;

        case RNIC_ETH_TYPE_IPV6:
            ucPdnId = RNIC_GET_SPEC_NET_IPV6_PDNID(enRmNetId);
            break;

        default:
            IMM_ZcFree((IMM_ZC_STRU *)pstSkb);
            RNIC_DBG_RECV_UL_ERR_PKT_NUM(1, enRmNetId);
            pstPriv->stStats.tx_dropped++;
            return;
    }

    /* ????Pdn Id */
    if (RNIC_PDN_ID_INVALID == ucPdnId)
    {
        IMM_ZcFree((IMM_ZC_STRU *)pstSkb);
        RNIC_DBG_PDN_ID_ERR_NUM(1, enRmNetId);
        return;
    }

    /* ????MAC?? */
    if (VOS_OK != IMM_ZcRemoveMacHead(pstSkb))
    {
        IMM_ZcFree(pstSkb);
        RNIC_DBG_UL_RMV_MAC_HDR_FAIL_NUM(1, enRmNetId);
        return;
    }

    /* ??SDIO */
    ulRslt = SDIO_UL_SendPacket(pstSkb, ucPdnId);
    if (SDIO_OK != ulRslt)
    {
        IMM_ZcFree(pstSkb);
        RNIC_DBG_SEND_UL_PKT_FAIL_NUM(1, enRmNetId);
        pstPriv->stStats.tx_dropped++;
    }
    else
    {
        RNIC_DBG_SEND_UL_PKT_NUM(1, enRmNetId);
        pstPriv->stStats.tx_packets++;
        pstPriv->stStats.tx_bytes += ulDataLen;
    }

    return;
}
/*****************************************************************************
 ?? ?? ??  : RNIC_RcvInsideModemUlData
 ????????  : ??????TCP/IP??????3GPP??????????
 ????????  : struct sk_buff                     *pstSkb
             RNIC_RMNET_ID_ENUM_UINT8            enRmNetId
 ????????  : ??
 ?? ?? ??  : ??
 ????????  :
 ????????  :

 ????????      :
  1.??    ??   : 2014??1??7??
    ??    ??   : m00217266
    ????????   : ??????????

*****************************************************************************/
VOS_VOID RNIC_RcvInsideModemUlData(
    struct sk_buff                     *pstSkb,
    RNIC_SPEC_CTX_STRU                 *pstNetCntxt
)
{
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv = VOS_NULL_PTR;
    VOS_UINT16                          usEthType;
    RNIC_RMNET_ID_ENUM_UINT8            enRmNetId;

    pstPriv     = pstNetCntxt->pstPriv;
    enRmNetId   = pstNetCntxt->enRmNetId;

    /* ?????????????? */
    usEthType   = VOS_NTOHS(((RNIC_ETH_HEADER_STRU *)(pstSkb->data))->usEtherType);

    /* ???????? */
    if (RNIC_FLOW_CTRL_STATUS_START == RNIC_GET_FLOW_CTRL_STATUS(enRmNetId))
    {
        IMM_ZcFree(pstSkb);
        RNIC_DBG_FLOW_CTRL_UL_DISCARD_NUM(1, enRmNetId);
        return;
    }

    /* ????MAC?? */
    if (VOS_OK != IMM_ZcRemoveMacHead(pstSkb))
    {
        IMM_ZcFree(pstSkb);
        RNIC_DBG_UL_RMV_MAC_HDR_FAIL_NUM(1, enRmNetId);
        pstPriv->stStats.tx_dropped++;
        return;
    }

    /* ????????0???????????????????? */
    if ((RNIC_ETH_TYPE_IP == usEthType)
     && (RNIC_DIAL_MODE_DEMAND_DISCONNECT == RNIC_GET_DIAL_MODE())
     && (RNIC_PDP_REG_STATUS_DEACTIVE == RNIC_GET_SPEC_NET_IPV4_REG_STATE(enRmNetId))
     && (RNIC_RMNET_ID_0 == enRmNetId))
    {
        RNIC_SPE_MEM_UNMAP(pstSkb, RNIC_SPE_CACHE_HDR_SIZE);

        if (VOS_ERR == RNIC_ProcDemDial(pstSkb))
        {
            RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_StartXmit, the data is discared!");
        }

        RNIC_SPE_MEM_MAP(pstSkb, RNIC_SPE_CACHE_HDR_SIZE);

        IMM_ZcFree(pstSkb);
        return;
    }

    /* IP???????????? */
    switch(usEthType)
    {
        case RNIC_ETH_TYPE_IP:
            RNIC_SendULIpv4Data(pstSkb, pstNetCntxt);
            RNIC_DBG_RECV_UL_IPV4_PKT_NUM(1, enRmNetId);
            break;

        case RNIC_ETH_TYPE_IPV6:
            RNIC_SendULIpv6Data(pstSkb, pstNetCntxt);
            RNIC_DBG_RECV_UL_IPV6_PKT_NUM(1, enRmNetId);
            break;

        default:
            IMM_ZcFree(pstSkb);
            RNIC_DBG_RECV_UL_ERR_PKT_NUM(1, enRmNetId);
            break;
    }

    return;
}
/* Modified by l60609 for L-C??????????, 2014-1-14, end */

/* Modified by l60609 for L-C??????????, 2014-01-06, Begin */
/*****************************************************************************
 ?? ?? ??  : RNIC_ShowSdioDlData
 ????????  : ????sdio??????????????
 ????????  : pstSkb   :SKBUF??????????
 ????????  : ??
 ?? ?? ??  : ??
 ????????  :
 ????????  :

 ????????      :
 1.??    ??   : 2014??03??11??
   ??    ??   : m00217266
   ????????   : ??????????
*****************************************************************************/
VOS_VOID RNIC_ShowSdioDlData(
    VOS_UINT8                           ucPdnId,
    IMM_ZC_STRU                        *pstImmZc
)
{
    VOS_UINT32                          i;

    /* RNIC ????????????????????????mac???????? */
    if (VOS_TRUE == g_ulRnicPrintDlDataFlg)
    {
        PS_PRINTF("RNIC_ShowSdioDlData : dl data pdn id is %d. \r\n", ucPdnId);
        PS_PRINTF("RNIC_ShowSdioDlData : dl data len is %d. \r\n", pstImmZc->len);
        PS_PRINTF("RNIC_ShowSdioDlData : dl data content is: \r\n");

        for (i = 0; i < pstImmZc->len; i++)
        {
            if (pstImmZc->data[i] > 0xf)
            {
                PS_PRINTF("%x", pstImmZc->data[i]);
            }
            else
            {
                PS_PRINTF("0%x", pstImmZc->data[i]);
            }
        }
        PS_PRINTF("\r\n");
    }

    return;
}

/*****************************************************************************
 ?? ?? ??  : RNIC_RcvSdioDlData
 ????????  : ????SDIO??????????
 ????????  : VOS_UINT8                           ucPdnId
             IMM_ZC_STRU                        *pstData
 ????????  : ??
 ?? ?? ??  : VOS_UINT32
 ????????  :
 ????????  :

 ????????      :
  1.??    ??   : 2014??1??6??
    ??    ??   : m00217266
    ????????   : ??????????

*****************************************************************************/
VOS_UINT32 RNIC_RcvSdioDlData(
    VOS_UINT8                           ucPdnId,
    IMM_ZC_STRU                        *pstImmZc
)
{
    VOS_UINT8                           ucNetIndex;
    ADS_PKT_TYPE_ENUM_UINT8             enPktType;
    VOS_UINT8                           ucIpType;
    VOS_UINT32                          ulRet;

    /* ??????????????????????????pdn id */
    RNIC_ShowSdioDlData(ucPdnId, pstImmZc);

    /* ????PDNid??????????RMNETid */
    ucNetIndex     = RNIC_GET_RM_NET_ID_BY_PDN_ID(ucPdnId);

    /* ?????????????? */
    ucIpType    = RNIC_GET_IP_VERSION(pstImmZc->data[0]);

    if (RNIC_IPV4_VERSION == ucIpType)
    {
        enPktType = ADS_PKT_TYPE_IPV4;
    }
    else if (RNIC_IPV6_VERSION == ucIpType)
    {
        enPktType = ADS_PKT_TYPE_IPV6;
    }
    else    /* ?????????????????????????????? */
    {
        RNIC_DBG_RECV_DL_ERR_PKT_NUM(1, ucNetIndex);
        RNIC_ERROR_LOG1(ACPU_PID_RNIC, "RNIC_RcvSdioDlData, Ip Type is !", ucIpType);

        /* ???????? */
        IMM_ZcFree(pstImmZc);

        return RNIC_PKT_TYPE_INVAL;
    }

    ulRet = RNIC_SendDlData(ucNetIndex, pstImmZc, enPktType);
    return ulRet;
}

/*****************************************************************************
 ?? ?? ??  : RNIC_RcvAdsDlData
 ????????  : RNIC????ADS????????
 ????????  : ucRabid  :??????????
             pstData  :SKBUF????????????
             enPdpType:PDP ????
 ????????  : ??
 ?? ?? ??  : VOS_UINT32
 ????????  :
 ????????  :

 ????????      :
  1.??    ??   : 2014??2??10??
    ??    ??   : m00217266
    ????????   : ??????????

  2.??    ??   : 2015??5??28??
    ??    ??   : l00198894
    ????????   : TSTS
*****************************************************************************/
VOS_UINT32  RNIC_RcvAdsDlData(
    VOS_UINT8                           ucExRabid,
    IMM_ZC_STRU                        *pstImmZc,
    ADS_PKT_TYPE_ENUM_UINT8             enPktType,
    VOS_UINT32                          ulExParam
)
{
    VOS_UINT32                          ulRet;
    VOS_UINT8                           ucRmNetId;

    ucRmNetId = RNIC_GET_RMNETID_FROM_EXPARAM(ulExParam);

    ulRet = RNIC_SendDlData(ucRmNetId, pstImmZc, enPktType);

    return ulRet;
}

/*****************************************************************************
 ?? ?? ??  : RNIC_SendDlData
 ????????  : RNIC_SendDlData????????????????????
 ????????  : enRmNetId  :????id
             pstData    :SKBUF????????????
             enPdpType  :PDP ????
 ????????  : ??
 ?? ?? ??  :
 ????????  : VOS_UINT32:RNIC_OK, RNIC_ERROR
 ????????  :

 ????????     :
  1.??    ??   : 2011??12??06??
    ??    ??   : ????
    ????????   : ??????????
  2.??    ??   : 2011??06??08??
    ??    ??   : zhangyizhan 60575
    ????????   : DTS2012060708396????????????
  3.??    ??   : 2011??06??09??
    ??    ??   : zhangyizhan 60575
    ????????   : DTS2012060902909??????????????,V3R2/V7R1????????netif_rx
  4.??    ??   : 2012??6??20??
    ??    ??   : A00165503
    ????????   : DTS2012061904440: ??????????????????
  5.??    ??   : 2012??6??20??
    ??    ??   : f00179208
    ????????   : DTS2012070306267: ??????????????????????????????????
  6.??    ??   : 2012??11??23??
    ??    ??   : f00179208
    ????????   : DSDA Phase I: RNIC??????
  7.??    ??   : 2014??6??13??
    ??    ??   : A00165503
    ????????   : DTS2014052607108: ??????????????????????????
*****************************************************************************/
VOS_UINT32 RNIC_SendDlData(
    RNIC_RMNET_ID_ENUM_UINT8            enRmNetId,
    IMM_ZC_STRU                        *pstImmZc,
    ADS_PKT_TYPE_ENUM_UINT8             enPktType
)
{
    RNIC_SPEC_CTX_STRU                 *pstNetCntxt = VOS_NULL_PTR;

    if (!RNIC_RMNET_IS_VALID(enRmNetId))
    {
        IMM_ZcFree(pstImmZc);
        return RNIC_INVAL;
    }

    /* ?????????????? */
    pstNetCntxt = RNIC_GET_SPEC_NET_CTX(enRmNetId);


    return RNIC_NetRxData(pstNetCntxt, pstImmZc, enPktType);
}
/* Modified by l60609 for L-C??????????, 2014-01-06, End */

/*****************************************************************************
 ?? ?? ??  : RNIC_StartFlowCtrl
 ????????  : ????????????
 ????????  : ucRmNetId : ????ID
 ????????  : ??
 ?? ?? ??  : VOS_OK     - ????????????
             VOS_ERR    - ????????????
 ????????  :
 ????????  :

 ????????      :
 1.??    ??   : 2012??5??31??
   ??    ??   : A00165503
   ????????   : ??????????
 2.??    ??   : 2012??11??23??
   ??    ??   : f00179208
   ????????   : DSDA Phase I: RNIC??????
*****************************************************************************/
unsigned int RNIC_StartFlowCtrl(unsigned char ucRmNetId)
{
    RNIC_SET_FLOW_CTRL_STATUS(RNIC_FLOW_CTRL_STATUS_START, ucRmNetId);
    return VOS_OK;
}

/*****************************************************************************
 ?? ?? ??  : RNIC_StopFlowCtrl
 ????????  : ????????????
 ????????  : ucRmNetId : ????ID
 ????????  : ??
 ?? ?? ??  : VOS_OK     - ????????????
             VOS_ERR    - ????????????
 ????????  :
 ????????  :

 ????????      :
 1.??    ??   : 2012??5??31??
   ??    ??   : A00165503
   ????????   : ??????????
 2.??    ??   : 2012??11??23??
   ??    ??   : f00179208
   ????????   : DSDA Phase I: RNIC??????
*****************************************************************************/
unsigned int RNIC_StopFlowCtrl(unsigned char ucRmNetId)
{
    RNIC_SET_FLOW_CTRL_STATUS(RNIC_FLOW_CTRL_STATUS_STOP, ucRmNetId);
    return VOS_OK;
}

/* Added by m00217266 for L-C??????????, 2014-1-21, begin */
/*****************************************************************************
 ?? ?? ??  : RNIC_ConfigRmnetStatus
 ????????  : ????????????????????????????????
 ????????  : ??
 ????????  : ??
 ?? ?? ??  : unsigned long
 ????????  :
 ????????  :

 ????????      :
  1.??    ??   : 2014??1??21??
    ??    ??   : m00217266
    ????????   : ??????????

*****************************************************************************/
unsigned long RNIC_ConfigRmnetStatus(
    RNIC_RMNET_CONFIG_STRU             *pstConfigInfo
)
{
    /*??????????????????????*/
    RNIC_MNTN_SndRmnetConfigInfoMsg(pstConfigInfo);
    RNIC_DBG_CONFIGCHECK_ADD_TOTLA_NUM();

    /* ???????? */
    /* ????modem???????? rab id???????? */
    if (RNIC_MODEM_TYPE_INSIDE == pstConfigInfo->enModemType)
    {
        if (!RNIC_RAB_ID_IS_VALID(pstConfigInfo->ucRabId))
        {
            RNIC_DBG_CONFIGCHECK_ADD_RABID_ERR_NUM();
            RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ConfigRmnetStatus: invaild RAB id !");
            return VOS_ERR;
        }
    }
    /* ????modem???????? pdn id???????? */
    else if (RNIC_MODEM_TYPE_OUTSIDE == pstConfigInfo->enModemType)
    {
        if (!RNIC_PDN_ID_IS_VALID(pstConfigInfo->ucPdnId))
        {
            RNIC_DBG_CONFIGCHECK_ADD_PDNID_ERR_NUM();
            RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ConfigRmnetStatus: invaild Pdn id !");
            return VOS_ERR;
        }
    }
    /* ????MODEM TYPE */
    else
    {
        RNIC_DBG_CONFIGCHECK_ADD_MODEMTYPE_ERR_NUM();
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ConfigRmnetStatus: invaild modem type!");
        return VOS_ERR;
    }

    /* ????????????????????????error */
    if (!RNIC_RMNET_STATUS_IS_VALID(pstConfigInfo->enRmnetStatus))
    {
        RNIC_DBG_CONFIGCHECK_ADD_RMNETSTATUS_ERR_NUM();
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ConfigRmnetStatus: invaild Rmnet Status !");
        return VOS_ERR;
    }

    /* IP????????????????error */
    if (!RNIC_IP_TYPE_IS_VALID(pstConfigInfo->enIpType))
    {
        RNIC_DBG_CONFIGCHECK_ADD_IPTYPE_ERR_NUM();
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ConfigRmnetStatus: invaild IP type !");
        return VOS_ERR;
    }

    /* ???????????? */
    if (VOS_OK != RNIC_SndRnicRmnetConfigReq(pstConfigInfo))
    {
        RNIC_DBG_CONFIGCHECK_ADD_SND_ERR_NUM();
        RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_ConfigRmnetStatus: send pdp status ind fail !");
        return VOS_ERR;
    }

    RNIC_DBG_CONFIGCHECK_ADD_SUCC_NUM();
    return VOS_OK;
}
/* Added by m00217266 for L-C??????????, 2014-1-21, end */

/*****************************************************************************
 ?? ?? ??  : RNIC_NetRxData
 ????????  : ??IP??????????????
 ????????  : pstNetCntxt --- ??????????
             pstImmZc    --- ??????
             enPktType   --- ????????
 ????????  : ??
 ?? ?? ??  : RNIC_RESULT_TYPE_ENUM
 ????????  :
 ????????  :

 ????????      :
  1.??    ??   : 2015??3??30??
    ??    ??   : A00165503
    ????????   : ??????????

  2.??    ??   : 2016??3??31??
    ??    ??   : A00165503
    ????????   : DTS2016032501658: SPE????????????????????????, ????????????
                 ????????, ????????????????

*****************************************************************************/
VOS_UINT32 RNIC_NetRxData(
    RNIC_SPEC_CTX_STRU                 *pstNetCntxt,
    IMM_ZC_STRU                        *pstImmZc,
    ADS_PKT_TYPE_ENUM_UINT8             enPktType
)
{
    RNIC_NETCARD_DEV_INFO_STRU         *pstPriv    = VOS_NULL_PTR;
    VOS_UINT8                          *pucAddData = VOS_NULL_PTR;
    RNIC_RMNET_ID_ENUM_UINT8            enRmNetId;
    VOS_INT32                           lRet;

    pstPriv     = pstNetCntxt->pstPriv;
    enRmNetId   = pstNetCntxt->enRmNetId;


    /* ???????????? */
    if (VOS_NULL_PTR == pstPriv)
    {
        IMM_ZcFree(pstImmZc);
        RNIC_DBG_NETCAED_DL_DISCARD_NUM(1, enRmNetId);
        return RNIC_INVAL;
    }

    /* ?????????? */
    if (RNIC_NETCARD_STATUS_CLOSED == pstPriv->enStatus)
    {
        IMM_ZcFree(pstImmZc);
        RNIC_DBG_DISCARD_DL_PKT_NUM(1, enRmNetId);
        pstPriv->stStats.rx_dropped++;
        return RNIC_OK;
    }

    /* ??????????????????(??????MAC??) */
    if ((pstImmZc->len) > RNIC_MAX_PACKET)
    {
        IMM_ZcFree(pstImmZc);
        RNIC_DBG_RECV_DL_BIG_PKT_NUM(1, enRmNetId);
        pstPriv->stStats.rx_errors++;
        pstPriv->stStats.rx_length_errors++;
        return RNIC_OK;
    }

    /* ????MAC??????????ImmZc??????MAC????????ImmZc?? */
    if (ADS_PKT_TYPE_IPV4 == enPktType)
    {
        pucAddData = (VOS_UINT8*)&g_astRnicManageTbl[enRmNetId].stIpv4Ethhead;
        RNIC_DBG_RECV_DL_IPV4_PKT_NUM(1, enRmNetId);
    }
    else if (ADS_PKT_TYPE_IPV6 == enPktType)
    {
        pucAddData = (VOS_UINT8*)&g_astRnicManageTbl[enRmNetId].stIpv6Ethhead;
        RNIC_DBG_RECV_DL_IPV6_PKT_NUM(1, enRmNetId);
    }
    else   /* ?????????????????????????????? */
    {
        IMM_ZcFree(pstImmZc);
        RNIC_DBG_RECV_DL_ERR_PKT_NUM(1, enRmNetId);
        return RNIC_PKT_TYPE_INVAL;
    }

    if (VOS_OK != IMM_ZcAddMacHead(pstImmZc, pucAddData))
    {
        IMM_ZcFree(pstImmZc);
        RNIC_DBG_ADD_DL_MACHEAD_FAIL_NUM(1, enRmNetId);
        return RNIC_ADDMAC_FAIL;
    }

    pstImmZc->protocol = eth_type_trans(pstImmZc, pstPriv->pstDev);

    /* ???????????????????? */
    pstPriv->stStats.rx_packets++;
    pstPriv->stStats.rx_bytes += pstImmZc->len;

    /* ?????????????????????????????????????? */
    pstNetCntxt->stDsFlowStats.ulTotalRecvFluxLow += pstImmZc->len;

    if (RNIC_NETIF_RX_NI == RNIC_GET_NETIF_RX_TYPE())
    {
        lRet = netif_rx_ni(pstImmZc);
    }
    else
    {
        lRet = netif_rx(pstImmZc);
    }

    if (NET_RX_SUCCESS != lRet)
    {
        RNIC_DBG_SEND_DL_PKT_FAIL_NUM(1, enRmNetId);
        pstPriv->stStats.rx_dropped++;
        return RNIC_RX_PKT_FAIL;
    }

    /* ???????????????????? */
    RNIC_DBG_SEND_DL_PKT_NUM(1, enRmNetId);

    return RNIC_OK;
}

/*****************************************************************************
 ?? ?? ??  : RNIC_ProcessTxDataByModemType
 ????????  : ????MODEM????????????????
 ????????  : pstNetCntxt --- ??????????
             pstSkb      --- ??????
 ????????  : ??
 ?? ?? ??  : VOS_VOID
 ????????  :
 ????????  :

 ????????      :
  1.??    ??   : 2015??3??30??
    ??    ??   : A00165503
    ????????   : ??????????
*****************************************************************************/
VOS_VOID RNIC_ProcessTxDataByModemType(
    RNIC_SPEC_CTX_STRU                 *pstNetCntxt,
    struct sk_buff                     *pstSkb
)
{
    RNIC_DBG_PRINT_UL_DATA(pstSkb);

    if (RNIC_MODEM_TYPE_INSIDE == pstNetCntxt->enModemType)
    {
        RNIC_RcvInsideModemUlData(pstSkb, pstNetCntxt);
    }
    else if (RNIC_MODEM_TYPE_OUTSIDE == pstNetCntxt->enModemType)
    {
        RNIC_RcvOutsideModemUlData(pstSkb, pstNetCntxt);
    }
    else
    {
        IMM_ZcFree(pstSkb);
    }

    return;
}





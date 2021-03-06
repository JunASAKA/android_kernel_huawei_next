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



/*****************************************************************************
  1 ??????????
**************************************************************************** */
#include "OmCommonPpm.h"
#include "cpm.h"
#include "omprivate.h"
#include "SCMProc.h"
#include "diag_debug.h"
#include "msp_diag.h"
#include "diag_common.h"



/*lint -e767 */
#define    THIS_FILE_ID        PS_FILE_ID_OM_COMMON_PPM_C
/*lint +e767 */

/* ****************************************************************************
  2 ????????????
**************************************************************************** */
/* ????ACPU??USB??????UDI???? */
UDI_HANDLE                              g_astOMPortUDIHandle[OM_PORT_HANDLE_BUTT];

/* USB??????OM IND?????????????????????????????????????? */
OM_PSEUDO_SYNC_STRU                     g_stUsbIndPseudoSync;

/* USB??????OM CNF?????????????????????????????????????? */
OM_PSEUDO_SYNC_STRU                     g_stUsbCfgPseudoSync;

VOS_UINT32                              g_ulUSBSendErrCnt   = 0;

/* ???????????????????????? */
PPM_PORT_CFG_INFO_STRU                  g_stPpmPortSwitchInfo;

/* ??????????????AT?????????????????????????? */
VOS_SPINLOCK                            g_stPpmPortSwitchSpinLock;

VOS_BOOL                                g_bPpmLogCompress   = VOS_FALSE;


OM_ACPU_DEBUG_INFO                      g_stAcpuDebugInfo;

VOS_UINT32                              g_ulOmAcpuDbgFlag = VOS_FALSE;

/*****************************************************************************
  3 ????????????
*****************************************************************************/
extern OM_CHANNLE_PORT_CFG_STRU         g_stPortCfg;
extern VOS_SPINLOCK                     g_stScmSoftDecodeDataRcvSpinLock;
extern VOS_SPINLOCK                     g_stCbtScmDataRcvSpinLock;
/*****************************************************************************
  4 ????????
*****************************************************************************/

/*****************************************************************************
 ?? ?? ??  : PPM_DisconnectGUPort
 ????????  : GU????OM????
 ????????  : ucCpuId:CpuId
 ????????  : ??
 ?? ?? ??  : VOS_ERR/VOS_OK
 ????????  :
   1.??    ??  : 2014??05??26??
     ??    ??  : h59254
     ????????  : Creat Function
*****************************************************************************/
VOS_UINT32 PPM_DisconnectGUPort(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel)
{

    return VOS_OK;
}

/*****************************************************************************
 ?? ?? ??  : PPM_DisconnectTLPort
 ????????  : TL????OM????
 ????????  : ??
 ????????  : ??
 ?? ?? ??  : VOS_ERR/VOS_OK
 ????????  :
   1.??    ??  : 2014??05??26??
     ??    ??  : h59254
     ????????  : Creat Function
*****************************************************************************/
VOS_UINT32 PPM_DisconnectTLPort(VOS_VOID)
{
    DIAG_DATA_MSG_STRU                 *pstMsg;

    pstMsg = (DIAG_DATA_MSG_STRU *)VOS_AllocMsg(MSP_PID_DIAG_APP_AGENT, sizeof(DIAG_DATA_MSG_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_ERR;
    }

    pstMsg->ulReceiverPid = MSP_PID_DIAG_APP_AGENT;
    pstMsg->ulMsgId       = ID_MSG_DIAG_HSO_DISCONN_IND;

    (VOS_VOID)VOS_SendMsg(MSP_PID_DIAG_APP_AGENT, pstMsg);

    return VOS_OK;
}

/*****************************************************************************
 ?? ?? ??  : PPM_DisconnectAllPort
 ????????  : GUTL????OM????
 ????????  : ??
 ????????  : ??
 ?? ?? ??  : VOS_ERR/VOS_OK
 ????????  :
   1.??    ??  : 2014??05??26??
     ??    ??  : h59254
     ????????  : Creat Function
*****************************************************************************/
VOS_VOID PPM_DisconnectAllPort(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel)
{
    PPM_DisconnectTLPort();

    PPM_DisconnectGUPort(enChannel);

    return;
}

/*****************************************************************************
 ?? ?? ??  : PPM_GetSendDataLen
 ????????  : PPM??????????????????????
 ????????  : ulDataLen: SOCP??????????????
             enChanID:  SOCP channel ID
 ????????  : pulSendDataLen:??????????????????????
             penPhyport:??????????????
 ?? ?? ??  : VOS_VOID

 ????????      :
  1.??    ??   : 2014??5??25??
    ??    ??   : h59254
    ????????   : V8R1 OM_Optimize????????
*****************************************************************************/
VOS_VOID PPM_GetSendDataLen(SOCP_CODER_DST_ENUM_U32 enChanID, VOS_UINT32 ulDataLen, VOS_UINT32 *pulSendDataLen, CPM_PHY_PORT_ENUM_UINT32 *penPhyport)
{
    CPM_PHY_PORT_ENUM_UINT32    enPhyport;

    if (SOCP_CODER_DST_OM_CNF == enChanID)
    {
        enPhyport = CPM_QueryPhyPort(CPM_OM_CFG_COMM);
    }
    else
    {
        enPhyport = CPM_QueryPhyPort(CPM_OM_IND_COMM);
    }


    /*????????????USB????????????????60k????????????????????????*/
    if (((CPM_IND_PORT == enPhyport) || (CPM_CFG_PORT == enPhyport))
        &&(ulDataLen > USB_MAX_DATA_LEN))
    {
        /*lint -e534*/
        LogPrint1("PPM_GetSendDataLen: Change the send Data Len %d", (VOS_INT)ulDataLen);
        /*lint +e534*/

        *pulSendDataLen = USB_MAX_DATA_LEN;
    }
    else
    {
        *pulSendDataLen = ulDataLen;  /*????????????????????????????????????sd??wifi*/
    }

    *penPhyport = enPhyport;
    return;
}

/*****************************************************************************
 ?? ?? ??  : PPM_PortStatus
 ????????  : ????ACPU????????????????????????OM????????
 ????????  : enHandle:   UDI????????
             enPhyPort:  ????????
             enPortState:????????
 ????????  : ??
 ?? ?? ??  : ??

 ????????      :
  1.??    ??   : 2014??5??25??
    ??    ??   : h59254
    ????????   : V8R1 OM_Optimize????????
*****************************************************************************/
VOS_VOID PPM_PortStatus(OM_PROT_HANDLE_ENUM_UINT32 enHandle, CPM_PHY_PORT_ENUM_UINT32 enPhyPort,ACM_EVT_E enPortState)
{
    VOS_ULONG                           ulLockLevel;
    OM_LOGIC_CHANNEL_ENUM_UINT32        enChannel;
    VOS_BOOL                            ulSndMsg;
    VOS_UINT32 ulSlicehigh, ulSlicelow;
    

    if (ACM_EVT_DEV_SUSPEND == enPortState)
    {
        (VOS_VOID)mdrv_timer_get_accuracy_timestamp(&ulSlicehigh, &ulSlicelow);

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutNum++;

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutTime = ulSlicelow;

        diag_printf("Receive USB disconnect (chan %d) callback at slice 0x%x %x!\n",enHandle, ulSlicehigh, ulSlicelow);

        VOS_SpinLockIntLock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

        ulSndMsg  = VOS_FALSE;
        enChannel = OM_LOGIC_CHANNEL_BUTT;

        /* CFG????????GU??TL????????????????????GU??TL????????????????CPM?????? */
        if (OM_USB_CFG_PORT_HANDLE == enHandle)
        {
            if (enPhyPort == CPM_QueryPhyPort(CPM_OM_CFG_COMM))
            {
                ulSndMsg  = VOS_TRUE;
                enChannel = OM_LOGIC_CHANNEL_CNF;
            }
        }
        /* IND??????????????????GU??TL?????? */
        else if (OM_USB_IND_PORT_HANDLE == enHandle)
        {
            if (enPhyPort == CPM_QueryPhyPort(CPM_OM_IND_COMM))
            {
                ulSndMsg  = VOS_TRUE;
                enChannel = OM_LOGIC_CHANNEL_IND;
            }
        }
        else
        {

        }

        VOS_SpinUnlockIntUnlock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

        if (VOS_TRUE == ulSndMsg)
        {
            PPM_DisconnectAllPort(enChannel);
        }
    }
    else if(ACM_EVT_DEV_READY == enPortState)
    {
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINNum++;

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINTime = mdrv_timer_get_normal_timestamp();
    }
    else
    {
        /*lint -e534*/
        LogPrint2("PPM_PortStatus: The USB Port %d State %d is Unknow", (VOS_INT)enPhyPort, (VOS_INT)enPortState);
        /*lint +e534*/

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrNum++;

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrTime = mdrv_timer_get_normal_timestamp();
    }

    return;
}

/*****************************************************************************
 ?? ?? ??  : PPM_PortCloseProc
 ????????  : OM??????????????????????USB????
 ????????  : enHandle: ??????????????????????????????????VOS_ERROR
             enPhyPort:????????
 ????????  : ??
 ?? ?? ??  : ??

 ????????      :
  1.??    ??   : 2014??5??25??
    ??    ??   : h59254
    ????????   : V8R1 OM_Optimize????????
*****************************************************************************/
VOS_VOID PPM_PortCloseProc(OM_PROT_HANDLE_ENUM_UINT32  enHandle, CPM_PHY_PORT_ENUM_UINT32 enPhyPort)
{
    VOS_ULONG                           ulLockLevel;
    OM_LOGIC_CHANNEL_ENUM_UINT32        enChannel;
    VOS_BOOL                            ulSndMsg;

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseSlice = mdrv_timer_get_normal_timestamp();

    if (VOS_ERROR == g_astOMPortUDIHandle[enHandle])
    {
        return;
    }

    mdrv_udi_close(g_astOMPortUDIHandle[enHandle]);

    g_astOMPortUDIHandle[enHandle] = VOS_ERROR;

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkSlice = mdrv_timer_get_normal_timestamp();

    VOS_SpinLockIntLock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

    ulSndMsg  = VOS_FALSE;
    enChannel = OM_LOGIC_CHANNEL_BUTT;

    /* CFG????????GU??TL????????????????????GU??TL????????????????CPM?????? */
    if (OM_USB_CFG_PORT_HANDLE == enHandle)
    {
        if (enPhyPort == CPM_QueryPhyPort(CPM_OM_CFG_COMM))
        {
            ulSndMsg  = VOS_TRUE;
            enChannel = OM_LOGIC_CHANNEL_CNF;
        }
    }
    /* IND??????????????????GU??TL????????????????CPM?????? */
    else if (OM_USB_IND_PORT_HANDLE == enHandle)
    {
        if (enPhyPort == CPM_QueryPhyPort(CPM_OM_IND_COMM))
        {
            ulSndMsg  = VOS_TRUE;
            enChannel = OM_LOGIC_CHANNEL_IND;
        }
    }
    else
    {

    }

    VOS_SpinUnlockIntUnlock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

    if (VOS_TRUE == ulSndMsg)
    {
        PPM_DisconnectAllPort(enChannel);
    }

    return;
}

/*****************************************************************************
 ?? ?? ??  : GU_OamUSBReadData
 ????????  : ????ACPU??????????????????ICC??????OM????
 ????????  : enPhyPort: ????????
             UdiHandle:????????
             enHandle: UDI????????????????
 ????????  : ??
 ?? ?? ??  : VOS_ERR/VOS_OK

 ????????      :
  1.??    ??   : 2014??5??25??
    ??    ??   : h59254
    ????????   : V8R1 OM_Optimize????????
*****************************************************************************/
VOS_INT32 PPM_ReadPortData(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, UDI_HANDLE UdiHandle, OM_PROT_HANDLE_ENUM_UINT32 enHandle)
{
    ACM_WR_ASYNC_INFO                   stInfo;

    diag_PTR(EN_DIAG_PTR_PPM_READDATA);

    if (VOS_ERROR == UdiHandle)
    {
        /*lint -e534*/
        LogPrint("\r\n PPM_PortCloseProc: Input HANDLE  is err. \n");
        /*lint +e534*/

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleErr++;

        diag_PTR(EN_DIAG_PTR_PPM_ERR1);

        return VOS_ERR;
    }

    /*lint -e534*/
    VOS_MemSet(&stInfo, 0, sizeof(stInfo));
    /*lint +e534*/

    /* ????USB??IO CTRL?????????? */
    if (VOS_OK != mdrv_udi_ioctl(UdiHandle, UDI_ACM_IOCTL_GET_READ_BUFFER_CB, &stInfo))
    {
        /*lint -e534*/
        LogPrint("\r\n PPM_ReadPortData:Call mdrv_udi_ioctl is Failed\n");
        /*lint +e534*/

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleReadGetBufferErr++;
        diag_PTR(EN_DIAG_PTR_PPM_ERR2);

        return VOS_ERR;
    }

    /*lint -e40*/
    OM_ACPU_DEBUG_CHANNEL_TRACE(enPhyPort, (VOS_UINT8*)stInfo.pVirAddr, stInfo.u32Size, OM_ACPU_USB_CB);
    /*lint +e40*/

    if(VOS_OK != CPM_ComRcv(enPhyPort, (VOS_UINT8*)stInfo.pVirAddr, stInfo.u32Size))
    {
        /*lint -e534*/
        LogPrint1("\r\n Info: PPM_ReadPortData:Call CPM_ComRcv is NULL, PhyPort is %d\n", (VOS_INT)enPhyPort);
        /*lint +e534*/

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiCommRcvNullPtrErr++;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBRcvPktNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBRcvPktByte += stInfo.u32Size;

    if(VOS_OK != mdrv_udi_ioctl(UdiHandle, UDI_ACM_IOCTL_RETUR_BUFFER_CB, &stInfo))
    {
        /*lint -e534*/
        LogPrint("\r\n PPM_ReadPortData:Call mdrv_udi_ioctl UDI_ACM_IOCTL_RETUR_BUFFER_CB is Failed\n");
        /*lint +e534*/

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleReadBufferFreeErr++;
    }

    return VOS_OK;
}


/*****************************************************************************
 ?? ?? ??  : PPM_PortPseudoSyncGetSmp
 ????????  : ??????????????????????????????????????????????????
 ????????  : enHandle:UDI????????
 ????????  : VOS_VOID
 ?? ?? ??  : VOS_VOID
 ????????  :
 ????????  :

 ????????      :
  1.??    ??   : 2014??5??25??
    ??    ??   : L00256032
    ????????   : V8R1 OM_Optimize????????
*****************************************************************************/
VOS_VOID PPM_PortPseudoSyncGetSmp(OM_PROT_HANDLE_ENUM_UINT32 enHandle)
{

    return;
}

/*****************************************************************************
 ?? ?? ??  : PPM_QueryLogPort
 ????????  : ??????NAS????Log????????
 ????????  : ??
 ????????  : pulLogPort????Log????????

 ?? ?? ??  : VOS_ERR/VOS_OK

 ????????  :
   1.??    ??  : 2014??06??3??
     ??    ??  : h59254
     ????????  : Creat Function
*****************************************************************************/
VOS_UINT32 PPM_QueryLogPort(VOS_UINT32  *pulLogPort)
{
    if (VOS_NULL_PTR == pulLogPort)
    {
        /*lint -e534*/
        vos_printf("\r\n PPM_QueryLogPort: para is NULL !!\n");
        /*lint +e534*/
        return VOS_ERR;
    }

    *pulLogPort = g_stPortCfg.enPortNum;

    if ((CPM_OM_PORT_TYPE_USB != *pulLogPort) && (CPM_OM_PORT_TYPE_VCOM != *pulLogPort))
    {
        /*lint -e534*/
        vos_printf("\r\n PPM_QueryLogPort: log prot is not support. port:%d!!\n", *pulLogPort);
        /*lint +e534*/
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 ?? ?? ??  : PPM_LogPortSwitch
 ????????  : ??????NAS????????????
 ????????  : enPhyPort: ????????????????????
             ulEffect:????????????
 ????????  : ??
 ?? ?? ??  : VOS_ERR/VOS_OK

 ????????  :
   1.??    ??  : 2014??6??3??
     ??    ??  : h59254
     ????????  : Creat Function
*****************************************************************************/
VOS_UINT32 PPM_LogPortSwitch(VOS_UINT32  ulPhyPort, VOS_BOOL ulEffect)
{
    CPM_PHY_PORT_ENUM_UINT32            enPhyCfgPort;
    CPM_PHY_PORT_ENUM_UINT32            enPhyIndPort;
    VOS_ULONG                           ulLockLevel;
    VOS_BOOL                            ulSndMsg;

    if ((CPM_OM_PORT_TYPE_USB != ulPhyPort) && (CPM_OM_PORT_TYPE_VCOM != ulPhyPort))
    {
        /*lint -e534*/
        vos_printf("\r\n PPM_LogPortSwitch: enPhyPort is error.port %d\n", ulPhyPort);
        /*lint +e534*/

        g_stPpmPortSwitchInfo.ulPortTypeErr++;

        return VOS_ERR;
    }

    /* ?????????????????????????????? */
    if (ulPhyPort == g_stPortCfg.enPortNum)
    {
        /* ??????VCOM????????LOG??????????????????????????????SOCP?????????? */
        if (CPM_OM_PORT_TYPE_VCOM == g_stPortCfg.enPortNum)
        {
            mdrv_socp_set_log_int_timeout();
        }

        /* ????????USB????????????????????????????????,??????USB??????????????????SOCP?????????????????? */
        if (CPM_OM_PORT_TYPE_USB == g_stPortCfg.enPortNum)
        {
            mdrv_socp_set_log_int_default_timeout();
        }
        return VOS_OK;
    }

    g_stPpmPortSwitchInfo.ulStartSlice = mdrv_timer_get_normal_timestamp();

    enPhyCfgPort = CPM_QueryPhyPort(CPM_OM_CFG_COMM);
    enPhyIndPort = CPM_QueryPhyPort(CPM_OM_IND_COMM);

    ulSndMsg = VOS_FALSE;

    VOS_SpinLockIntLock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

    /* ??????VCOM???? */
    if (CPM_OM_PORT_TYPE_VCOM == ulPhyPort)
    {
        /* ??????USB???? */
        if ((CPM_CFG_PORT == enPhyCfgPort) && (CPM_IND_PORT == enPhyIndPort))
        {
            /* ???????????? */
            ulSndMsg = VOS_TRUE;

            CPM_DisconnectPorts(CPM_CFG_PORT, CPM_OM_CFG_COMM);
            CPM_DisconnectPorts(CPM_IND_PORT, CPM_OM_IND_COMM);
        }

        /* ????OM??VCOM???? */
        CPM_ConnectPorts(CPM_VCOM_CFG_PORT, CPM_OM_CFG_COMM);
        CPM_ConnectPorts(CPM_VCOM_IND_PORT, CPM_OM_IND_COMM);

        g_stPortCfg.enPortNum = CPM_OM_PORT_TYPE_VCOM;
    }
    /* ??????USB???? */
    else
    {
        /* ??????VCOM???? */
        if ((CPM_VCOM_CFG_PORT == enPhyCfgPort) && (CPM_VCOM_IND_PORT == enPhyIndPort))
        {
            /* ???????? */
            ulSndMsg = VOS_TRUE;

            CPM_DisconnectPorts(CPM_VCOM_CFG_PORT, CPM_OM_CFG_COMM);
            CPM_DisconnectPorts(CPM_VCOM_IND_PORT, CPM_OM_IND_COMM);
        }

        /* OM??USB???? */
        CPM_ConnectPorts(CPM_CFG_PORT, CPM_OM_CFG_COMM);
        CPM_ConnectPorts(CPM_IND_PORT, CPM_OM_IND_COMM);

        g_stPortCfg.enPortNum = CPM_OM_PORT_TYPE_USB;
    }

    VOS_SpinUnlockIntUnlock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

    if (VOS_TRUE == ulSndMsg)
    {
        PPM_DisconnectAllPort(OM_LOGIC_CHANNEL_CNF);
    }
    /* ??????VCOM????????LOG??????????????????????????????SOCP?????????? */
    if (CPM_OM_PORT_TYPE_VCOM == g_stPortCfg.enPortNum)
    {
        mdrv_socp_set_log_int_timeout();
    }

    /* ????????USB????????????????????????????????,??????USB??????????????????SOCP?????????????????? */
    if (CPM_OM_PORT_TYPE_USB == g_stPortCfg.enPortNum)
    {
        mdrv_socp_set_log_int_default_timeout();
    }
    g_stPpmPortSwitchInfo.ulSwitchSucc++;
    g_stPpmPortSwitchInfo.ulEndSlice = mdrv_timer_get_normal_timestamp();

    if (VOS_TRUE == ulEffect)
    {
        if (VOS_OK != NV_Write(en_NV_Item_Om_Port_Type, &g_stPortCfg, sizeof(OM_CHANNLE_PORT_CFG_STRU)))
        {
            /*lint -e534*/
            vos_printf("\r\n PPM_LogPortSwitch: NV_Write fail\n");
            /*lint +e534*/
        }
    }

    return VOS_OK;
}

VOS_UINT32 PPM_LogCompressCfg(VOS_BOOL enable)
{
    if(g_bPpmLogCompress != enable)
    {
        g_bPpmLogCompress = enable;
        if (CPM_OM_PORT_TYPE_VCOM == g_stPortCfg.enPortNum)
        {
            SCM_CoderDstCompressCfg(enable);
        }
        /*??????????????????????????USB????????????????*/
        if (CPM_OM_PORT_TYPE_USB == g_stPortCfg.enPortNum)
        {
            SCM_CoderDstCompressCfg(enable);
        }
    }

    return VOS_OK;
}


/*****************************************************************************
 ?? ?? ??  : PPM_UdiRegCallBackFun
 ????????  : ????????UDI??????????????
 ????????  : enHandle: ??????????
             ulCmdType: ????????
             pFunc: ??????????????????
 ????????  : ??
 ?? ?? ??  : VOS_OK/VOS_ERR
 ????????      :
  1.??    ??   : 2014??5??31??
    ??    ??   : h59254
    ????????   : V8R1 OM_Optimize????????
*****************************************************************************/
VOS_UINT32 PPM_UdiRegCallBackFun(UDI_HANDLE enHandle, VOS_UINT32 ulCmdType, VOS_VOID* pFunc)
{
    if (VOS_NULL_PTR == pFunc)
    {
        return VOS_OK;
    }

    if (VOS_OK != mdrv_udi_ioctl(enHandle, ulCmdType, pFunc))
    {
        /*lint -e534*/
        LogPrint("\r\n PPM_UdiRegCallBackFun, mdrv_udi_ioctl Failed\r\n");
        /*lint +e534*/

        return VOS_ERR;
    }

    return VOS_OK;
}



#define OM_SOCP_CNF_BUFFER_SIZE          (8*1024)
#define OM_SOCP_CNF_BUFFER_NUM           (8)

#define OM_SOCP_IND_BUFFER_SIZE          (2*1024)
#define OM_SOCP_IND_BUFFER_NUM           (2)


/*****************************************************************************
 ?? ?? ??  : PPM_ReadPortDataInit
 ????????  : ??????????OM??????????
 ????????  : enPhyPort: ??????????
             enHandle: ??????????
             pReadCB: ????????????????????????
             pWriteCB: ??????????????????????????
             pStateCB: ????????????????????????
 ????????  : ??
 ?? ?? ??  : VOS_OK/VOS_ERR
 ????????      :
  1.??    ??   : 2014??5??31??
    ??    ??   : h59254
    ????????   : V8R1 OM_Optimize????????
*****************************************************************************/
VOS_VOID PPM_ReadPortDataInit(CPM_PHY_PORT_ENUM_UINT32        enPhyPort,
                                    OM_PROT_HANDLE_ENUM_UINT32          enHandle,
                                    VOS_VOID                            *pReadCB,
                                    VOS_VOID                            *pWriteCB,
                                    VOS_VOID                            *pStateCB)
{
    UDI_OPEN_PARAM_S                    stUdiPara;
    ACM_READ_BUFF_INFO                  stReadBuffInfo;
    VOS_BOOL                            isCnf = VOS_TRUE;

    /*????????????????USB????*/
    /* Add by h59254 for V8R1 OM begin */
    if (CPM_IND_PORT == enPhyPort)
    {
        isCnf                      = VOS_FALSE;
        stUdiPara.devid            = UDI_ACM_LTE_DIAG_ID;
    }
    else if (CPM_CFG_PORT == enPhyPort)
    {
        isCnf                      = VOS_TRUE;
        stUdiPara.devid            = UDI_ACM_GPS_ID;
    }
    else if (CPM_HSIC_IND_PORT == enPhyPort)
    {
        isCnf                      = VOS_FALSE;
        stUdiPara.devid            = UDI_ACM_HSIC_ACM7_ID;
    }
    else if (CPM_HSIC_CFG_PORT == enPhyPort)
    {
        isCnf                      = VOS_TRUE;
        stUdiPara.devid            = UDI_ACM_HSIC_ACM8_ID;
    }
    /* Add by h59254 for V8R1 OM end */
    else
    {
        /*lint -e534*/
        LogPrint1("\r\n PPM_ReadPortDataInit: Open Wrong Port %d!", (VOS_INT)enPhyPort);
        /*lint +e534*/

        return;
    }

    if(isCnf == VOS_TRUE)
    {
        stReadBuffInfo.u32BuffSize = OM_SOCP_CNF_BUFFER_SIZE;
        stReadBuffInfo.u32BuffNum  = OM_SOCP_CNF_BUFFER_NUM;
    }
    else
    {
        stReadBuffInfo.u32BuffSize = OM_SOCP_IND_BUFFER_SIZE;
        stReadBuffInfo.u32BuffNum  = OM_SOCP_IND_BUFFER_NUM;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenSlice = mdrv_timer_get_normal_timestamp();

    if (VOS_ERROR != g_astOMPortUDIHandle[enHandle])
    {
        /*lint -e534*/
        LogPrint("\r\n PPM_ReadPortDataInit: The UDI Handle is not Null !");
        /*lint +e534*/

        return;
    }

    /* ????OM??????USB???? */
    g_astOMPortUDIHandle[enHandle] = mdrv_udi_open(&stUdiPara);

    if (VOS_ERROR == g_astOMPortUDIHandle[enHandle])
    {
        /*lint -e534*/
        LogPrint("\r\n PPM_ReadPortDataInit: Open UDI ACM failed!");
        /*lint +e534*/

        return;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkSlice = mdrv_timer_get_normal_timestamp();

    /* ????OM??????USB???????? */
    if (VOS_OK != mdrv_udi_ioctl(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_RELLOC_READ_BUFF, &stReadBuffInfo))
    {
        /*lint -e534*/
        LogPrint("\r\n PPM_ReadPortDataInit, mdrv_udi_ioctl ACM_IOCTL_RELLOC_READ_BUFF Failed\r\n");
        /*lint +e534*/

        return;
    }

    /* ????OM??????USB?????????????? */
    if (VOS_OK != PPM_UdiRegCallBackFun(g_astOMPortUDIHandle[enHandle], UDI_ACM_IOCTL_SET_READ_CB, pReadCB))
    {
        /*lint -e534*/
        LogPrint("\r\n PPM_ReadPortDataInit, mdrv_udi_ioctl UDI_ACM_IOCTL_SET_READ_CB Failed\r\n");
        /*lint +e534*/

        return;
    }

    if(VOS_OK != PPM_UdiRegCallBackFun(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_SET_WRITE_CB, pWriteCB))
    {
        /*lint -e534*/
        LogPrint("\r\n PPM_ReadPortDataInit, mdrv_udi_ioctl ACM_IOCTL_SET_WRITE_CB Failed\r\n");
        /*lint +e534*/

        return;
    }

    if(VOS_OK != PPM_UdiRegCallBackFun(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_SET_EVT_CB, pStateCB))
    {
        /*lint -e534*/
        LogPrint("\r\n PPM_ReadPortDataInit, mdrv_udi_ioctl ACM_IOCTL_SET_WRITE_CB Failed\r\n");
        /*lint +e534*/

        return;
    }

    if (VOS_OK != mdrv_udi_ioctl(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_WRITE_DO_COPY, VOS_NULL_PTR))
    {
        /*lint -e534*/
        LogPrint("\r\n PPM_ReadPortDataInit, mdrv_udi_ioctl ACM_IOCTL_WRITE_DO_COPY Failed\r\n");
        /*lint +e534*/

        return;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Num++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Slice = mdrv_timer_get_normal_timestamp();

    return;
}

/*****************************************************************************
 ?? ?? ??  : PPM_PortSend
 ????????  : ??????????????????????????PC??
 ????????  :  enHandle: ??????????????????????
              pucVirAddr:   ??????????
              pucPhyAddr:   ??????????
              ulDataLen: ????????
 ????????  : ??
 ?? ?? ??  : VOS_ERROR/VOS_OK
 ????????      :
  1.??    ??   : 2014??5??31??
    ??    ??   : h59254
    ????????   : V8R1 OM_Optimize????????
*****************************************************************************/
VOS_UINT32 PPM_PortSend(OM_PROT_HANDLE_ENUM_UINT32 enHandle, VOS_UINT8 *pucVirAddr, VOS_UINT8 *pucPhyAddr, VOS_UINT32 ulDataLen)
{
    VOS_INT32           lRet;
    ACM_WR_ASYNC_INFO   stVcom;
    VOS_UINT32          ulInSlice;
    VOS_UINT32          ulOutSlice;
    VOS_UINT32          ulWriteSlice;
//    VOS_CHAR            aucUsbLog[100];

    if((OM_USB_CFG_PORT_HANDLE == enHandle)
        || (OM_HSIC_CFG_PORT_HANDLE == enHandle))
    {
        diag_PTR(EN_DIAG_PTR_PPM_PORTSEND);
    }

    if ((VOS_NULL_PTR == pucVirAddr) || (VOS_NULL_PTR == pucPhyAddr))
    {
        /* ???????? */
        /*lint -e534*/
        LogPrint("\r\nPPM_PortSend: Vir or Phy Addr is Null \n");
        /*lint +e534*/

        return CPM_SEND_PARA_ERR;
    }

    stVcom.pVirAddr = (VOS_CHAR*)pucVirAddr;
    stVcom.pPhyAddr = (VOS_CHAR*)pucPhyAddr;
    stVcom.u32Size  = ulDataLen;
    stVcom.pDrvPriv = VOS_NULL_PTR;

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum1++;

    if (VOS_ERROR == g_astOMPortUDIHandle[enHandle])
    {
        /*lint -e534*/
        LogPrint("\r\nPPM_PortSend: USB HANDLE  is err. \n");
        /*lint +e534*/

        return CPM_SEND_ERR;
    }

    ulInSlice = mdrv_timer_get_normal_timestamp();

    if(OM_USB_IND_PORT_HANDLE == enHandle)
    {
        g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendLen += ulDataLen;
    }

    /* ?????????????????????????????? */
    lRet = mdrv_udi_ioctl(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_WRITE_ASYNC, &stVcom);

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum2++;

    ulOutSlice = mdrv_timer_get_normal_timestamp();

    if(ulInSlice > ulOutSlice)
    {
        ulWriteSlice = (0xffffffff - ulInSlice) + ulOutSlice;
    }
    else
    {
        ulWriteSlice = ulOutSlice - ulInSlice;
    }

    if(ulWriteSlice > g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteMaxTime)
    {
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteMaxTime = ulWriteSlice;
    }

    if (MDRV_OK == lRet)     /*????????????*/
    {
        /* ?????????????????????? */
        PPM_PortPseudoSyncGetSmp(enHandle);

        return CPM_SEND_AYNC;

    }
    else if(MDRV_OK > lRet)    /*????????*/
    {
        /*??????????????UDI??????????????*/
        /*lint -e534*/
        LogPrint1("\r\nPPM_PortSend: mdrv_udi_ioctl Send Data return Error %d\n", lRet);
        /*lint +e534*/

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrNum++;
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrLen    += ulDataLen;
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrValue  = (VOS_UINT32)lRet;
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrTime   = mdrv_timer_get_normal_timestamp();


        g_ulUSBSendErrCnt++;

        return CPM_SEND_FUNC_NULL; /*??????????????????????NULL????????*/
    }
    else    /*????????????????????*/
    {
        /*??????????????UDI????*/
        /*lint -e534*/
        LogPrint1("\r\nPPM_PortSend: mdrv_udi_ioctl Send Data return Error %d\n", lRet);
        /*lint +e534*/

        VOS_ProtectionReboot(OAM_USB_SEND_ERROR, lRet, (VOS_INT)enHandle,
                             (VOS_CHAR *)&g_stAcpuDebugInfo, (VOS_INT)sizeof(OM_ACPU_DEBUG_INFO));

        return CPM_SEND_ERR;
    }
}

/*****************************************************************************
 ?? ?? ??  :GU_OamPortWriteAsyCB
 ????????  :OM????????????????
 ????????  :enHandle:UDI????
            pucData:??????????????????
            lLen:????????
 ????????  :??
 ?? ?? ??  :??
 ????????      :
  1.??    ??   : 2014??5??31??
    ??    ??   : h59254
    ????????   : V8R1 OM_Optimize????????
*****************************************************************************/
VOS_VOID PPM_PortWriteAsyCB(OM_PROT_HANDLE_ENUM_UINT32 enHandle, VOS_UINT8* pucData, VOS_INT lLen)
{
    VOS_UINT32      ulRlsLen;

    if(lLen < 0)
    {
        ulRlsLen = 0;
    }
    else
    {
        ulRlsLen = (VOS_UINT32)lLen;
    }

    /* ???????????????????? */
    if(OM_USB_IND_PORT_HANDLE == enHandle)
    {
        diag_ThroughputSave(EN_DIAG_THRPUT_DATA_CHN_CB, lLen);
    }

    /* ?????????????????????? */
    if (OM_USB_IND_PORT_HANDLE == enHandle)
    {
        g_stUsbIndPseudoSync.ulLen          = ulRlsLen;
        g_stUsbIndPseudoSync.pucAsyncCBData = pucData;

        SCM_RlsDestBuf(SOCP_CODER_DST_OM_IND, ulRlsLen);
    }
    else if (OM_USB_CFG_PORT_HANDLE == enHandle)
    {
        g_stUsbCfgPseudoSync.ulLen          = ulRlsLen;
        g_stUsbCfgPseudoSync.pucAsyncCBData = pucData;

        SCM_RlsDestBuf(SOCP_CODER_DST_OM_CNF, ulRlsLen);
    }
    else
    {
        ;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteCBNum++;

    return;
}


OM_PSEUDO_SYNC_STRU * PPM_ComPpmGetSyncInfo(VOS_VOID)
{
    return &g_stUsbIndPseudoSync;
}

OM_ACPU_DEBUG_INFO * PPM_ComPpmGetDebugInfo(VOS_VOID)
{
    return &g_stAcpuDebugInfo;
}


/*****************************************************************************
 ?? ?? ??  : PPM_InitPhyPort
 ????????  : ??????????????
 ????????  : VOS_VOID
 ????????  : ??
 ?? ?? ??  : VOS_OK:????????????????
 ????????:
     1. ????:2012-01-21
         ??????:s00207770
         ????????:????SOCKET??????????
     2. ????:2013-12-29
         ??????:s00207770
         ????????:????LOG??????????????
*****************************************************************************/
VOS_UINT32 PPM_InitPhyPort(VOS_VOID)
{
    if (VOS_OK != PPM_PortInit())
    {
        vos_printf("PPM_InitPhyPort: PPM_PortInit failed.\n");
        return VOS_ERR;
    }


    return VOS_OK;
}

/*****************************************************************************
 ?? ?? ??  : PPM_PortInit
 ????????  : ????????????????
 ????????  : ??
 ????????  : ??
 ?? ?? ??  : VOS_OK   - ??????????
             VOS_ERR  - ??????????

 ????????      :
  1.??    ??   : 2014??5??31??
    ??    ??   : h59254
    ????????   : V8R1 OM_Optimize????????
*****************************************************************************/
VOS_UINT32 PPM_PortInit(VOS_VOID)
{
    /*lint -e534*/
    VOS_MemSet(&g_stAcpuDebugInfo, 0, sizeof(g_stAcpuDebugInfo));

    VOS_MemSet(g_astOMPortUDIHandle, VOS_ERROR, sizeof(g_astOMPortUDIHandle));

    VOS_MemSet(&g_stPpmPortSwitchInfo, 0, sizeof(g_stPpmPortSwitchInfo));
    /*lint +e534*/

    VOS_SpinLockInit(&g_stPpmPortSwitchSpinLock);

    /* USB?????????????????????????? */
    PPM_UsbPortInit();

    /* Hsic?????????????????????????? */
    PPM_HsicPortInit();

    /* Vcom?????????????????????????? */
    PPM_VComPortInit();

    return VOS_OK;
}


/*****************************************************************************
 ?? ?? ??  : OmOpenLog
 ????????  : ????????OM??????????
 ????????  :
 ????????  :
 ?? ?? ??  :
 ????????  :
 ????????  :
 ????????  :
   1.??    ??  : 2009??5??18??
     ??    ??  : g47350
     ????????  : Creat Function
*****************************************************************************/
VOS_VOID OmOpenLog(VOS_UINT32 ulFlag)
{
    g_ulOmAcpuDbgFlag = ulFlag;

    return;
}


/*****************************************************************************
 ?? ?? ??  : PPM_OmPortInfoShow
 ????????  : ??????????????????????????
 ????????  : ??
 ????????  : ??
 ?? ?? ??  : ??
 ????????  :
 ????????  :
 ????????  :
   1.??    ??  : 2011??3??10??
     ??    ??  : l46160
     ????????  : Creat Function
*****************************************************************************/
VOS_VOID PPM_OmPortInfoShow(OM_PROT_HANDLE_ENUM_UINT32  enHandle)
{
    /*lint -e534*/
    vos_printf("\r\nInvalidchanel num is             %d",   g_stAcpuDebugInfo.ulInvaldChannel);
    
    vos_printf("\r\nThe Port Write num 1 is          %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum1);
    vos_printf("\r\nThe Port Write num 2 is          %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum2);
    vos_printf("\r\nThe Port Write Max Time is       0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteMaxTime);

    vos_printf("\r\nThe Port Write CB Num is         %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteCBNum);

    vos_printf("\r\nThe Port Write Err Time is       %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrTime);
    vos_printf("\r\nThe Port Write Err Num  is       %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrNum);
    vos_printf("\r\nThe Port Write Err Value is      0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrValue);
    vos_printf("\r\nThe Port Write Err Len is        0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrLen);

    vos_printf("\r\nThe Port In CB Num is            %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINNum);
    vos_printf("\r\nThe Port In CB Time is           0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINTime);
    vos_printf("\r\nThe Port Out CB Num is           %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutNum);
    vos_printf("\r\nThe Port Out CB Time is          0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutTime);
    vos_printf("\r\nThe Port State CB Err Num is     %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrNum);
    vos_printf("\r\nThe Port State CB Err Time is    0x%x", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrTime);

    vos_printf("\r\nThe Port Open num is            %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenNum);
    vos_printf("\r\nThe Port Open slice is          0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenSlice);

    vos_printf("\r\nThe Port Open OK num is         %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkNum);
    vos_printf("\r\nThe Port Open OK slice is       0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkSlice);

    vos_printf("\r\nThe Port Open OK2 num is        %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Num);
    vos_printf("\r\nThe Port Open OK2 slice is      0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Slice);

    vos_printf("\r\nThe Port Close num is           %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseNum);
    vos_printf("\r\nThe Port Close slice is         0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseSlice);

    vos_printf("\r\nThe Port Close OK num is        %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkNum);
    vos_printf("\r\nThe Port Close OK slice is      0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkSlice);

    vos_printf("\r\nUSB IND Pseudo sync fail num is   %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBIndPseudoSyncFailNum);
    vos_printf("\r\nUSB IND Pseudo sync fail slice is 0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBIndPseudoSyncFailSlice);
    vos_printf("\r\nUSB CFG Pseudo sync fail num is   %d",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCnfPseudoSyncFailNum);
    vos_printf("\r\nUSB CFG Pseudo sync fail slice is 0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCnfPseudoSyncFailSlice);

    vos_printf("\r\nThe Port UDI Handle Err num is %d",                 g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleErr);
    vos_printf("\r\nThe Port UDI Handle Get Buffer Err num is %d",      g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleReadGetBufferErr);
    vos_printf("\r\nThe Port UDI Handle Comm Rcv Null Ptr num is %d",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiCommRcvNullPtrErr);
    vos_printf("\r\nThe Port UDI Handle Read Buffer Free Err num is %d",g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleReadBufferFreeErr);

    vos_printf("\r\nThe Port UDI Handle Total Rcv Pkt num is %d",       g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBRcvPktNum);
    vos_printf("\r\nThe Port UDI Handle Total Rcv Byte is %d",          g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBRcvPktByte);
    /*lint +e534*/

    return;
}


/*****************************************************************************
 ?? ?? ??  : PPM_OmPortDebugInfoShow
 ????????  : ????????CNF\IND??????????????
 ????????  : ??
 ????????  : ??
 ?? ?? ??  : ??
 ????????  :
 ????????  :
 ????????  :
   1.??    ??  : 2015??11??2??
     ??    ??  : c00326366
     ????????  : Creat Function
*****************************************************************************/
VOS_VOID PPM_OmPortDebugInfoShow(VOS_VOID)
{
    vos_printf("IND ulOmDiscardNum %d, len %d; ulUSBSendErrNum %d, Len %d; ulUSBSendNum %d, len %d.\r\n",
    g_stAcpuDebugInfo.stIndDebugInfo.ulOmDiscardNum, g_stAcpuDebugInfo.stIndDebugInfo.ulOmDiscardLen,
    g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendErrNum, g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendErrLen,
    g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendNum, g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendLen);

    vos_printf("CNF ulOmDiscardNum %d, len %d; ulUSBSendErrNum %d, Len %d; ulUSBSendNum %d, len %d.\r\n",
    g_stAcpuDebugInfo.stCnfDebugInfo.ulOmDiscardNum, g_stAcpuDebugInfo.stCnfDebugInfo.ulOmDiscardLen,
    g_stAcpuDebugInfo.stCnfDebugInfo.ulUSBSendErrNum, g_stAcpuDebugInfo.stCnfDebugInfo.ulUSBSendErrLen,
    g_stAcpuDebugInfo.stCnfDebugInfo.ulUSBSendNum, g_stAcpuDebugInfo.stCnfDebugInfo.ulUSBSendLen);
}

/*****************************************************************************
 ?? ?? ??  : PPM_PortSwitchInfoShow
 ????????  : ??????????????????????????
 ????????  : ??
 ????????  : ??
 ?? ?? ??  : ??
 ????????  :
 ????????  :
 ????????  :
   1.??    ??  : 2014??6??10??
     ??    ??  : h59254
     ????????  : Creat Function
*****************************************************************************/
VOS_VOID PPM_PortSwitchInfoShow(VOS_VOID)
{
    /*lint -e534*/
    vos_printf("\r\n Port Type Err num is %d", g_stPpmPortSwitchInfo.ulPortTypeErr);

    vos_printf("\r\n Port Switch Fail time is %d", g_stPpmPortSwitchInfo.ulSwitchFail);

    vos_printf("\r\n Port Switch Success time is %d", g_stPpmPortSwitchInfo.ulSwitchSucc);

    vos_printf("\r\n Port Switch Start slice is 0x%x", g_stPpmPortSwitchInfo.ulStartSlice);

    vos_printf("\r\n Port Switch End slice is 0x%x", g_stPpmPortSwitchInfo.ulEndSlice);
    /*lint +e534*/

    return;
}








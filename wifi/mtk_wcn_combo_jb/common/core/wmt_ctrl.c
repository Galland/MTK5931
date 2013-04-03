/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */


/*! \file
    \brief  Declaration of library functions

    Any definitions in this file will be shared among GLUE Layer and internal Driver Stack.
*/

/*******************************************************************************
* Copyright (c) 2009 MediaTek Inc.
*
* All rights reserved. Copying, compilation, modification, distribution
* or any other use whatsoever of this material is strictly prohibited
* except in accordance with a Software License Agreement with
* MediaTek Inc.
********************************************************************************
*/

/*******************************************************************************
* LEGAL DISCLAIMER
*
* BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND
* AGREES THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK
* SOFTWARE") RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE
* PROVIDED TO BUYER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY
* DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT
* LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE
* ANY WARRANTY WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY
* WHICH MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK
* SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY
* WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE
* FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION OR TO
* CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
* BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
* LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL
* BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT
* ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
* BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
* WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT
* OF LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING
* THEREOF AND RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN
* FRANCISCO, CA, UNDER THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE
* (ICC).
********************************************************************************
*/


/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/
#ifdef DFT_TAG
#undef DFT_TAG
#endif
#define DFT_TAG         "[WMT-CTRL]"


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "osal_typedef.h"
#include "osal.h"

#include "wmt_ctrl.h"
#include "wmt_core.h"
#include "wmt_lib.h"
#include "wmt_dev.h"
#include "wmt_plat.h"
#include "hif_sdio.h"
#include "stp_core.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/



/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/


/*******************************************************************************
*                    F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
// moved to wmt_ctrl.h
/*static INT32  wmt_ctrl_tx_ex (UINT8 *pData, UINT32 size, UINT32 *writtenSize, MTK_WCN_BOOL bRawFlag);*/

static INT32  wmt_ctrl_stp_conf_ex (WMT_STP_CONF_TYPE type, UINT32 value);

static INT32  wmt_ctrl_hw_pwr_off(P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_hw_pwr_on(P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_hw_rst(P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_stp_close(P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_stp_open(P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_stp_conf(P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_free_patch(P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_get_patch(P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_host_baudrate_set(P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_sdio_hw(P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_sdio_func(P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_hwidver_set (P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_stp_rst(P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_get_wmt_conf(P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_others(P_WMT_CTRL_DATA);
static INT32  wmt_ctrl_tx(P_WMT_CTRL_DATA);
static VOID wmt_ctrl_rx_event_cb (VOID);
static INT32 wmt_ctrl_rx_flag_checker(PVOID pvData);
static INT32 wmt_ctrl_rx_timeout (P_OSAL_EVENT pEvent);
static INT32 wmt_ctrl_rx(P_WMT_CTRL_DATA);
static INT32
wmt_ctrl_rx_flush (
    P_WMT_CTRL_DATA
    );

static INT32
wmt_ctrl_gps_sync_set (
    P_WMT_CTRL_DATA pData
    );

static INT32
wmt_ctrl_gps_lna_set (
    P_WMT_CTRL_DATA pData
    );


static INT32  wmt_ctrl_get_patch_name(P_WMT_CTRL_DATA pWmtCtrlData);


/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

// TODO:[FixMe][GeorgeKuo]: use module APIs instead of direct access to internal data
extern DEV_WMT gDevWmt;


/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/* GeorgeKuo: Use designated initializers described in
 * http://gcc.gnu.org/onlinedocs/gcc-4.0.4/gcc/Designated-Inits.html
 */
const static WMT_CTRL_FUNC wmt_ctrl_func[] =
{
    [WMT_CTRL_HW_PWR_OFF] = wmt_ctrl_hw_pwr_off,
    [WMT_CTRL_HW_PWR_ON] = wmt_ctrl_hw_pwr_on,
    [WMT_CTRL_HW_RST] = wmt_ctrl_hw_rst,
    [WMT_CTRL_STP_CLOSE] = wmt_ctrl_stp_close,
    [WMT_CTRL_STP_OPEN] = wmt_ctrl_stp_open,
    [WMT_CTRL_STP_CONF] = wmt_ctrl_stp_conf,
    [WMT_CTRL_FREE_PATCH] = wmt_ctrl_free_patch,
    [WMT_CTRL_GET_PATCH] = wmt_ctrl_get_patch,
    [WMT_CTRL_GET_PATCH_NAME] = wmt_ctrl_get_patch_name,
    [WMT_CTRL_HOST_BAUDRATE_SET] = wmt_ctrl_host_baudrate_set,
    [WMT_CTRL_SDIO_HW] = wmt_ctrl_sdio_hw,
    [WMT_CTRL_SDIO_FUNC] = wmt_ctrl_sdio_func,
    [WMT_CTRL_HWIDVER_SET] = wmt_ctrl_hwidver_set,
    [WMT_CTRL_HWVER_GET] = NULL, // TODO: [FixMe][GeorgeKuo]: remove unused function.
    [WMT_CTRL_STP_RST] = wmt_ctrl_stp_rst,
    [WMT_CTRL_GET_WMT_CONF] = wmt_ctrl_get_wmt_conf,
    [WMT_CTRL_TX] = wmt_ctrl_tx,
    [WMT_CTRL_RX] = wmt_ctrl_rx,
    [WMT_CTRL_RX_FLUSH] = wmt_ctrl_rx_flush,
    [WMT_CTRL_GPS_SYNC_SET] = wmt_ctrl_gps_sync_set,
    [WMT_CTRL_GPS_LNA_SET] = wmt_ctrl_gps_lna_set,
    [WMT_CTRL_MAX] = wmt_ctrl_others,
};

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

INT32  wmt_ctrl (P_WMT_CTRL_DATA pWmtCtrlData)
{
    UINT32 ctrlId = pWmtCtrlData->ctrlId;

    if (NULL == pWmtCtrlData) {
        osal_assert(0);
        return -1;
    }

    /*1sanity check, including wmtCtrlId*/
    if ( (NULL == pWmtCtrlData)
        || (WMT_CTRL_MAX <= ctrlId) )
        /* || (ctrlId < WMT_CTRL_HW_PWR_OFF) ) [FixMe][GeorgeKuo]: useless comparison */
    {
        osal_assert(NULL != pWmtCtrlData);
        osal_assert(WMT_CTRL_MAX > ctrlId);
        /* osal_assert(ctrlId >= WMT_CTRL_HW_PWR_OFF); [FixMe][GeorgeKuo]: useless comparison */
        return -2;
    }

    // TODO: [FixMe][GeorgeKuo] do sanity check to const function table when init and skip checking here
    if (wmt_ctrl_func[ctrlId]) {
        /*call servicd handling API*/
        return (*(wmt_ctrl_func[ctrlId]))(pWmtCtrlData); /* serviceHandlerPack[ctrlId].serviceHandler */
    }
    else {
        osal_assert(NULL != wmt_ctrl_func[ctrlId]);
        return -3;
    }
}

INT32 wmt_ctrl_tx (P_WMT_CTRL_DATA pWmtCtrlData/*UINT8 *pData, UINT32 size, UINT32 *writtenSize*/)
{
    UINT8 *pData = (UINT8 *)pWmtCtrlData->au4CtrlData[0];
    UINT32 size = pWmtCtrlData->au4CtrlData[1];
    UINT32 *writtenSize = (UINT32 *)pWmtCtrlData->au4CtrlData[2];
    MTK_WCN_BOOL bRawFlag = pWmtCtrlData->au4CtrlData[3];

    return wmt_ctrl_tx_ex(pData, size, writtenSize, bRawFlag);
}

/* moved from wmt_dev_rx_event_cb */
static VOID wmt_ctrl_rx_event_cb (VOID)
{
    WMT_DBG_FUNC("\n");

    /* George: rWmtRxWq is initialized during initialization, wmt_lib_init() */
    gDevWmt.rWmtRxWq.waitFlag = 1;
    /* do we need smp_mb()? */
    //wake_up_interruptible(&gDevWmt.rWmtRxWq.waitQueue);
    osal_trigger_event(&gDevWmt.rWmtRxWq);
}

static INT32 wmt_ctrl_rx_flag_checker(PVOID pvData)
{
    return (*((PINT32)(pvData)) != 0);
}

/* moved from wmt_dev_rx_timeout */
static INT32 wmt_ctrl_rx_timeout (P_OSAL_EVENT pEvent)
{
    UINT32 ms;
    INT32 iRet;
    //LONG lRet;

    ms = pEvent->timeoutValue;

    WMT_DBG_FUNC("\n");
    if (0 != ms) {
        iRet = osal_wait_for_event_timeout(pEvent, wmt_ctrl_rx_flag_checker, &pEvent->waitFlag);
        //lRet = wait_event_interruptible_timeout(pEvent->waitQueue,  pEvent->waitFlag != 0, msecs_to_jiffies(ms));
    }
    else {
        iRet = osal_wait_for_event(pEvent, wmt_ctrl_rx_flag_checker, &pEvent->waitFlag);
        //lRet = wait_event_interruptible(pEvent->waitQueue,  pEvent->waitFlag != 0);
    }
    pEvent->waitFlag = 0;
    return iRet;
}

INT32 wmt_ctrl_rx(P_WMT_CTRL_DATA pWmtCtrlData/*UINT8 *pBuff, UINT32 buffLen, UINT32 *readSize*/)
{
    P_DEV_WMT pDev = &gDevWmt; /* single instance */
    INT32 readLen;
    LONG waitRet = -1;
    UINT8 *pBuff = (UINT8 *)pWmtCtrlData->au4CtrlData[0];
    UINT32 buffLen = pWmtCtrlData->au4CtrlData[1];
    UINT32 *readSize = (UINT32 *)pWmtCtrlData->au4CtrlData[2];

    if (readSize) {
        *readSize = 0;
    }

    /* sanity check */
    if (!buffLen ) {
        WMT_WARN_FUNC("buffLen = 0\n");
        osal_assert(buffLen);
        return 0;
    }

#if 0
    if (!pDev) {
        WMT_WARN_FUNC("gpDevWmt = NULL\n");
        osal_assert(pDev);
        return -1;
    }
#endif

    if (!osal_test_bit(WMT_STAT_STP_OPEN, &pDev->state)) {
        WMT_WARN_FUNC("state(0x%lx) \n", pDev->state);
        osal_assert(osal_test_bit(WMT_STAT_STP_OPEN, &pDev->state));
        return -2;
    }

    /* sanity ok, proceeding rx operation */
    /* read_len = mtk_wcn_stp_receive_data(data, size, WMT_TASK_INDX); */
    readLen = mtk_wcn_stp_receive_data(pBuff, buffLen, WMT_TASK_INDX);

    while (readLen == 0) { // got nothing, wait for STP's signal
        //WMT_LOUD_FUNC("before wmt_ctrl_rx_timeout\n");
        pDev->rWmtRxWq.timeoutValue = WMT_LIB_RX_TIMEOUT;
        //waitRet = wmt_dev_rx_timeout(&pDev->rWmtRxWq);
        waitRet = wmt_ctrl_rx_timeout(&pDev->rWmtRxWq);

        //WMT_LOUD_FUNC("wmt_ctrl_rx_timeout returned\n");

        if (0 == waitRet) {
            WMT_ERR_FUNC("wmt_ctrl_rx_timeout: timeout \n");
            return -1;
        }
        else if (waitRet < 0) {
            WMT_WARN_FUNC("wmt_ctrl_rx_timeout: interrupted by signal (%ld)\n", waitRet);
            return waitRet;
        }
        //WMT_LOUD_FUNC("wmt_ctrl_rx_timeout, iRet(%ld)\n", waitRet);
        /* read_len = mtk_wcn_stp_receive_data(data, size, WMT_TASK_INDX); */
        readLen = mtk_wcn_stp_receive_data(pBuff, buffLen, WMT_TASK_INDX);

        if (0 == readLen) {
            WMT_WARN_FUNC("wmt_ctrl_rx be signaled, but no rx data(%ld)\n", waitRet);
        }
        WMT_DBG_FUNC("readLen(%d) \n", readLen);
    }

    if (readSize) {
        *readSize = readLen ;
    }

    return 0;

}


INT32
wmt_ctrl_tx_ex (
    const UINT8 *pData,
    const UINT32 size,
    UINT32 *writtenSize,
    const MTK_WCN_BOOL bRawFlag
    )
{
    P_DEV_WMT pDev = &gDevWmt; /* single instance */
    INT32 iRet;

    if (NULL != writtenSize) {
        *writtenSize = 0;
    }

    /* sanity check */
    if (0 == size) {
        WMT_WARN_FUNC("size to tx is 0\n");
        osal_assert(size);
        return -1;
    }

    /* if STP is not enabled yet, can't use this function. Use tx_raw instead */
    if ( !osal_test_bit(WMT_STAT_STP_OPEN, &pDev->state) ||
        !osal_test_bit(WMT_STAT_STP_EN, &pDev->state) ) {
        WMT_ERR_FUNC("wmt state(0x%lx) \n", pDev->state);
        osal_assert(osal_test_bit(WMT_STAT_STP_EN, &pDev->state));
        osal_assert(osal_test_bit(WMT_STAT_STP_OPEN, &pDev->state));
        return -2;
    }

    /* sanity ok, proceeding tx operation */
    /*retval = mtk_wcn_stp_send_data(data, size, WMTDRV_TYPE_WMT);*/
    mtk_wcn_stp_flush_rx_queue(WMT_TASK_INDX);
    if (bRawFlag) {
        iRet = mtk_wcn_stp_send_data_raw(pData, size, WMT_TASK_INDX);
    }
    else {
        iRet = mtk_wcn_stp_send_data(pData, size, WMT_TASK_INDX);
    }

    if (iRet != size){
        WMT_WARN_FUNC("write(%d) written(%d)\n", size, iRet);
        osal_assert(iRet == size);
    }

    if (writtenSize) {
        *writtenSize = iRet;
    }

    return 0;

}

INT32
wmt_ctrl_rx_flush (
    P_WMT_CTRL_DATA pWmtCtrlData
    )
{
    UINT32 type = pWmtCtrlData->au4CtrlData[0];

    WMT_INFO_FUNC("flush rx %d queue\n", type);
    mtk_wcn_stp_flush_rx_queue(type);

    return 0;
}


INT32  wmt_ctrl_hw_pwr_off(P_WMT_CTRL_DATA pWmtCtrlData)
{
    INT32 iret;

/*psm should be disabled before wmt_ic_deinit*/
    P_DEV_WMT pDev = &gDevWmt;
    if (osal_test_and_clear_bit(WMT_STAT_PWR, &pDev->state)) {
        WMT_DBG_FUNC("on->off \n");
        iret = wmt_plat_pwr_ctrl(FUNC_OFF);
    }
    else {
        WMT_WARN_FUNC("already off \n");
        iret = 0;
    }

    return iret;
}

INT32  wmt_ctrl_hw_pwr_on(P_WMT_CTRL_DATA pWmtCtrlData)
{
    INT32 iret;

    /*psm should be enabled right after wmt_ic_init*/
    P_DEV_WMT pDev = &gDevWmt;
    if (osal_test_and_set_bit(WMT_STAT_PWR, &pDev->state)) {
        WMT_WARN_FUNC("already on\n");
        iret = 0;
    }
    else {
        WMT_DBG_FUNC("off->on \n");
        iret = wmt_plat_pwr_ctrl(FUNC_ON);
    }

    return iret;
}

INT32  wmt_ctrl_ul_cmd (
    P_DEV_WMT pWmtDev,
    const UCHAR *pCmdStr
    )
{
    INT32 waitRet = -1;
    P_OSAL_SIGNAL pCmdSignal;
    P_OSAL_EVENT pCmdReq;

    pCmdSignal = &pWmtDev->cmdResp;
    osal_signal_init(pCmdSignal);
    pCmdSignal->timeoutValue = 2000;

    if (osal_test_and_set_bit(WMT_STAT_CMD, &pWmtDev->state)) {
        WMT_WARN_FUNC("cmd buf is occupied by (%s) \n", pWmtDev->cCmd);
        /* [FIXME][George] deinit(pCmdSignal) here or not? */
	    osal_signal_deinit(pCmdSignal);
        return -1;
    }

    /* indicate baud rate change to user space app */
#if 0
    INIT_COMPLETION(pWmtDev->cmd_comp);
    pWmtDev->cmd_result = -1;
    strncpy(pWmtDev->cCmd, pCmdStr, OSAL_NAME_MAX);
    pWmtDev->cCmd[OSAL_NAME_MAX] = '\0';
    wake_up_interruptible(&pWmtDev->cmd_wq);
#endif

//    pCmdSignal = &pWmtDev->cmdResp;
//    osal_signal_init(pCmdSignal);
//    pCmdSignal->timeoutValue = 2000;

    osal_strncpy(pWmtDev->cCmd, pCmdStr, OSAL_NAME_MAX);
    pWmtDev->cCmd[OSAL_NAME_MAX] = '\0';

    pCmdReq = &pWmtDev->cmdReq;

    osal_trigger_event(&pWmtDev->cmdReq);
    WMT_DBG_FUNC("str(%s) request ok\n", pCmdStr);

//    waitRet = wait_for_completion_interruptible_timeout(&pWmtDev->cmd_comp, msecs_to_jiffies(2000));
    waitRet = osal_wait_for_signal_timeout(pCmdSignal);
    WMT_LOUD_FUNC("wait signal iRet:%d\n", waitRet);
    if (0 == waitRet) {
        WMT_ERR_FUNC("wait signal timeout \n");
        osal_signal_deinit(pCmdSignal);
        return -2;
    }

    WMT_INFO_FUNC("str(%s) result(%d)\n", pCmdStr, pWmtDev->cmdResult);
    osal_signal_deinit(pCmdSignal);

    return pWmtDev->cmdResult;
}

INT32  wmt_ctrl_hw_rst(P_WMT_CTRL_DATA pWmtCtrlData)
{
    wmt_plat_pwr_ctrl(FUNC_RST);
    return 0;
}

INT32  wmt_ctrl_stp_close(P_WMT_CTRL_DATA pWmtCtrlData)
{
    P_DEV_WMT pDev = &gDevWmt; /* single instance */
    INT32 iRet = 0;
    UCHAR cmdStr[OSAL_NAME_MAX + 1];
    /* un-register to STP-core for rx */
    iRet = mtk_wcn_stp_register_event_cb(WMT_TASK_INDX, NULL); /* mtk_wcn_stp_register_event_cb */
    if (iRet) {
        WMT_WARN_FUNC("stp_reg cb unregister fail(%d)\n", iRet);
        return -1;
    }

    if (WMT_HIF_UART == pDev->rWmtHifConf.hifType) {
        osal_memset(&cmdStr[0], 0, sizeof(cmdStr));
        osal_snprintf(cmdStr, OSAL_NAME_MAX, "close_stp");

        iRet = wmt_ctrl_ul_cmd(pDev, cmdStr);
        if (iRet) {
            WMT_WARN_FUNC("wmt_ctrl_ul_cmd fail(%d)\n", iRet);
            return -2;
        }
    }

    osal_clear_bit(WMT_STAT_STP_OPEN, &pDev->state);

    return 0;
}

INT32  wmt_ctrl_stp_open(P_WMT_CTRL_DATA pWmtCtrlData)
{
    P_DEV_WMT pDev = &gDevWmt; /* single instance */
    INT32 iRet;
    UCHAR cmdStr[OSAL_NAME_MAX + 1];

    if (WMT_HIF_UART == pDev->rWmtHifConf.hifType) {
        osal_memset(&cmdStr[0], 0, sizeof(cmdStr));
        osal_snprintf(cmdStr, OSAL_NAME_MAX, "open_stp");
        iRet = wmt_ctrl_ul_cmd(pDev, cmdStr);
        if (iRet) {
            WMT_WARN_FUNC("wmt_ctrl_ul_cmd fail(%d)\n", iRet);
            return -1;
        }
    }

    /* register to STP-core for rx */
    iRet = mtk_wcn_stp_register_event_cb(WMT_TASK_INDX, wmt_ctrl_rx_event_cb); /* mtk_wcn_stp_register_event_cb */
    if (iRet) {
        WMT_WARN_FUNC("stp_reg cb fail(%d)\n", iRet);
        return -2;
    }

    osal_set_bit(WMT_STAT_STP_OPEN, &pDev->state);

    return 0;
}


INT32  wmt_ctrl_stp_conf_ex (WMT_STP_CONF_TYPE type, UINT32 value)
{
    INT32 iRet = -1;
    switch (type) {
    case WMT_STP_CONF_EN:
        iRet = mtk_wcn_stp_enable(value);
        break;

    case WMT_STP_CONF_RDY:
        iRet = mtk_wcn_stp_ready(value);
        break;

    case WMT_STP_CONF_MODE:
        mtk_wcn_stp_set_mode(value);
        iRet = 0;
        break;

    default:
        WMT_WARN_FUNC("invalid type(%d) value(%d) \n", type, value);
        break;
    }
    return iRet;
}


INT32  wmt_ctrl_stp_conf(P_WMT_CTRL_DATA pWmtCtrlData)
{
    INT32 iRet = -1;
    P_DEV_WMT pDev = &gDevWmt; /* single instance */
    UINT32 type;
    UINT32 value;
    if (!osal_test_bit(WMT_STAT_STP_OPEN, &pDev->state)) {
        WMT_WARN_FUNC("CTRL_STP_ENABLE but invalid Handle of WmtStp \n");
        return -1;
    }

    type = pWmtCtrlData->au4CtrlData[0];
    value = pWmtCtrlData->au4CtrlData[1];
    iRet = wmt_ctrl_stp_conf_ex(type, value);

    if (!iRet) {
        if (WMT_STP_CONF_EN == type) {
            if (value) {
                osal_set_bit(WMT_STAT_STP_EN, &pDev->state);
                WMT_DBG_FUNC("enable STP\n");
            }
            else {
                osal_clear_bit(WMT_STAT_STP_EN, &pDev->state);
                WMT_DBG_FUNC("disable STP\n");
            }
        }
    }

    return iRet;
}

INT32  wmt_ctrl_free_patch(P_WMT_CTRL_DATA pWmtCtrlData)
{
    WMT_DBG_FUNC("BF free patch, gDevWmt.pPatch(0x%08x)\n", gDevWmt.pPatch);
    if (NULL != gDevWmt.pPatch)
    {
        wmt_dev_patch_put((OSAL_FIRMWARE **)(&gDevWmt.pPatch));
    }
    WMT_DBG_FUNC("AF free patch, gDevWmt.pPatch(0x%08x)\n", gDevWmt.pPatch);
    return 0;
}



INT32  wmt_ctrl_get_patch_name(P_WMT_CTRL_DATA pWmtCtrlData)
{
    PUCHAR pBuf = (PUCHAR)pWmtCtrlData->au4CtrlData[0];
    osal_memcpy(pBuf, gDevWmt.cPatchName, osal_sizeof(gDevWmt.cPatchName));
    return 0;
}


INT32  wmt_ctrl_get_patch(P_WMT_CTRL_DATA pWmtCtrlData)
{
    UCHAR *pFullPatchName = NULL;
    UCHAR *pDefPatchName = NULL;
    PUINT8 *ppBuf = (PUINT8 *)pWmtCtrlData->au4CtrlData[2];
    PUINT32 pSize = (PUINT32)pWmtCtrlData->au4CtrlData[3];

    //osal_firmware *pPatch = NULL;
    OSAL_FIRMWARE *pPatch = NULL;//zhiguo
    pFullPatchName = (UCHAR *)pWmtCtrlData->au4CtrlData[1];
    WMT_DBG_FUNC("BF get patch, pPatch(0x%08x)\n", pPatch);
    if ( (NULL != pFullPatchName)
        && (0 == wmt_dev_patch_get(pFullPatchName, &pPatch, BCNT_PATCH_BUF_HEADROOM)) ) {
        /*get full name patch success*/
        WMT_DBG_FUNC("get full patch name(%s) buf(0x%p) size(%d)\n",
            pFullPatchName, (pPatch)->data, (pPatch)->size);
        WMT_DBG_FUNC("AF get patch, pPatch(0x%08x)\n", pPatch);
        *ppBuf = (PUINT8)(pPatch)->data;
        *pSize = (pPatch)->size;
        gDevWmt.pPatch = pPatch;
        return 0;
    }

    pDefPatchName = (UCHAR *)pWmtCtrlData->au4CtrlData[0];
    if ( (NULL != pDefPatchName)
        && (0 == wmt_dev_patch_get(pDefPatchName, &pPatch, BCNT_PATCH_BUF_HEADROOM)) ) {
        WMT_DBG_FUNC("get def patch name(%s) buf(0x%p) size(%d)\n",
            pDefPatchName, (pPatch)->data, (pPatch)->size);
        WMT_DBG_FUNC("AF get patch, pPatch(0x%08x)\n", pPatch);
        /*get full name patch success*/
        *ppBuf = (PUINT8)(pPatch)->data;
        *pSize = (pPatch)->size;
        gDevWmt.pPatch = pPatch;
        return 0;
    }
    return -1;

}

INT32  wmt_ctrl_host_baudrate_set(P_WMT_CTRL_DATA pWmtCtrlData)
{
    INT32 iRet = -1;
    CHAR cmdStr[OSAL_NAME_MAX + 1];
    UINT32 u4Baudrate = pWmtCtrlData->au4CtrlData[0];
    UINT32 u4FlowCtrl = pWmtCtrlData->au4CtrlData[1];

    WMT_DBG_FUNC("baud(%d), flowctrl(%d) \n", u4Baudrate, u4FlowCtrl);

    if (osal_test_bit(WMT_STAT_STP_OPEN, &gDevWmt.state)) {
        osal_memset(&cmdStr[0], 0, sizeof(cmdStr));
        osal_snprintf(cmdStr, OSAL_NAME_MAX, "baud_%d_%d", u4Baudrate, u4FlowCtrl);
        iRet = wmt_ctrl_ul_cmd(&gDevWmt, cmdStr);
    if (iRet) {
        WMT_WARN_FUNC("CTRL_BAUDRATE baud(%d), flowctrl(%d) fail(%d) \n",
            u4Baudrate,
            pWmtCtrlData->au4CtrlData[1],
            iRet);
        }
    else {
        WMT_DBG_FUNC("CTRL_BAUDRATE baud(%d), flowctrl(%d) ok\n",
        u4Baudrate,
        u4FlowCtrl);
        }
    }
    else {
        WMT_INFO_FUNC("CTRL_BAUDRATE but invalid Handle of WmtStp \n");
    }
    return iRet;
}

INT32  wmt_ctrl_sdio_hw(P_WMT_CTRL_DATA pWmtCtrlData)
{
    INT32 iRet = 0;
    UINT32 statBit = WMT_STAT_SDIO1_ON;
    P_DEV_WMT pDev = &gDevWmt; /* single instance */

    WMT_SDIO_SLOT_NUM sdioSlotNum = pWmtCtrlData->au4CtrlData[0];
    INT32 funcState = pWmtCtrlData->au4CtrlData[1];

    if ((WMT_SDIO_SLOT_INVALID == sdioSlotNum)
        || (WMT_SDIO_SLOT_MAX <= sdioSlotNum)) {
        WMT_WARN_FUNC("CTRL_SDIO_SLOT(%d) but invalid slot num \n", sdioSlotNum);
        return -1;
    }

    WMT_DBG_FUNC("WMT_CTRL_SDIO_HW (0x%x, %d)\n", sdioSlotNum, funcState);

    if (WMT_SDIO_SLOT_SDIO2 == sdioSlotNum) {
        statBit = WMT_STAT_SDIO2_ON;
    }

    if (funcState) {
        if (osal_test_and_set_bit(statBit, &pDev->state)) {
            WMT_WARN_FUNC("CTRL_SDIO_SLOT slotNum(%d) already ON \n", sdioSlotNum);
            //still return 0
            iRet = 0;
        }
        else {
            iRet = wmt_plat_sdio_ctrl(sdioSlotNum, FUNC_ON);
        }
    }
    else  {
        if (osal_test_and_clear_bit(statBit, &pDev->state)) {
            iRet = wmt_plat_sdio_ctrl(sdioSlotNum, FUNC_OFF);
        }
        else {
            WMT_WARN_FUNC("CTRL_SDIO_SLOT slotNum(%d) already OFF \n", sdioSlotNum);
            //still return 0
            iRet = 0;
        }
    }

    return iRet;
}

INT32  wmt_ctrl_sdio_func(P_WMT_CTRL_DATA pWmtCtrlData)
{
    INT32 iRet = -1;
    UINT32 statBit = WMT_STAT_SDIO_WIFI_ON;
    INT32 retry = 10;
    P_DEV_WMT pDev = &gDevWmt; /* single instance */
    WMT_SDIO_FUNC_TYPE sdioFuncType = pWmtCtrlData->au4CtrlData[0];
    UINT32 u4On = pWmtCtrlData->au4CtrlData[1];

    if (WMT_SDIO_FUNC_MAX <= sdioFuncType) {
        WMT_ERR_FUNC("CTRL_SDIO_FUNC, invalid func type (%d) \n", sdioFuncType);
        return -1;
    }

    if (WMT_SDIO_FUNC_STP == sdioFuncType) {
        statBit = WMT_STAT_SDIO_STP_ON;
    }

    if (u4On) {
        if (osal_test_bit(statBit, &pDev->state)) {
            WMT_WARN_FUNC("CTRL_SDIO_FUNC(%d) but already ON \n", sdioFuncType);
            iRet = 0;
        }
        else {
            while (retry-- > 0 && iRet != 0) {
                if (iRet) {
                    /* sleep 150ms before sdio slot ON ready */
                    osal_msleep(150);
                }
                iRet = mtk_wcn_hif_sdio_wmt_control(sdioFuncType, MTK_WCN_BOOL_TRUE);
                if (HIF_SDIO_ERR_NOT_PROBED == iRet) {
                    /* not probed case, retry */
                    continue;
                }
                else if (HIF_SDIO_ERR_CLT_NOT_REG == iRet){
                    /* For WiFi, client not reg yet, no need to retry, WiFi function can work any time when wlan.ko is insert into system*/
                    iRet = 0;
                }
                else
                {
                    /* other fail cases, stop */
                    break;
                }
            }
            if (!retry || iRet) {
                WMT_ERR_FUNC("mtk_wcn_hif_sdio_wmt_control(%d, TRUE) fail(%d) retry(%d)\n", sdioFuncType, iRet, retry);
            }
            else
            {
                osal_set_bit(statBit, &pDev->state);
            }
        }
    }
    else  {
        if (osal_test_bit(statBit, &pDev->state)) {
            iRet = mtk_wcn_hif_sdio_wmt_control(sdioFuncType, MTK_WCN_BOOL_FALSE);
            if (iRet) {
                WMT_ERR_FUNC("mtk_wcn_hif_sdio_wmt_control(%d, FALSE) fail(%d)\n", sdioFuncType, iRet);
            }
            /*any way, set to OFF state*/
            osal_clear_bit(statBit, &pDev->state);
        }
        else {
            WMT_WARN_FUNC("CTRL_SDIO_FUNC(%d) but already OFF \n", sdioFuncType);
            iRet = 0;
        }
    }

    return iRet;
}

INT32 wmt_ctrl_hwidver_set(P_WMT_CTRL_DATA pWmtCtrlData)
{
    P_DEV_WMT pDev = &gDevWmt; /* single instance */

    /* input sanity check is done in wmt_ctrl() */
    pDev->chip_id = (pWmtCtrlData->au4CtrlData[0] & 0xFFFF0000) >> 16;
    pDev->hw_ver = pWmtCtrlData->au4CtrlData[0] & 0x0000FFFF;
    pDev->fw_ver = pWmtCtrlData->au4CtrlData[1] & 0x0000FFFF;

    // TODO: [FixMe][GeorgeKuo] remove translated ENUM_WMTHWVER_TYPE_T in the future!!!
    // Only use hw_ver read from hw.
    pDev->eWmtHwVer =
        (ENUM_WMTHWVER_TYPE_T)(pWmtCtrlData->au4CtrlData[1] & 0xFFFF0000) >> 16;

    return 0;
}

static INT32
wmt_ctrl_gps_sync_set (
    P_WMT_CTRL_DATA pData
    )
{
    INT32 iret;

    WMT_INFO_FUNC("ctrl GPS_SYNC(%d)\n", (0 == pData->au4CtrlData[0]) ? PIN_STA_DEINIT : PIN_STA_MUX);
    iret = wmt_plat_gpio_ctrl(PIN_GPS_SYNC,
        (0 == pData->au4CtrlData[0]) ? PIN_STA_DEINIT : PIN_STA_MUX);

    if (iret) {
        WMT_WARN_FUNC("ctrl GPS_SYNC(%d) fail!(%d) ignore it...\n",
            (0 == pData->au4CtrlData[0]) ? PIN_STA_DEINIT : PIN_STA_MUX,
            iret);
    }

    return 0;
}


static INT32
wmt_ctrl_gps_lna_set (
    P_WMT_CTRL_DATA pData
    )
{
    INT32 iret;

    WMT_INFO_FUNC("ctrl GPS_LNA(%d)\n", (0 == pData->au4CtrlData[0]) ? PIN_STA_DEINIT : PIN_STA_OUT_H);
    iret = wmt_plat_gpio_ctrl(PIN_GPS_LNA,
        (0 == pData->au4CtrlData[0]) ? PIN_STA_DEINIT : PIN_STA_OUT_H);

    if (iret) {
        WMT_WARN_FUNC("ctrl GPS_SYNC(%d) fail!(%d) ignore it...\n",
            (0 == pData->au4CtrlData[0]) ? PIN_STA_DEINIT : PIN_STA_OUT_H,
            iret);
    }

    return 0;
}


INT32  wmt_ctrl_stp_rst(P_WMT_CTRL_DATA pWmtCtrlData)
{
    return 0;
}

INT32  wmt_ctrl_get_wmt_conf(P_WMT_CTRL_DATA pWmtCtrlData)
{
    P_DEV_WMT pDev = &gDevWmt; /* single instance */

    pWmtCtrlData->au4CtrlData[0] = (UINT32)&pDev->rWmtGenConf;

    return 0;
}

INT32  wmt_ctrl_others(P_WMT_CTRL_DATA pWmtCtrlData)
{
    WMT_ERR_FUNC("wmt_ctrl_others, invalid CTRL ID (%d)\n", pWmtCtrlData->ctrlId);
    return -1;
}




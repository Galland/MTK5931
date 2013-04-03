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


/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

#include "stp_dbg.h"
#include "stp_core.h"
#include "btm_core.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

#define PFX_BTM "[BTM]"
#define STP_BTM_LOG_LOUD (4)
#define STP_BTM_LOG_DBG (3)
#define STP_BTM_LOG_INFO (2)
#define STP_BTM_LOG_WARN (1)
#define STP_BTM_LOG_ERR (0)

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

INT32 gBtmDbgLevel = STP_BTM_LOG_INFO;

MTKSTP_BTM_T stp_btm_i;
MTKSTP_BTM_T *stp_btm = &stp_btm_i;

const CHAR *g_btm_op_name[]={
        "STP_OPID_BTM_RETRY",
        "STP_OPID_BTM_RST",
        "STP_OPID_BTM_DBG_DUMP",
        "STP_OPID_BTM_EXIT"
    };

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

#define STP_BTM_LOUD_FUNC(fmt, arg...)   if (gBtmDbgLevel >= STP_BTM_LOG_LOUD){ osal_loud_print(PFX_BTM "%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_BTM_DBG_FUNC(fmt, arg...)    if (gBtmDbgLevel >= STP_BTM_LOG_DBG){  osal_dbg_print(PFX_BTM "%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_BTM_INFO_FUNC(fmt, arg...)   if (gBtmDbgLevel >= STP_BTM_LOG_INFO){ osal_info_print(PFX_BTM "[I]%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_BTM_WARN_FUNC(fmt, arg...)   if (gBtmDbgLevel >= STP_BTM_LOG_WARN){ osal_warn_print(PFX_BTM "[W]%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_BTM_ERR_FUNC(fmt, arg...)    if (gBtmDbgLevel >= STP_BTM_LOG_ERR){  osal_err_print(PFX_BTM "[E]%s(%d):ERROR! "   fmt, __FUNCTION__ , __LINE__, ##arg);}
#define STP_BTM_TRC_FUNC(f)              if (gBtmDbgLevel >= STP_BTM_LOG_DBG){  osal_dbg_print(PFX_BTM "<%s> <%d>\n", __FUNCTION__, __LINE__);}


/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/


/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

static INT32 _stp_btm_handler(MTKSTP_BTM_T *stp_btm, P_STP_BTM_OP pStpOp)
{
    INT32 ret = -1;

    if (NULL == pStpOp)
    {
        return -1;
    }

    switch(pStpOp->opId)
    {
        case STP_OPID_BTM_EXIT:
            // TODO: clean all up?
            ret = 0;
        break;

        /*tx timeout retry*/
        case STP_OPID_BTM_RETRY:
            stp_do_tx_timeout();
            ret = 0;

        break;

        /*whole chip reset*/
        case STP_OPID_BTM_RST:
            STP_BTM_INFO_FUNC("whole chip reset start!\n");
            STP_BTM_INFO_FUNC("....+\n");
            if (stp_btm->wmt_notify)
            {
                stp_btm->wmt_notify(BTM_RST_OP);
                ret = 0;
            }
            else
            {
                STP_BTM_ERR_FUNC("stp_btm->wmt_notify is NULL.");
                ret = -1;
            }

            STP_BTM_INFO_FUNC("whole chip reset end!\n");

        break;

        case STP_OPID_BTM_DBG_DUMP:
            /*Notify the wmt to get dump data*/
            STP_BTM_DBG_FUNC("wmt dmp notification\n");
            //btm_dump_data(stp_btm, "stp trace", 4);
            if (stp_btm->wmt_notify)
            {
                stp_btm->wmt_notify(BTM_DMP_OP);
                ret = 0;
            }
            else
            {
                STP_BTM_ERR_FUNC("stp_btm->wmt_notify is NULL.");
                ret = -1;
            }

        break;

        default:
            ret = -1;
        break;
    }

    return ret;
}

static P_OSAL_OP _stp_btm_get_op (
    MTKSTP_BTM_T *stp_btm,
    P_OSAL_OP_Q pOpQ
    )
{
    P_OSAL_OP pOp;

    if (!pOpQ)
    {
        STP_BTM_WARN_FUNC("!pOpQ \n");
        return NULL;
    }

    osal_lock_unsleepable_lock(&(stp_btm->wq_spinlock));
    /* acquire lock success */
    RB_GET(pOpQ, pOp);
    osal_unlock_unsleepable_lock(&(stp_btm->wq_spinlock));

    if (!pOp)
    {
        STP_BTM_WARN_FUNC("RB_GET fail\n");
    }

    return pOp;
}

static INT32 _stp_btm_put_op (
    MTKSTP_BTM_T *stp_btm,
    P_OSAL_OP_Q pOpQ,
    P_OSAL_OP pOp
    )
{
    INT32 ret;

    if (!pOpQ || !pOp)
    {
        STP_BTM_WARN_FUNC("invalid input param: 0x%p, 0x%p \n", pOpQ, pOp);
        return 0;//;MTK_WCN_BOOL_FALSE;
    }

    ret = 0;

    osal_lock_unsleepable_lock(&(stp_btm->wq_spinlock));
    /* acquire lock success */
    if (!RB_FULL(pOpQ))
    {
        RB_PUT(pOpQ, pOp);
    }
    else
    {
        ret = -1;
    }
    osal_unlock_unsleepable_lock(&(stp_btm->wq_spinlock));

    if (ret)
    {
        STP_BTM_WARN_FUNC("RB_FULL(0x%p) %d ,rFreeOpQ = %p, rActiveOpQ = %p\n", pOpQ, RB_COUNT(pOpQ),
            &stp_btm->rFreeOpQ,  &stp_btm->rActiveOpQ);
        return 0;
    }
    else
    {
        //STP_BTM_WARN_FUNC("RB_COUNT = %d\n",RB_COUNT(pOpQ));
        return 1;
    }
}

P_OSAL_OP _stp_btm_get_free_op (
    MTKSTP_BTM_T *stp_btm
    )
{
    P_OSAL_OP pOp;

    if (stp_btm)
    {
        pOp = _stp_btm_get_op(stp_btm, &stp_btm->rFreeOpQ);
        if (pOp)
        {
            osal_memset(&pOp->op, 0, sizeof(pOp->op));
        }
        return pOp;
    }
    else {
        return NULL;
    }
}

INT32 _stp_btm_put_act_op (
    MTKSTP_BTM_T *stp_btm,
    P_OSAL_OP pOp
    )
{
    INT32 bRet = 0;
    INT32 bCleanup = 0;
    LONG wait_ret = -1;

    P_OSAL_SIGNAL pSignal = NULL;

    do {
        if (!stp_btm || !pOp)
        {
            break;
        }

        pSignal = &pOp->signal;

        if (pSignal->timeoutValue)
        {
            pOp->result = -9;
            osal_signal_init(&pOp->signal);
        }

        /* put to active Q */
        bRet = _stp_btm_put_op(stp_btm, &stp_btm->rActiveOpQ, pOp);
        if (0 == bRet)
        {
            STP_BTM_WARN_FUNC("put active queue fail\n");
            bCleanup = 1;//MTK_WCN_BOOL_TRUE;
            break;
        }

        /* wake up wmtd */
        osal_trigger_event(&stp_btm->BTMd_event);

        if (pSignal->timeoutValue == 0)
        {
            bRet = 1;//MTK_WCN_BOOL_TRUE;
            /* clean it in wmtd */
            break;
        }

        /* wait result, clean it here */
        bCleanup = 1;//MTK_WCN_BOOL_TRUE;

        /* check result */
        wait_ret = osal_wait_for_signal_timeout(&pOp->signal);

        STP_BTM_DBG_FUNC("wait completion:%ld\n", wait_ret);
        if (!wait_ret)
        {
            STP_BTM_ERR_FUNC("wait completion timeout \n");
            // TODO: how to handle it? retry?
        }
        else
        {
            if (pOp->result)
            {
                STP_BTM_WARN_FUNC("op(%d) result:%d\n", pOp->op.opId, pOp->result);
            }

            bRet = (pOp->result) ? 0 : 1;
        }
    } while(0);

    if (bCleanup) {
        /* put Op back to freeQ */
        _stp_btm_put_op(stp_btm, &stp_btm->rFreeOpQ, pOp);
    }

    return bRet;
}

static INT32 _stp_btm_wait_for_msg(void *pvData)
{
    MTKSTP_BTM_T *stp_btm = (MTKSTP_BTM_T *)pvData;
    return ((!RB_EMPTY(&stp_btm->rActiveOpQ)) || osal_thread_should_stop(&stp_btm->BTMd));
}

static INT32 _stp_btm_proc (void *pvData)
{
    MTKSTP_BTM_T *stp_btm = (MTKSTP_BTM_T *)pvData;
    P_OSAL_OP pOp;
    INT32 id;
    INT32 result;

    if (!stp_btm)
    {
        STP_BTM_WARN_FUNC("!stp_btm \n");
        return -1;
    }

    for (;;)
    {
        pOp = NULL;

        osal_wait_for_event(&stp_btm->BTMd_event,
            _stp_btm_wait_for_msg,
            (void *)stp_btm
            );

        if (osal_thread_should_stop(&stp_btm->BTMd))
        {
            STP_BTM_INFO_FUNC("should stop now... \n");
            // TODO: clean up active opQ
            break;
        }

        /* get Op from activeQ */
        pOp = _stp_btm_get_op(stp_btm, &stp_btm->rActiveOpQ);

        if (!pOp)
        {
            STP_BTM_WARN_FUNC("get_lxop activeQ fail\n");
            continue;
        }

        id = osal_op_get_id(pOp);

        STP_BTM_DBG_FUNC("======> lxop_get_opid = %d, %s, remaining count = *%d*\n",
            id, (id >= 4)?("???"):(g_btm_op_name[id]), RB_COUNT(&stp_btm->rActiveOpQ));

        if (id >= STP_OPID_BTM_NUM)
        {
            STP_BTM_WARN_FUNC("abnormal opid id: 0x%x \n", id);
            result = -1;
            goto handler_done;
        }

        result = _stp_btm_handler(stp_btm, &pOp->op);

handler_done:

        if (result)
        {
            STP_BTM_WARN_FUNC("opid id(0x%x)(%s) error(%d)\n", id, (id >= 4)?("???"):(g_btm_op_name[id]), result);
        }

        if (osal_op_is_wait_for_signal(pOp))
        {
            osal_op_raise_signal(pOp, result);
        }
        else
        {
            /* put Op back to freeQ */
            _stp_btm_put_op(stp_btm, &stp_btm->rFreeOpQ, pOp);
        }

        if (STP_OPID_BTM_EXIT == id)
        {
            break;
        }
    }

    STP_BTM_INFO_FUNC("exits \n");

    return 0;
};

static inline INT32 _stp_btm_notify_wmt_rst_wq(MTKSTP_BTM_T *stp_btm)
{

    P_OSAL_OP       pOp;
    INT32           bRet;
    INT32 retval;

    if (stp_btm == NULL)
    {
        return STP_BTM_OPERATION_FAIL;
    }
    else
    {
        pOp = _stp_btm_get_free_op(stp_btm);
        if (!pOp)
        {
            STP_BTM_WARN_FUNC("get_free_lxop fail \n");
            return -1;//break;
        }
        pOp->op.opId = STP_OPID_BTM_RST;
        pOp->signal.timeoutValue= 0;
        bRet = _stp_btm_put_act_op(stp_btm, pOp);
        STP_BTM_DBG_FUNC("OPID(%d) type(%d) bRet(%d) \n\n",
            pOp->op.opId,
            pOp->op.au4OpData[0],
            bRet);
        retval = (0 == bRet) ? STP_BTM_OPERATION_FAIL : STP_BTM_OPERATION_SUCCESS;
    }
    return retval;
}

static inline INT32 _stp_btm_notify_stp_retry_wq(MTKSTP_BTM_T *stp_btm){

    P_OSAL_OP       pOp;
    INT32           bRet;
    INT32 retval;

    if (stp_btm == NULL)
    {
        return STP_BTM_OPERATION_FAIL;
    }
    else
    {
        pOp = _stp_btm_get_free_op(stp_btm);
        if (!pOp)
        {
            STP_BTM_WARN_FUNC("get_free_lxop fail \n");
            return -1;//break;
        }
        pOp->op.opId = STP_OPID_BTM_RETRY;
        pOp->signal.timeoutValue= 0;
        bRet = _stp_btm_put_act_op(stp_btm, pOp);
        STP_BTM_DBG_FUNC("OPID(%d) type(%d) bRet(%d) \n\n",
            pOp->op.opId,
            pOp->op.au4OpData[0],
            bRet);
        retval = (0 == bRet) ? STP_BTM_OPERATION_FAIL : STP_BTM_OPERATION_SUCCESS;
    }
    return retval;
}


static inline INT32 _stp_btm_notify_wmt_dmp_wq(MTKSTP_BTM_T *stp_btm){

    P_OSAL_OP     pOp;
    INT32         bRet;
    INT32 retval;

    if (stp_btm == NULL)
    {
        return STP_BTM_OPERATION_FAIL;
    }
    else
    {
        pOp = _stp_btm_get_free_op(stp_btm);
        if (!pOp)
        {
            STP_BTM_WARN_FUNC("get_free_lxop fail \n");
            return -1;//break;
        }
        pOp->op.opId = STP_OPID_BTM_DBG_DUMP;
        pOp->signal.timeoutValue= 0;
        bRet = _stp_btm_put_act_op(stp_btm, pOp);
        STP_BTM_DBG_FUNC("OPID(%d) type(%d) bRet(%d) \n\n",
            pOp->op.opId,
            pOp->op.au4OpData[0],
            bRet);
        retval = (0 == bRet) ? STP_BTM_OPERATION_FAIL : STP_BTM_OPERATION_SUCCESS;
    }
    return retval;
}

INT32 stp_btm_notify_wmt_rst_wq(MTKSTP_BTM_T *stp_btm)
{
    return _stp_btm_notify_wmt_rst_wq(stp_btm);
}

INT32 stp_btm_notify_stp_retry_wq(MTKSTP_BTM_T *stp_btm)
{
    return _stp_btm_notify_stp_retry_wq(stp_btm);
}

INT32 stp_btm_notify_wmt_dmp_wq(MTKSTP_BTM_T *stp_btm)
{
    return _stp_btm_notify_wmt_dmp_wq(stp_btm);
}

MTKSTP_BTM_T *stp_btm_init(void)
{
    INT32 i = 0x0;
    INT32 ret =-1;

    STP_BTM_DBG_FUNC("btm init (0x%p,%d)\n", stp_btm, sizeof(MTKSTP_BTM_T));

    osal_unsleepable_lock_init(&stp_btm->wq_spinlock);
    osal_event_init(&stp_btm->BTMd_event);
    stp_btm->wmt_notify = wmt_lib_btm_cb;

    RB_INIT(&stp_btm->rFreeOpQ, STP_BTM_OP_BUF_SIZE);
    RB_INIT(&stp_btm->rActiveOpQ, STP_BTM_OP_BUF_SIZE);

    /* Put all to free Q */
    for (i = 0; i < STP_BTM_OP_BUF_SIZE; i++)
    {
         osal_signal_init(&(stp_btm->arQue[i].signal));
         _stp_btm_put_op(stp_btm, &stp_btm->rFreeOpQ, &(stp_btm->arQue[i]));
    }

    /*Generate PSM thread, to servie STP-CORE for packet retrying and core dump receiving*/
    stp_btm->BTMd.pThreadData = (VOID *)stp_btm;
    stp_btm->BTMd.pThreadFunc = (VOID *)_stp_btm_proc;
    osal_memcpy(stp_btm->BTMd.threadName, BTM_THREAD_NAME , osal_strlen(BTM_THREAD_NAME));

    ret = osal_thread_create(&stp_btm->BTMd);
    if (ret < 0)
    {
        STP_BTM_ERR_FUNC("osal_thread_create fail...\n");
        goto ERR_EXIT1;
    }

    /* Start STPd thread*/
    ret = osal_thread_run(&stp_btm->BTMd);
    if (ret < 0)
    {
        STP_BTM_ERR_FUNC("osal_thread_run FAILS\n");
        goto ERR_EXIT1;
    }

    return stp_btm;

ERR_EXIT1:

    return NULL;

}

INT32 stp_btm_deinit(MTKSTP_BTM_T *stp_btm){

    UINT32 ret = -1;
    INT32 i = 0;

    STP_BTM_DBG_FUNC("btm deinit\n");

    if (!stp_btm)
    {
        return STP_BTM_OPERATION_FAIL;
    }

    ret = osal_thread_destroy(&stp_btm->BTMd);
    for (i = 0; i < STP_BTM_OP_BUF_SIZE; i++)
    {
         osal_signal_deinit(&(stp_btm->arQue[i].signal));
    }
    ret = osal_event_deinit(&stp_btm->BTMd_event);
    ret = osal_unsleepable_lock_deinit(&stp_btm->wq_spinlock);

    if (ret < 0)
    {
        STP_BTM_ERR_FUNC("osal_thread_destroy FAILS\n");
        return STP_BTM_OPERATION_FAIL;
    }

    return STP_BTM_OPERATION_SUCCESS;
}


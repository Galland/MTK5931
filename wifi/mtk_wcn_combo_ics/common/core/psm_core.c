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

#include "osal_typedef.h"
#include "osal.h"
#include "psm_core.h"
#include "stp_core.h"
#include <mach/mtk_wcn_cmb_stub.h>

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/


/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

INT32         gPsmDbgLevel = STP_PSM_LOG_INFO;
MTKSTP_PSM_T  stp_psm_i;
MTKSTP_PSM_T  *stp_psm = &stp_psm_i;

/*******************************************************************************
*                            P R I V A T E  D A T A
********************************************************************************
*/

static const CHAR *g_psm_state[STP_PSM_MAX_STATE] = {
    "ACT",
    "ACT_INACT",
    "INACT",
    "INACT_ACT"
};

static const CHAR *g_psm_action[STP_PSM_MAX_ACTION] = {
    "SLEEP" ,
    "HOST_AWAKE",
    "WAKEUP",
    "EIRQ",
    "ROLL_BACK"
};

static const CHAR *g_psm_op_name[STP_OPID_PSM_NUM] = {
    "STP_OPID_PSM_SLEEP",
    "STP_OPID_PSM_WAKEUP",
    "STP_OPID_PSM_HOST_AWAKE",
    "STP_OPID_PSM_EXIT"
};


/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

#define STP_PSM_LOUD_FUNC(fmt, arg...)   if (gPsmDbgLevel >= STP_PSM_LOG_LOUD){ osal_loud_print(PFX_PSM "%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_PSM_DBG_FUNC(fmt, arg...)    if (gPsmDbgLevel >= STP_PSM_LOG_DBG){  osal_dbg_print(PFX_PSM "%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_PSM_INFO_FUNC(fmt, arg...)   if (gPsmDbgLevel >= STP_PSM_LOG_INFO){ osal_info_print(PFX_PSM "[I]%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_PSM_WARN_FUNC(fmt, arg...)   if (gPsmDbgLevel >= STP_PSM_LOG_WARN){ osal_warn_print(PFX_PSM "[W]%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_PSM_ERR_FUNC(fmt, arg...)    if (gPsmDbgLevel >= STP_PSM_LOG_ERR){  osal_err_print(PFX_PSM "[E]%s(%d):ERROR! "   fmt, __FUNCTION__ , __LINE__, ##arg);}
#define STP_PSM_TRC_FUNC(f)              if (gPsmDbgLevel >= STP_PSM_LOG_DBG){  osal_dbg_print(PFX_PSM "<%s> <%d>\n", __FUNCTION__, __LINE__);}

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

static inline INT32 _stp_psm_notify_wmt(MTKSTP_PSM_T *stp_psm, const MTKSTP_PSM_ACTION_T action);

/*******************************************************************************
*                           P R I V A T E   F U N C T I O N S
********************************************************************************
*/

/* change from macro to static function to enforce type checking on parameters. */
static INT32 psm_fifo_lock_init (MTKSTP_PSM_T *psm)
{
#if CFG_PSM_CORE_FIFO_SPIN_LOCK
    return osal_unsleepable_lock_init(&(psm->hold_fifo_lock));
#else
    return osal_sleepable_lock_init(&(psm->hold_fifo_lock));
#endif
}

static INT32 psm_fifo_lock_deinit (MTKSTP_PSM_T *psm)
{
#if CFG_PSM_CORE_FIFO_SPIN_LOCK
    return osal_unsleepable_lock_deinit(&(psm->hold_fifo_lock));
#else
    return osal_sleepable_lock_deinit(&(psm->hold_fifo_lock));
#endif
}

static INT32 psm_fifo_lock (MTKSTP_PSM_T *psm)
{
#if CFG_PSM_CORE_FIFO_SPIN_LOCK
    return osal_lock_unsleepable_lock(&(psm->hold_fifo_lock));
#else
    return osal_lock_sleepable_lock(&(psm->hold_fifo_lock));
#endif
}

static INT32 psm_fifo_unlock (MTKSTP_PSM_T *psm)
{
#if CFG_PSM_CORE_FIFO_SPIN_LOCK
    return osal_unlock_unsleepable_lock(&(psm->hold_fifo_lock));
#else
    return osal_unlock_sleepable_lock(&(psm->hold_fifo_lock));
#endif
}

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

MTK_WCN_BOOL mtk_wcn_stp_psm_dbg_level(UINT32 dbglevel)
{
   if (0 <= dbglevel && dbglevel <= 4)
   {
      gPsmDbgLevel = dbglevel;
      STP_PSM_INFO_FUNC("gPsmDbgLevel = %d\n", gPsmDbgLevel);
      return MTK_WCN_BOOL_TRUE;
   }
   else
   {
      STP_PSM_INFO_FUNC("invalid psm debug level. gPsmDbgLevel = %d\n", gPsmDbgLevel);
   }
   return MTK_WCN_BOOL_FALSE;
}



static INT32 _stp_psm_handler(MTKSTP_PSM_T *stp_psm, P_STP_OP pStpOp)
{
    INT32 ret = -1;

    if (NULL == pStpOp)
    {
        return -1;
    }

    switch(pStpOp->opId)
    {
        case STP_OPID_PSM_EXIT:
            // TODO: clean all up?
            ret = 0;
            break;

        case STP_OPID_PSM_SLEEP:
            ret =_stp_psm_notify_wmt(stp_psm, SLEEP);
            break;

        case STP_OPID_PSM_WAKEUP:
           ret = _stp_psm_notify_wmt(stp_psm, WAKEUP);
           break;

        case STP_OPID_PSM_HOST_AWAKE:
           ret = _stp_psm_notify_wmt(stp_psm, HOST_AWAKE);
           break;

        default:
           STP_PSM_ERR_FUNC("invalid operation id (%d)\n", pStpOp->opId);
           ret = -1;
           break;
    }

    return ret;
}

static P_OSAL_OP _stp_psm_get_op (
    MTKSTP_PSM_T *stp_psm,
    P_OSAL_OP_Q pOpQ
    )
{
    P_OSAL_OP pOp;

    if (!pOpQ)
    {
        STP_PSM_WARN_FUNC("pOpQ == NULL\n");
        return NULL;
    }

    osal_lock_unsleepable_lock(&(stp_psm->wq_spinlock));
    /* acquire lock success */
    RB_GET(pOpQ, pOp);
    osal_unlock_unsleepable_lock(&(stp_psm->wq_spinlock));

    if (!pOp)
    {
        STP_PSM_WARN_FUNC("RB_GET fail\n");
    }

    return pOp;
}

static INT32 _stp_psm_dump_active_q(
    P_OSAL_OP_Q pOpQ
)
{
    UINT32 read_idx;
    UINT32 write_idx;
    UINT32 opId;

    if (pOpQ == &stp_psm->rActiveOpQ)
    {
        read_idx = stp_psm->rActiveOpQ.read;
        write_idx = stp_psm->rActiveOpQ.write;

        STP_PSM_DBG_FUNC("Active op list:++\n");
        while ((read_idx & RB_MASK(pOpQ)) != (write_idx & RB_MASK(pOpQ)))
        {
            opId = pOpQ->queue[read_idx & RB_MASK(pOpQ)]->op.opId;
            if (opId < STP_OPID_PSM_NUM)
            {
                STP_PSM_DBG_FUNC("%s\n", g_psm_op_name[opId] );
            }
            else
            {
                STP_PSM_WARN_FUNC("Unkown OP Id\n");
            }
            ++read_idx;
        }
        STP_PSM_DBG_FUNC("Active op list:--\n");
    }
    else
    {
        STP_PSM_ERR_FUNC("%s: not active queue, dont dump\n", __func__)
    }

    return 0;
}

static INT32 _stp_psm_put_op (
    MTKSTP_PSM_T *stp_psm,
    P_OSAL_OP_Q pOpQ,
    P_OSAL_OP pOp
    )
{
    INT32 ret;

    if (!pOpQ || !pOp)
    {
        STP_PSM_WARN_FUNC("pOpQ = 0x%p, pLxOp = 0x%p \n", pOpQ, pOp);
        return 0;
    }

    ret = 0;

    osal_lock_unsleepable_lock(&(stp_psm->wq_spinlock));
    /* acquire lock success */
    if (!RB_FULL(pOpQ))
    {
        RB_PUT(pOpQ, pOp);
    }
    else
    {
        ret = -1;
    }

    if (pOpQ == &stp_psm->rActiveOpQ)
    {
        _stp_psm_dump_active_q(&stp_psm->rActiveOpQ);
    }

    osal_unlock_unsleepable_lock(&(stp_psm->wq_spinlock));

    if (ret)
    {
        STP_PSM_WARN_FUNC("RB_FULL, RB_COUNT=%d , RB_SIZE=%d\n",RB_COUNT(pOpQ), RB_SIZE(pOpQ));
        return 0;
    }
    else
    {
        return 1;
    }
}

P_OSAL_OP _stp_psm_get_free_op (
    MTKSTP_PSM_T *stp_psm
    )
{
    P_OSAL_OP pOp;

    if (stp_psm)
    {
        pOp = _stp_psm_get_op(stp_psm, &stp_psm->rFreeOpQ);
        if (pOp)
        {
            osal_memset(&pOp->op, 0, sizeof(pOp->op));
        }
        return pOp;
    }
    else
    {
        return NULL;
    }
}

INT32 _stp_psm_put_act_op (
    MTKSTP_PSM_T *stp_psm,
    P_OSAL_OP pOp
    )
{
    INT32 bRet = 0;//MTK_WCN_BOOL_FALSE;
    INT32 bCleanup = 0;//MTK_WCN_BOOL_FALSE;
    INT32 wait_ret = -1;
    P_OSAL_SIGNAL pSignal = NULL;

    do
    {
        if (!stp_psm || !pOp)
        {
            STP_PSM_ERR_FUNC("stp_psm = %p, pOp = %p\n", stp_psm, pOp);
            break;
        }

        pSignal = &pOp->signal;

        if (pSignal->timeoutValue)
        {
            pOp->result = -9;
            osal_signal_init(&pOp->signal);
        }

        /* put to active Q */
        bRet = _stp_psm_put_op(stp_psm, &stp_psm->rActiveOpQ, pOp);

        if (0 == bRet)
        {
            STP_PSM_WARN_FUNC("+++++++++++ Put op Active queue Fail\n");
            bCleanup = 1;//MTK_WCN_BOOL_TRUE;
            break;
        }

        /* wake up wmtd */
        osal_trigger_event(&stp_psm->PSMd_event);

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
        STP_PSM_DBG_FUNC("wait completion:%d\n", wait_ret);
        if (!wait_ret)
        {
            STP_PSM_ERR_FUNC("wait completion timeout \n");
            // TODO: how to handle it? retry?
        }
        else
        {
            if (pOp->result)
            {
                STP_PSM_WARN_FUNC("op(%d) result:%d\n", pOp->op.opId, pOp->result);
            }
            /* op completes, check result */
            bRet = (pOp->result) ? 0 : 1;
        }
    } while(0);

    if (bCleanup) {
        /* put Op back to freeQ */
        bRet = _stp_psm_put_op(stp_psm, &stp_psm->rFreeOpQ, pOp);
        if (bRet == 0)
        {
            STP_PSM_WARN_FUNC("+++++++++++ Put op active free fail, maybe disable/enable psm\n");
        }
    }

    return bRet;
}

static INT32 _stp_psm_wait_for_msg(void *pvData)
{
    MTKSTP_PSM_T *stp_psm = (MTKSTP_PSM_T *)pvData;
    STP_PSM_DBG_FUNC("%s: stp_psm->rActiveOpQ = %d\n", __func__, RB_COUNT(&stp_psm->rActiveOpQ));

    return ((!RB_EMPTY(&stp_psm->rActiveOpQ)) || osal_thread_should_stop(&stp_psm->PSMd));
}

static INT32 _stp_psm_proc (void *pvData)
{
    MTKSTP_PSM_T *stp_psm = (MTKSTP_PSM_T *)pvData;
    P_OSAL_OP pOp;
    UINT32 id;
    INT32 result;

    if (!stp_psm) {
        STP_PSM_WARN_FUNC("!stp_psm \n");
        return -1;
    }

//    STP_PSM_INFO_FUNC("wmtd starts running: pWmtDev(0x%p) [pol, rt_pri, n_pri, pri]=[%d, %d, %d, %d] \n",
//        stp_psm, current->policy, current->rt_priority, current->normal_prio, current->prio);

    for (;;) {

        pOp = NULL;

        osal_wait_for_event(&stp_psm->PSMd_event,
            _stp_psm_wait_for_msg,
            (void *)stp_psm);

        //we set reset flag when calling stp_reset after cleanup all op.
        if (stp_psm->flag & STP_PSM_RESET_EN)
        {
            stp_psm->flag &= ~STP_PSM_RESET_EN;
        }

        if (osal_thread_should_stop(&stp_psm->PSMd))
        {
            STP_PSM_INFO_FUNC("should stop now... \n");
            // TODO: clean up active opQ
            break;
        }

        /* get Op from activeQ */
        pOp = _stp_psm_get_op(stp_psm, &stp_psm->rActiveOpQ);
        if (!pOp)
        {
            STP_PSM_WARN_FUNC("+++++++++++ Get op from activeQ fail, maybe disable/enable psm\n");
            continue;
        }

        id = osal_op_get_id(pOp);

        if (id >= STP_OPID_PSM_NUM)
        {
            STP_PSM_WARN_FUNC("abnormal opid id: 0x%x \n", id);
            result = -1;
            goto handler_done;
        }

        result = _stp_psm_handler(stp_psm, &pOp->op);

handler_done:

        if (result)
        {
            STP_PSM_WARN_FUNC("opid id(0x%x)(%s) error(%d)\n", id, (id >= 4)?("???"):(g_psm_op_name[id]), result);
        }

        if (osal_op_is_wait_for_signal(pOp))
        {
            osal_op_raise_signal(pOp, result);
        }
        else
        {
           /* put Op back to freeQ */
           if (_stp_psm_put_op(stp_psm, &stp_psm->rFreeOpQ, pOp) == 0)
           {
                STP_PSM_WARN_FUNC("+++++++++++ Put op to FreeOpQ fail, maybe disable/enable psm\n");
           }
        }

        if (STP_OPID_PSM_EXIT == id)
        {
            break;
        }
    }
    STP_PSM_INFO_FUNC("exits \n");

    return 0;
};

static inline INT32 _stp_psm_get_time(void)
{
    if (gPsmDbgLevel >= STP_PSM_LOG_LOUD)
    {
        osal_printtimeofday("<psm time>>>>");
    }

    return 0;
}

static inline INT32  _stp_psm_get_state(MTKSTP_PSM_T *stp_psm)
{

    if (stp_psm == NULL)
    {
        return STP_PSM_OPERATION_FAIL;
    }
    else
    {
        if (stp_psm->work_state < STP_PSM_MAX_STATE)
        {
            return stp_psm->work_state;
        }
        else
        {
            STP_PSM_ERR_FUNC("work_state = %d, invalid\n", stp_psm->work_state);

            return -STP_PSM_OPERATION_FAIL;
        }
    }
}

static inline INT32 _stp_psm_set_state(MTKSTP_PSM_T *stp_psm,const MTKSTP_PSM_STATE_T state)
{
    if (stp_psm == NULL)
    {
        return STP_PSM_OPERATION_FAIL;
    }
    else
    {
         if (stp_psm->work_state < STP_PSM_MAX_STATE)
         {
            _stp_psm_get_time();
            STP_PSM_INFO_FUNC("work_state = %s --> %s\n", g_psm_state[stp_psm->work_state], g_psm_state[state]);

            stp_psm->work_state = state;
            if (stp_psm->work_state != ACT)
            {
//                osal_lock_unsleepable_lock(&stp_psm->flagSpinlock);
                stp_psm->flag |= STP_PSM_BLOCK_DATA_EN;
//                osal_unlock_unsleepable_lock(&stp_psm->flagSpinlock);
            }
         }
         else
         {
            STP_PSM_ERR_FUNC("work_state = %d, invalid\n", stp_psm->work_state);
         }
    }

    return STP_PSM_OPERATION_SUCCESS;
}

static inline INT32 _stp_psm_start_monitor(MTKSTP_PSM_T *stp_psm)
{

     if (!stp_psm)
     {
        return STP_PSM_OPERATION_FAIL;
     }

     if ((stp_psm->flag & STP_PSM_WMT_EVENT_DISABLE_MONITOR)!= 0)
     {
        STP_PSM_DBG_FUNC("STP-PSM DISABLE, DONT restart monitor!\n\r");
        return STP_PSM_OPERATION_SUCCESS;
     }


     STP_PSM_LOUD_FUNC("start monitor\n");
     osal_timer_modify(&stp_psm->psm_timer, stp_psm->idle_time_to_sleep);

     return STP_PSM_OPERATION_SUCCESS;
}

static inline INT32 _stp_psm_stop_monitor(MTKSTP_PSM_T *stp_psm)
{

    if (!stp_psm)
    {
        return STP_PSM_OPERATION_FAIL;
    }
    else
    {
        STP_PSM_DBG_FUNC("stop monitor\n");
        osal_timer_stop_sync(&stp_psm->psm_timer);
    }

    return STP_PSM_OPERATION_SUCCESS;
}

INT32
_stp_psm_hold_data (
    MTKSTP_PSM_T *stp_psm,
    const UINT8 *buffer,
    const UINT32 len,
    const UINT8 type
    )
{
    INT32 available_space = 0;
    INT32 needed_space = 0;
    UINT8 delimiter [] = {0xbb, 0xbb};

    if (!stp_psm)
    {
        return STP_PSM_OPERATION_FAIL;
    }
    else
    {
        //osal_lock_unsleepable_lock(&stp_psm->hold_fifo_spinlock_global);
        psm_fifo_lock(stp_psm);

        available_space = STP_PSM_FIFO_SIZE- osal_fifo_len(&stp_psm->hold_fifo);
        needed_space = len + sizeof(UINT8) + sizeof(UINT32) + 2;

        STP_PSM_DBG_FUNC("*******FIFO Available(%d), Need(%d)\n", available_space, needed_space);

        if ( available_space < needed_space )
        {
            STP_PSM_ERR_FUNC("FIFO Available!! Reset FIFO\n");
            osal_fifo_reset(&stp_psm->hold_fifo);
        }
        //type
        osal_fifo_in(&stp_psm->hold_fifo,(UINT8 *) &type , sizeof(UINT8));
        //lenght
        osal_fifo_in(&stp_psm->hold_fifo,(UINT8 *) &len , sizeof(UINT32));
        //buffer
        osal_fifo_in(&stp_psm->hold_fifo,(UINT8 *) buffer, len);
        //delimiter
        osal_fifo_in(&stp_psm->hold_fifo,(UINT8 *) delimiter, 2);

        //osal_unlock_unsleepable_lock(&stp_psm->hold_fifo_spinlock_global);
        psm_fifo_unlock(stp_psm);

        return len;
    }
}

INT32 _stp_psm_has_pending_data(MTKSTP_PSM_T *stp_psm)
{
    return osal_fifo_len(&stp_psm->hold_fifo);
}

INT32 _stp_psm_release_data(MTKSTP_PSM_T *stp_psm)
{

    INT32 i = 20; /*Max buffered packet number*/
    INT32 ret = 0;
    UINT8 type = 0;
    UINT32  len = 0;
    UINT8 delimiter[2];

    //STP_PSM_ERR_FUNC("++++++++++release data++len=%d\n", osal_fifo_len(&stp_psm->hold_fifo));
    while(osal_fifo_len(&stp_psm->hold_fifo) && i > 0)
    {
        //acquire spinlock
        //osal_lock_unsleepable_lock(&stp_psm->hold_fifo_spinlock_global);
        psm_fifo_lock(stp_psm);

        ret = osal_fifo_out(&stp_psm->hold_fifo, (UINT8 *)&type, sizeof(UINT8));
        ret = osal_fifo_out(&stp_psm->hold_fifo, (UINT8 *)&len, sizeof(UINT32));

        if (len > STP_PSM_PACKET_SIZE_MAX)
        {
            STP_PSM_ERR_FUNC("***psm packet's length too Long!****\n");
            STP_PSM_INFO_FUNC("***reset psm's fifo***\n");
        }
        else
        {
            osal_memset(stp_psm->out_buf, 0, STP_PSM_TX_SIZE);
            ret = osal_fifo_out(&stp_psm->hold_fifo, (UINT8 *)stp_psm->out_buf, len);
        }

        ret = osal_fifo_out(&stp_psm->hold_fifo, (UINT8 *)delimiter, 2);

        if (delimiter[0]==0xbb && delimiter[1]==0xbb)
        {
            /* [George] move unlock call from below to here. fifo can be
             * unlocked here for all related operations are done. fifo lock has
             * no need to cover osal_buffer_dump() nor stp_send_data_no_ps().
             */
            psm_fifo_unlock(stp_psm);

            if (gPsmDbgLevel >= STP_PSM_LOG_DBG){
                osal_buffer_dump(stp_psm->out_buf, "psm->out_buf", len, 32);
            }
            stp_send_data_no_ps(stp_psm->out_buf, len, type);
        }
        else
        {
            STP_PSM_ERR_FUNC("***psm packet fifo parsing fail****\n");
            STP_PSM_INFO_FUNC("***reset psm's fifo***\n");

            osal_fifo_reset(&stp_psm->hold_fifo);

            /* move unlock call from below to here */
            psm_fifo_unlock(stp_psm);
        }

        /* fifo is unlocked already!! */
        i--;

        /* [George]move unlock forward to reduce locking range, and can
         * exclude stp_send_data_no_ps() call.
         */
        //osal_unlock_unsleepable_lock(&stp_psm->hold_fifo_spinlock_global);
        //psm_fifo_unlock(stp_psm);
    }
    return STP_PSM_OPERATION_SUCCESS;
}

static inline INT32 _stp_psm_notify_wmt_host_awake_wq(MTKSTP_PSM_T *stp_psm)
{

    P_OSAL_OP  pOp;
    INT32      bRet;
    INT32      retval;

    if (stp_psm == NULL)
    {
        return  STP_PSM_OPERATION_FAIL;
    }
    else
    {
        pOp = _stp_psm_get_free_op(stp_psm);
        if (!pOp)
        {
            STP_PSM_WARN_FUNC("get_free_lxop fail \n");
            return -1;//break;
        }

        pOp->op.opId = STP_OPID_PSM_HOST_AWAKE;
        pOp->signal.timeoutValue = 0;
        bRet = _stp_psm_put_act_op(stp_psm, pOp);

        STP_PSM_DBG_FUNC("OPID(%d) type(%d) bRet(%d) \n\n",
            pOp->op.opId,
            pOp->op.au4OpData[0],
            bRet);

        retval = (0 == bRet) ? (STP_PSM_OPERATION_FAIL) : 0;
    }
    return retval;
}

static inline INT32 _stp_psm_notify_wmt_wakeup_wq(MTKSTP_PSM_T *stp_psm)
{
    P_OSAL_OP  pOp;
    INT32      bRet;
    INT32      retval;

    if (stp_psm == NULL)
    {
        return (STP_PSM_OPERATION_FAIL);
    }
    else
    {
        pOp = _stp_psm_get_free_op(stp_psm);
        if (!pOp)
        {
            STP_PSM_WARN_FUNC("get_free_lxop fail \n");
            return -1;//break;
        }

        pOp->op.opId = STP_OPID_PSM_WAKEUP;
        pOp->signal.timeoutValue = 0;
        bRet = _stp_psm_put_act_op(stp_psm, pOp);

        STP_PSM_DBG_FUNC("OPID(%d) type(%d) bRet(%d) \n\n",
            pOp->op.opId,
            pOp->op.au4OpData[0],
            bRet);
        retval = (0 == bRet) ? (STP_PSM_OPERATION_FAIL) : (STP_PSM_OPERATION_SUCCESS);
    }
    return retval;
}

static inline INT32 _stp_psm_notify_wmt_sleep_wq(MTKSTP_PSM_T *stp_psm)
{
    P_OSAL_OP       pOp;
    INT32           bRet;
    INT32 retval;

    if (stp_psm == NULL)
    {
        return STP_PSM_OPERATION_FAIL;
    }
    else
    {
        pOp = _stp_psm_get_free_op(stp_psm);
        if (!pOp) {
            STP_PSM_WARN_FUNC("get_free_lxop fail \n");
            return -1;//break;
        }

        pOp->op.opId = STP_OPID_PSM_SLEEP;
        pOp->signal.timeoutValue = 0;
        bRet = _stp_psm_put_act_op(stp_psm, pOp);

        STP_PSM_DBG_FUNC("OPID(%d) type(%d) bRet(%d) \n\n",
            pOp->op.opId,
            pOp->op.au4OpData[0],
            bRet);

        retval = (0 == bRet) ? (STP_PSM_OPERATION_FAIL) : 0;
    }
    return retval;
}

/*internal function*/

static inline INT32 _stp_psm_reset(MTKSTP_PSM_T *stp_psm)
{
    INT32 i = 0;
    P_OSAL_OP_Q pOpQ;
    P_OSAL_OP pOp;

    STP_PSM_DBG_FUNC("PSM MODE RESET=============================>\n\r");

    STP_PSM_INFO_FUNC("_stp_psm_reset\n");
    STP_PSM_INFO_FUNC("reset-wake_lock(%d)\n", osal_wake_lock_count(&stp_psm->wake_lock));
    osal_wake_unlock(&stp_psm->wake_lock);
    STP_PSM_INFO_FUNC("reset-wake_lock(%d)\n", osal_wake_lock_count(&stp_psm->wake_lock));

    //--> serialized the request from wmt <--//
    osal_lock_sleepable_lock(&stp_psm->user_lock);

    //--> disable psm <--//
    stp_psm->flag = STP_PSM_WMT_EVENT_DISABLE_MONITOR;
    _stp_psm_stop_monitor(stp_psm);

    //--> prepare the op list <--//
    osal_lock_unsleepable_lock(&(stp_psm->wq_spinlock));
    RB_INIT(&stp_psm->rFreeOpQ, STP_OP_BUF_SIZE);
    RB_INIT(&stp_psm->rActiveOpQ, STP_OP_BUF_SIZE);

    pOpQ = &stp_psm->rFreeOpQ;
    for (i = 0; i < STP_OP_BUF_SIZE; i++)
    {
        if (!RB_FULL(pOpQ))
        {
            pOp = &stp_psm->arQue[i];
            RB_PUT(pOpQ, pOp);
        }
    }
    osal_unlock_unsleepable_lock(&(stp_psm->wq_spinlock));

    //--> clean up interal data structure<--//
    _stp_psm_set_state(stp_psm, ACT);

    //osal_lock_unsleepable_lock(&stp_psm->hold_fifo_spinlock_global);
    psm_fifo_lock(stp_psm);
    osal_fifo_reset(&stp_psm->hold_fifo);
    //osal_unlock_unsleepable_lock(&stp_psm->hold_fifo_spinlock_global);
    psm_fifo_unlock(stp_psm);

    //--> stop psm thread wait <--//
    stp_psm->flag |= STP_PSM_RESET_EN;
    osal_trigger_event(&stp_psm->wait_wmt_q);

    osal_unlock_sleepable_lock(&stp_psm->user_lock);

    STP_PSM_DBG_FUNC("PSM MODE RESET<============================\n\r");

    return STP_PSM_OPERATION_SUCCESS;
}

static INT32 _stp_psm_wait_wmt_event(void *pvData)
{
    MTKSTP_PSM_T *stp_psm = (MTKSTP_PSM_T *)pvData;

    STP_PSM_DBG_FUNC("stp_psm->flag=0x%08x\n", stp_psm->flag);

    return ((stp_psm->flag & STP_PSM_WMT_EVENT_SLEEP_EN) ||
        (stp_psm->flag & STP_PSM_WMT_EVENT_WAKEUP_EN) ||
        (stp_psm->flag & STP_PSM_WMT_EVENT_ROLL_BACK_EN) ||
        (stp_psm->flag & STP_PSM_RESET_EN));
}


static inline INT32 _stp_psm_wait_wmt_event_wq (MTKSTP_PSM_T *stp_psm)
{
    INT32 retval = 0;

    if (stp_psm == NULL)
    {
        return STP_PSM_OPERATION_FAIL;
    }
    else
    {
        osal_wait_for_event_timeout(&stp_psm->wait_wmt_q,
             _stp_psm_wait_wmt_event,
             (void *)stp_psm);

        if (stp_psm->flag & STP_PSM_WMT_EVENT_WAKEUP_EN)
        {
            stp_psm->flag &= ~STP_PSM_WMT_EVENT_WAKEUP_EN;
//            osal_lock_unsleepable_lock(&stp_psm->flagSpinlock);
            //STP send data here: STP enqueue data to psm buffer.
            _stp_psm_release_data(stp_psm);
            //STP send data here: STP enqueue data to psm buffer. We release packet by the next one.
            stp_psm->flag &= ~STP_PSM_BLOCK_DATA_EN;
            //STP send data here: STP sends data directly without PSM.
            _stp_psm_set_state(stp_psm, ACT);
//            osal_unlock_unsleepable_lock(&stp_psm->flagSpinlock);
            _stp_psm_start_monitor(stp_psm);
        }
        else if ( stp_psm->flag & STP_PSM_WMT_EVENT_SLEEP_EN)
        {
            stp_psm->flag &= ~STP_PSM_WMT_EVENT_SLEEP_EN;
            _stp_psm_set_state(stp_psm, INACT);

            STP_PSM_DBG_FUNC("mt_combo_plt_enter_deep_idle++\n");
            mt_combo_plt_enter_deep_idle(COMBO_IF_UART);
            STP_PSM_DBG_FUNC("mt_combo_plt_enter_deep_idle--\n");

            STP_PSM_DBG_FUNC("sleep-wake_lock(%d)\n", osal_wake_lock_count(&stp_psm->wake_lock));
            osal_wake_unlock(&stp_psm->wake_lock);
            STP_PSM_DBG_FUNC("sleep-wake_lock#(%d)\n", osal_wake_lock_count(&stp_psm->wake_lock));
        }
        else if ( stp_psm->flag & STP_PSM_WMT_EVENT_ROLL_BACK_EN)
        {
            stp_psm->flag &= ~STP_PSM_WMT_EVENT_ROLL_BACK_EN;
            if (_stp_psm_get_state(stp_psm) == ACT_INACT){
//                osal_lock_unsleepable_lock(&stp_psm->flagSpinlock);
                _stp_psm_release_data(stp_psm);
                stp_psm->flag &= ~STP_PSM_BLOCK_DATA_EN;
                _stp_psm_set_state(stp_psm, ACT);
//                osal_unlock_unsleepable_lock(&stp_psm->flagSpinlock);
            } else if (_stp_psm_get_state(stp_psm) == INACT_ACT) {
                _stp_psm_set_state(stp_psm, INACT);
                STP_PSM_INFO_FUNC("[WARNING]PSM state rollback due too wakeup fail\n");
            }
        }
        else if (stp_psm->flag & STP_PSM_RESET_EN)
        {
            stp_psm->flag &= ~STP_PSM_WMT_EVENT_ROLL_BACK_EN;
        }
        else
        {
            STP_PSM_ERR_FUNC("flag = %x<== Abnormal flag be set!!\n\r", stp_psm->flag);
            STP_PSM_ERR_FUNC("state = %d, flag = %d\n", stp_psm->work_state, stp_psm->flag);
        }
        retval = STP_PSM_OPERATION_SUCCESS;
    }

    return retval;
}

static inline INT32 _stp_psm_notify_stp(MTKSTP_PSM_T *stp_psm, const MTKSTP_PSM_ACTION_T action){

    INT32 retval = 0;
    if (action == EIRQ)
    {
        STP_PSM_DBG_FUNC("Call _stp_psm_notify_wmt_host_awake_wq\n\r");

        return _stp_psm_notify_wmt_host_awake_wq(stp_psm);

    }

    if ((_stp_psm_get_state(stp_psm) < STP_PSM_MAX_STATE) && (_stp_psm_get_state(stp_psm) >= 0))
    {
        STP_PSM_DBG_FUNC("state = %s, action=%s \n\r", g_psm_state[_stp_psm_get_state(stp_psm)], g_psm_action[action]);
    }

    // If STP trigger WAKEUP and SLEEP, to do the job below
    switch(_stp_psm_get_state(stp_psm))
    {
            //stp trigger
        case ACT_INACT:

            if (action == SLEEP)
            {
                STP_PSM_LOUD_FUNC("Action = %s, ACT_INACT state, ready to INACT\n\r", g_psm_action[action]);
                stp_psm->flag &= ~STP_PSM_WMT_EVENT_WAKEUP_EN;
                stp_psm->flag |= STP_PSM_WMT_EVENT_SLEEP_EN;
                //wake_up(&stp_psm->wait_wmt_q);
                osal_trigger_event(&stp_psm->wait_wmt_q);
            }
            else if (action == ROLL_BACK)
            {
                STP_PSM_LOUD_FUNC("Action = %s, ACT_INACT state, back to ACT\n\r", g_psm_action[action]);
                //stp_psm->flag &= ~STP_PSM_WMT_EVENT_ROLL_BACK_EN;
                stp_psm->flag |=  STP_PSM_WMT_EVENT_ROLL_BACK_EN;
                //wake_up(&stp_psm->wait_wmt_q);
                 osal_trigger_event(&stp_psm->wait_wmt_q);
            }
            else
            {
                if (action < STP_PSM_MAX_STATE)
                {
                    STP_PSM_ERR_FUNC("Action = %s, ACT_INACT state, the case should not happens\n\r", g_psm_action[action]);
                    STP_PSM_ERR_FUNC("state = %d, flag = %d\n", stp_psm->work_state, stp_psm->flag);
                }
                else
                {
                    STP_PSM_ERR_FUNC("Invalid Action!!\n\r");
                }
                retval = STP_PSM_OPERATION_FAIL;
            }
            break;
            //stp trigger

        case INACT_ACT:

            if (action == WAKEUP)
            {
                STP_PSM_LOUD_FUNC("Action = %s, INACT_ACT state, ready to ACT\n\r", g_psm_action[action]);
                stp_psm->flag &= ~STP_PSM_WMT_EVENT_SLEEP_EN;
                stp_psm->flag |= STP_PSM_WMT_EVENT_WAKEUP_EN;
                //wake_up(&stp_psm->wait_wmt_q);
                 osal_trigger_event(&stp_psm->wait_wmt_q);
            }
            else if (action == HOST_AWAKE)
            {
                STP_PSM_LOUD_FUNC("Action = %s, INACT_ACT state, ready to ACT\n\r", g_psm_action[action]);
                stp_psm->flag &= ~STP_PSM_WMT_EVENT_SLEEP_EN;
                stp_psm->flag |= STP_PSM_WMT_EVENT_WAKEUP_EN;
                //wake_up(&stp_psm->wait_wmt_q);
                 osal_trigger_event(&stp_psm->wait_wmt_q);
            }
            else if (action == ROLL_BACK)
            {
                STP_PSM_LOUD_FUNC("Action = %s, INACT_ACT state, back to INACT\n\r", g_psm_action[action]);
               // stp_psm->flag &= ~STP_PSM_WMT_EVENT_ROLL_BACK_EN;
                stp_psm->flag |=  STP_PSM_WMT_EVENT_ROLL_BACK_EN;
                //wake_up(&stp_psm->wait_wmt_q);
                 osal_trigger_event(&stp_psm->wait_wmt_q);
            }
            else
            {
                if (action < STP_PSM_MAX_STATE)
                {
                    STP_PSM_ERR_FUNC("Action = %s, INACT_ACT state, the case should not happens\n\r", g_psm_action[action]);
                    STP_PSM_ERR_FUNC("state = %d, flag = %d\n", stp_psm->work_state, stp_psm->flag);
                }
                else
                {
                    STP_PSM_ERR_FUNC("Invalid Action!!\n\r");
                }
                retval = STP_PSM_OPERATION_FAIL;
            }
            break;

        case INACT:

            if (action < STP_PSM_MAX_STATE)
            {
                STP_PSM_ERR_FUNC("Action = %s, INACT state, the case should not happens\n\r", g_psm_action[action]);
                STP_PSM_ERR_FUNC("state = %d, flag = %d\n", stp_psm->work_state, stp_psm->flag);
            }
            else
            {
                STP_PSM_ERR_FUNC("Invalid Action!!\n\r");
            }

            retval = -1;

            break;

        case ACT:

            if (action < STP_PSM_MAX_STATE)
            {
                STP_PSM_ERR_FUNC("Action = %s, ACT state, the case should not happens\n\r", g_psm_action[action]);
                STP_PSM_ERR_FUNC("state = %d, flag = %d\n", stp_psm->work_state, stp_psm->flag);
            }
            else
            {
                STP_PSM_ERR_FUNC("Invalid Action!!\n\r");
            }

            retval = STP_PSM_OPERATION_FAIL;

            break;

        default:

            /*invalid*/
            if (action < STP_PSM_MAX_STATE)
            {
                STP_PSM_ERR_FUNC("Action = %s, Invalid state, the case should not happens\n\r", g_psm_action[action]);
                STP_PSM_ERR_FUNC("state = %d, flag = %d\n", stp_psm->work_state, stp_psm->flag);
            }
            else
            {
                STP_PSM_ERR_FUNC("Invalid Action!!\n\r");
            }

            retval = STP_PSM_OPERATION_FAIL;

            break;
    }
    return retval;

}

static inline INT32 _stp_psm_notify_wmt(MTKSTP_PSM_T *stp_psm, const MTKSTP_PSM_ACTION_T action)
{
    INT32 ret = 0;

    if (stp_psm == NULL)
    {
        return STP_PSM_OPERATION_FAIL;
    }

    switch(_stp_psm_get_state(stp_psm))
    {
        case ACT:

            if (action == SLEEP)
            {
                _stp_psm_set_state(stp_psm, ACT_INACT);

                if (stp_psm->wmt_notify)
                {
                    stp_psm->wmt_notify(SLEEP);
                    _stp_psm_wait_wmt_event_wq(stp_psm);
                }
                else
                {
                    STP_PSM_ERR_FUNC("stp_psm->wmt_notify = NULL\n");
                    ret = STP_PSM_OPERATION_FAIL;
                }
            }
            else if (action == WAKEUP || action == HOST_AWAKE)
            {
                STP_PSM_DBG_FUNC("In ACT state, dont do WAKEUP/HOST_AWAKE again\n");
                _stp_psm_release_data(stp_psm);
            }
            else
            {
                STP_PSM_ERR_FUNC("invalid operation, the case should not happen\n");
                STP_PSM_ERR_FUNC("state = %d, flag = %d\n", stp_psm->work_state, stp_psm->flag);

                ret = STP_PSM_OPERATION_FAIL;

            }

        break;

        case INACT:

            if (action == WAKEUP)
            {
                _stp_psm_set_state(stp_psm, INACT_ACT);

                if (stp_psm->wmt_notify)
                {
                    STP_PSM_DBG_FUNC("wakeup +wake_lock(%d)\n", osal_wake_lock_count(&stp_psm->wake_lock));
                    osal_wake_lock(&stp_psm->wake_lock);
                    STP_PSM_DBG_FUNC("wakeup +wake_lock(%d)#\n", osal_wake_lock_count(&stp_psm->wake_lock));

                    STP_PSM_DBG_FUNC("mt_combo_plt_exit_deep_idle++\n");
                    mt_combo_plt_exit_deep_idle(COMBO_IF_UART);
                    STP_PSM_DBG_FUNC("mt_combo_plt_exit_deep_idle--\n");

                    stp_psm->wmt_notify(WAKEUP);
                    _stp_psm_wait_wmt_event_wq(stp_psm);
                }
                else
                {
                    STP_PSM_ERR_FUNC("stp_psm->wmt_notify = NULL\n");
                    ret = STP_PSM_OPERATION_FAIL;
                }
            }
            else if (action == HOST_AWAKE)
            {
                _stp_psm_set_state(stp_psm, INACT_ACT);

                if (stp_psm->wmt_notify)
                {
                    STP_PSM_DBG_FUNC("host awake +wake_lock(%d)\n", osal_wake_lock_count(&stp_psm->wake_lock));
                    osal_wake_lock(&stp_psm->wake_lock);
                    STP_PSM_DBG_FUNC("host awake +wake_lock(%d)#\n", osal_wake_lock_count(&stp_psm->wake_lock));

                    STP_PSM_DBG_FUNC("mt_combo_plt_exit_deep_idle++\n");
                    mt_combo_plt_exit_deep_idle(COMBO_IF_UART);
                    STP_PSM_DBG_FUNC("mt_combo_plt_exit_deep_idle--\n");

                    stp_psm->wmt_notify(HOST_AWAKE);
                    _stp_psm_wait_wmt_event_wq(stp_psm);
                }
                else
                {
                    STP_PSM_ERR_FUNC("stp_psm->wmt_notify = NULL\n");
                    ret = STP_PSM_OPERATION_FAIL;
                }
            }
            else if (action == SLEEP)
            {
                STP_PSM_INFO_FUNC("In INACT state, dont do SLEEP again\n");
            }
            else
            {
                STP_PSM_ERR_FUNC("invalid operation, the case should not happen\n");
                STP_PSM_ERR_FUNC("state = %d, flag = %d\n", stp_psm->work_state, stp_psm->flag);
                ret = STP_PSM_OPERATION_FAIL;
            }

        break;

        default:

            /*invalid*/
            STP_PSM_ERR_FUNC("invalid state, the case should not happen\n");
            STP_PSM_ERR_FUNC("state = %d, flag = %d\n", stp_psm->work_state, stp_psm->flag);
            ret = STP_PSM_OPERATION_FAIL;

            break;
    }

    return ret;
}

static inline void _stp_psm_stp_is_idle(ULONG data)
{
     MTKSTP_PSM_T *stp_psm = (MTKSTP_PSM_T *)data;

     if ((stp_psm->flag & STP_PSM_WMT_EVENT_DISABLE_MONITOR)!= 0)
     {
        STP_PSM_DBG_FUNC("STP-PSM DISABLE!\n");
        return ;
     }

     STP_PSM_INFO_FUNC("**IDLE is over %d msec, go to sleep!!!**\n\r", stp_psm->idle_time_to_sleep);
     _stp_psm_notify_wmt_sleep_wq(stp_psm);
}

static inline INT32 _stp_psm_init_monitor(MTKSTP_PSM_T *stp_psm)
{
    if (!stp_psm)
    {
        return STP_PSM_OPERATION_FAIL;
    }

    STP_PSM_INFO_FUNC("init monitor\n");

    stp_psm->psm_timer.timeoutHandler = _stp_psm_stp_is_idle;
    stp_psm->psm_timer.timeroutHandlerData = (UINT32)stp_psm;
    osal_timer_create(&stp_psm->psm_timer);

    return STP_PSM_OPERATION_SUCCESS;
}

static inline INT32 _stp_psm_deinit_monitor(MTKSTP_PSM_T *stp_psm)
{

     if (!stp_psm)
     {
        return STP_PSM_OPERATION_FAIL;
     }
     else
     {
        STP_PSM_INFO_FUNC("deinit monitor\n");

        osal_timer_stop_sync(&stp_psm->psm_timer);
        osal_timer_delete(&stp_psm->psm_timer);
     }

     return 0;
}

static inline INT32 _stp_psm_is_to_block_traffic(MTKSTP_PSM_T *stp_psm)
{
    INT32 iRet = -1;

//    osal_lock_unsleepable_lock(&stp_psm->flagSpinlock);

    if (stp_psm->flag & STP_PSM_BLOCK_DATA_EN)
    {
        iRet = 1;
    }
    else
    {
        iRet = 0;
    }
//    osal_unlock_unsleepable_lock(&stp_psm->flagSpinlock);
    return iRet;
}

static inline INT32 _stp_psm_is_disable(MTKSTP_PSM_T *stp_psm)
{
    if (stp_psm->flag & STP_PSM_WMT_EVENT_DISABLE_MONITOR)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static inline INT32  _stp_psm_do_wait(MTKSTP_PSM_T *stp_psm, MTKSTP_PSM_STATE_T state){

     #define POLL_WAIT 200
     #define POLL_WAIT_TIME 6000

     INT32 i = 0;
     INT32 limit = POLL_WAIT_TIME/POLL_WAIT;

     while(_stp_psm_get_state(stp_psm)!=state && i < limit)
     {
        osal_msleep(POLL_WAIT);
        i++;
        STP_PSM_DBG_FUNC("STP is waiting state for %s, i=%d\n", g_psm_state[state],i );
     }

     if (i == limit)
     {
        STP_PSM_WARN_FUNC("-Wait for %s takes %d msec\n", g_psm_state[state], i*POLL_WAIT);
        return STP_PSM_OPERATION_FAIL;
     }
     else
     {
        STP_PSM_DBG_FUNC("+Total waits for %s takes %d msec\n", g_psm_state[state], i*POLL_WAIT);
        return STP_PSM_OPERATION_SUCCESS;
     }
}

static inline INT32  _stp_psm_do_wakeup(MTKSTP_PSM_T *stp_psm)
{

    INT32 ret = 0;
    INT32 retry = 10;
    //INT32 i = 0;
    P_OSAL_OP_Q pOpQ = NULL;
    P_OSAL_OP   pOp = NULL;

    STP_PSM_LOUD_FUNC("*** Do Force Wakeup!***\n\r");

    //<1>If timer is active, we will stop it.
    _stp_psm_stop_monitor(stp_psm);

    osal_lock_unsleepable_lock(&(stp_psm->wq_spinlock));
    pOpQ = &stp_psm->rFreeOpQ;
    while(!RB_EMPTY(&stp_psm->rActiveOpQ))
    {
        RB_GET(&stp_psm->rActiveOpQ, pOp);
        if(pOp && !RB_FULL(&stp_psm->rFreeOpQ))
        {
            STP_PSM_DBG_FUNC("%s: opid = %d\n", __func__, pOp->op.opId);
            RB_PUT(pOpQ, pOp);
        }
        else
        {
            STP_PSM_ERR_FUNC("clear up active queue fail:0x%p,freeQ full(%d)\n",pOp,RB_FULL(&stp_psm->rFreeOpQ)? 1 : 0);
        }
    }
    osal_unlock_unsleepable_lock(&(stp_psm->wq_spinlock));

    //<5>We issue wakeup request into op queue. and wait for active.
    do{
        ret = _stp_psm_notify_wmt_wakeup_wq(stp_psm);

        if (ret == STP_PSM_OPERATION_SUCCESS)
        {
            ret = _stp_psm_do_wait(stp_psm, ACT);

            //STP_PSM_INFO_FUNC("<< wait ret = %d, num of activeQ = %d\n", ret,  RB_COUNT(&stp_psm->rActiveOpQ));
            if (ret == STP_PSM_OPERATION_SUCCESS)
            {
                break;
            }
        }
        else
        {
            STP_PSM_ERR_FUNC("_stp_psm_notify_wmt_wakeup_wq fail!!\n");
        }

        //STP_PSM_INFO_FUNC("retry = %d\n", retry);
        retry--;

        if (retry == 0)
        {
            break;
        }
    }
    while(1);

    if (retry == 0)
    {
        return STP_PSM_OPERATION_FAIL;
    }
    else
    {
        return STP_PSM_OPERATION_SUCCESS;
    }
}

static inline INT32 _stp_psm_disable(MTKSTP_PSM_T *stp_psm)
{
    INT32 ret = STP_PSM_OPERATION_FAIL;

    osal_lock_sleepable_lock(&stp_psm->user_lock);

    STP_PSM_DBG_FUNC("++\n");

    stp_psm->flag |= STP_PSM_WMT_EVENT_DISABLE_MONITOR;

    ret = _stp_psm_do_wakeup(stp_psm);
    if (ret == STP_PSM_OPERATION_SUCCESS)
    {
        STP_PSM_DBG_FUNC("--\n");
        osal_unlock_sleepable_lock(&stp_psm->user_lock);
        return STP_PSM_OPERATION_SUCCESS;
    }
    else
    {
        STP_PSM_ERR_FUNC("-- fail(%d)\n", ret);
        osal_unlock_sleepable_lock(&stp_psm->user_lock);
        return STP_PSM_OPERATION_FAIL;
    }
}

static inline INT32 _stp_psm_enable(MTKSTP_PSM_T *stp_psm, INT32 idle_time_to_sleep)
{
    INT32 ret = STP_PSM_OPERATION_FAIL;

    osal_lock_sleepable_lock(&stp_psm->user_lock);

    STP_PSM_DBG_FUNC("++\n");

    stp_psm->flag |= STP_PSM_WMT_EVENT_DISABLE_MONITOR;

    ret = _stp_psm_do_wakeup(stp_psm);
    if (ret == STP_PSM_OPERATION_SUCCESS)
    {
        stp_psm->flag &= ~STP_PSM_WMT_EVENT_DISABLE_MONITOR;
        stp_psm->idle_time_to_sleep = idle_time_to_sleep;

        if (osal_wake_lock_count(&stp_psm->wake_lock) == 0)
        {
            STP_PSM_DBG_FUNC("psm_en+wake_lock(%d)\n", osal_wake_lock_count(&stp_psm->wake_lock));
            osal_wake_lock(&stp_psm->wake_lock);
            STP_PSM_DBG_FUNC("psm_en+wake_lock(%d)#\n", osal_wake_lock_count(&stp_psm->wake_lock));
        }

        _stp_psm_start_monitor(stp_psm);

        STP_PSM_DBG_FUNC("--\n");
        osal_unlock_sleepable_lock(&stp_psm->user_lock);

        return STP_PSM_OPERATION_SUCCESS;
    }
    else
    {
        STP_PSM_ERR_FUNC("-- fail(%d)\n", ret);
        osal_unlock_sleepable_lock(&stp_psm->user_lock);
        return STP_PSM_OPERATION_FAIL;
    }
}

/*external function for WMT module to do sleep/wakeup*/
INT32  stp_psm_do_wakeup(MTKSTP_PSM_T *stp_psm)
{
    return _stp_psm_do_wakeup(stp_psm);
}

INT32 stp_psm_notify_stp(MTKSTP_PSM_T *stp_psm, const MTKSTP_PSM_ACTION_T action)
{

    return  _stp_psm_notify_stp(stp_psm, action);
}

INT32 stp_psm_notify_wmt_wakeup(MTKSTP_PSM_T *stp_psm)
{
    return _stp_psm_notify_wmt_wakeup_wq(stp_psm);
}

INT32 stp_psm_start_monitor(MTKSTP_PSM_T *stp_psm)
{
    return _stp_psm_start_monitor(stp_psm);
}

INT32 stp_psm_is_to_block_traffic(MTKSTP_PSM_T *stp_psm)
{
    return _stp_psm_is_to_block_traffic(stp_psm);
}

INT32 stp_psm_is_disable(MTKSTP_PSM_T *stp_psm)
{
    return _stp_psm_is_disable(stp_psm);
}

INT32 stp_psm_has_pending_data(MTKSTP_PSM_T *stp_psm)
{
    return _stp_psm_has_pending_data(stp_psm);
}

INT32 stp_psm_release_data(MTKSTP_PSM_T *stp_psm)
{
    return _stp_psm_release_data(stp_psm);
}

INT32
stp_psm_hold_data (
    MTKSTP_PSM_T *stp_psm,
    const UINT8 *buffer,
    const UINT32 len,
    const UINT8 type
    )
{
    return _stp_psm_hold_data(stp_psm, buffer, len, type);
}

INT32 stp_psm_disable(MTKSTP_PSM_T *stp_psm)
{
    return _stp_psm_disable(stp_psm);
}

INT32 stp_psm_enable(MTKSTP_PSM_T *stp_psm, INT32 idle_time_to_sleep)
{
    return _stp_psm_enable(stp_psm, idle_time_to_sleep);
}

INT32 stp_psm_reset(MTKSTP_PSM_T *stp_psm)
{
    return _stp_psm_reset(stp_psm);
}

MTKSTP_PSM_T *stp_psm_init(void)
{
    INT32 err = 0;
    INT32 i = 0;
    INT32 ret = -1;

    STP_PSM_INFO_FUNC("psm init (0x%p, %d)\n", stp_psm, sizeof(MTKSTP_PSM_T));
    osal_memset(stp_psm, 0x0, sizeof(MTKSTP_PSM_T));

    stp_psm->work_state = ACT;
    stp_psm->wmt_notify = wmt_lib_ps_stp_cb;
    stp_psm->idle_time_to_sleep = STP_PSM_IDLE_TIME_SLEEP;
    stp_psm->flag = 0;
    stp_psm->stp_tx_cb = NULL;

    ret = osal_fifo_init(&stp_psm->hold_fifo, NULL, STP_PSM_FIFO_SIZE);
    if (ret < 0)
    {
        STP_PSM_ERR_FUNC("FIFO INIT FAILS\n");
        goto ERR_EXIT4;
    }

    osal_fifo_reset(&stp_psm->hold_fifo);
    osal_sleepable_lock_init(&stp_psm->user_lock);

    //osal_unsleepable_lock_init(&stp_psm->hold_fifo_spinlock_global);
    psm_fifo_lock_init(stp_psm);

    osal_unsleepable_lock_init(&stp_psm->wq_spinlock);
//    osal_unsleepable_lock_init(&stp_psm->flagSpinlock);

    osal_memcpy(stp_psm->wake_lock.name, "MT6620", 6);
    osal_wake_lock_init(&stp_psm->wake_lock);

    osal_event_init(&stp_psm->PSMd_event);
    RB_INIT(&stp_psm->rFreeOpQ, STP_OP_BUF_SIZE);
    RB_INIT(&stp_psm->rActiveOpQ, STP_OP_BUF_SIZE);
    /* Put all to free Q */
    for (i = 0; i < STP_OP_BUF_SIZE; i++)
    {
         osal_signal_init(&(stp_psm->arQue[i].signal));
         _stp_psm_put_op(stp_psm, &stp_psm->rFreeOpQ, &(stp_psm->arQue[i]));
    }

    /*Generate BTM thread, to servie STP-CORE and WMT-CORE for sleeping, waking up and host awake*/
    stp_psm->PSMd.pThreadData = (VOID *)stp_psm;
    stp_psm->PSMd.pThreadFunc = (VOID *)_stp_psm_proc;
    osal_memcpy(stp_psm->PSMd.threadName, PSM_THREAD_NAME, osal_strlen(PSM_THREAD_NAME));

    ret = osal_thread_create(&stp_psm->PSMd);
    if (ret < 0)
    {
        STP_PSM_ERR_FUNC("osal_thread_create fail...\n");
        goto ERR_EXIT5;
    }

    //init_waitqueue_head(&stp_psm->wait_wmt_q);
    stp_psm->wait_wmt_q.timeoutValue = STP_PSM_WAIT_EVENT_TIMEOUT;
    osal_event_init(&stp_psm->wait_wmt_q);

    err = _stp_psm_init_monitor(stp_psm);
    if (err)
    {
        STP_PSM_ERR_FUNC("__stp_psm_init ERROR\n");
        goto ERR_EXIT6;
    }

    //Start STPd thread
    ret = osal_thread_run(&stp_psm->PSMd);
    if (ret < 0)
    {
        STP_PSM_ERR_FUNC("osal_thread_run FAILS\n");
        goto ERR_EXIT6;
    }

    //psm disable in default
    _stp_psm_disable(stp_psm);

    return stp_psm;

ERR_EXIT6:

    ret = osal_thread_destroy(&stp_psm->PSMd);
    if (ret < 0)
    {
        STP_PSM_ERR_FUNC("osal_thread_destroy FAILS\n");
        goto ERR_EXIT5;
    }
ERR_EXIT5:
    osal_fifo_deinit(&stp_psm->hold_fifo);
ERR_EXIT4:

    return NULL;
}

INT32 stp_psm_deinit(MTKSTP_PSM_T *stp_psm)
{
    INT32 ret = -1;
    INT32 i = 0;

    STP_PSM_INFO_FUNC("psm deinit\n");

    if (!stp_psm)
    {
        return STP_PSM_OPERATION_FAIL;
    }

    ret = osal_thread_destroy(&stp_psm->PSMd);
    if (ret < 0)
    {
        STP_PSM_ERR_FUNC("osal_thread_destroy FAILS\n");
    }

    ret = _stp_psm_deinit_monitor(stp_psm);
    if (ret < 0)
    {
        STP_PSM_ERR_FUNC("_stp_psm_deinit_monitor ERROR\n");
    }
    for (i = 0; i < STP_OP_BUF_SIZE; i++)
    {
         osal_signal_deinit(&(stp_psm->arQue[i].signal));
    }
    osal_event_deinit(&stp_psm->wait_wmt_q);
    osal_event_deinit(&stp_psm->PSMd_event);
    osal_wake_lock_deinit(&stp_psm->wake_lock);
    osal_fifo_deinit(&stp_psm->hold_fifo);
    osal_sleepable_lock_deinit(&stp_psm->user_lock);

    //osal_unsleepable_lock_deinit(&stp_psm->hold_fifo_spinlock_global);
    psm_fifo_lock_deinit(stp_psm);

    osal_unsleepable_lock_deinit(&stp_psm->wq_spinlock);
//    osal_unsleepable_lock_deinit(&stp_psm->flagSpinlock);

    return STP_PSM_OPERATION_SUCCESS;
}


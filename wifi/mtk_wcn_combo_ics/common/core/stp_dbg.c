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


#include "stp_dbg.h"
#include "btm_core.h"

#define PFX_STP_DBG                      "[STPDbg]"
#define STP_DBG_LOG_LOUD                 4
#define STP_DBG_LOG_DBG                  3
#define STP_DBG_LOG_INFO                 2
#define STP_DBG_LOG_WARN                 1
#define STP_DBG_LOG_ERR                  0

UINT32 gStpDbgDbgLevel = STP_DBG_LOG_INFO;

#define STP_DBG_LOUD_FUNC(fmt, arg...)   if (gStpDbgDbgLevel >= STP_DBG_LOG_LOUD){  osal_loud_print(PFX_STP_DBG "%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_DBG_DBG_FUNC(fmt, arg...)    if (gStpDbgDbgLevel >= STP_DBG_LOG_DBG){  osal_dbg_print(PFX_STP_DBG "%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_DBG_INFO_FUNC(fmt, arg...)   if (gStpDbgDbgLevel >= STP_DBG_LOG_INFO){ osal_info_print(PFX_STP_DBG "%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_DBG_WARN_FUNC(fmt, arg...)   if (gStpDbgDbgLevel >= STP_DBG_LOG_WARN){ osal_warn_print(PFX_STP_DBG "%s: "  fmt, __FUNCTION__ ,##arg);}
#define STP_DBG_ERR_FUNC(fmt, arg...)    if (gStpDbgDbgLevel >= STP_DBG_LOG_ERR){  osal_err_print(PFX_STP_DBG "%s: "   fmt, __FUNCTION__ ,##arg);}
#define STP_DBG_TRC_FUNC(f)              if (gStpDbgDbgLevel >= STP_DBG_LOG_DBG){  osal_dbg_print(PFX_STP_DBG "<%s> <%d>\n", __FUNCTION__, __LINE__);}

static VOID stp_dbg_dump_data (UINT8 *pBuf, CHAR * title,  INT32 len)
{
    INT32 k = 0;

    STP_DBG_INFO_FUNC(" %s-len:%d\n", title, len);
    for(k=0; k < len ; k++){
        if (k%16 == 0)  STP_DBG_INFO_FUNC("\n");
        STP_DBG_INFO_FUNC("0x%02x ",  pBuf[k]);
    }
    STP_DBG_INFO_FUNC("--end\n");
}

static INT32 _stp_dbg_enable (MTKSTP_DBG_T *stp_dbg)
{
    osal_lock_unsleepable_lock(&(stp_dbg->logsys->lock));
    stp_dbg->pkt_trace_no = 0;
    stp_dbg->is_enable = 1;
    osal_unlock_unsleepable_lock(&(stp_dbg->logsys->lock));

    return 0;
}

static INT32 _stp_dbg_disable (MTKSTP_DBG_T *stp_dbg)
{
    osal_lock_unsleepable_lock(&(stp_dbg->logsys->lock));
    stp_dbg->pkt_trace_no = 0;

    /* [FIXME][George] DANGEROUS CODING to MEMSET A STRUCTURE in a NON-INIT
     * or a NON-DEINIT CONTEXT!!!
     */
#if 0
    osal_memset(stp_dbg->logsys,0,osal_sizeof(MTKSTP_LOG_SYS_T));
#else
    osal_memset(&stp_dbg->logsys->queue[0], 0x0, sizeof(stp_dbg->logsys->queue));
    stp_dbg->logsys->size = 0;
    stp_dbg->logsys->in = 0;
    stp_dbg->logsys->out = 0;
#endif
    stp_dbg->is_enable = 0;
    osal_unlock_unsleepable_lock(&(stp_dbg->logsys->lock));

    return 0;
}

static INT32 _stp_dbg_dmp_in (MTKSTP_DBG_T *stp_dbg, CHAR *buf, INT32 len)
{
    UINT32 internalFlag = stp_dbg->logsys->size < STP_DBG_LOG_ENTRY_NUM;
#ifdef CONFIG_LOG_STP_INTERNAL
    internalFlag = 1;
#endif

    osal_lock_unsleepable_lock(&(stp_dbg->logsys->lock));

    if (internalFlag){
        stp_dbg->logsys->queue[stp_dbg->logsys->in].id = 0;
        stp_dbg->logsys->queue[stp_dbg->logsys->in].len = len;

        osal_memcpy(&(stp_dbg->logsys->queue[stp_dbg->logsys->in].buffer[0]),buf, ((len >= STP_DBG_LOG_ENTRY_SZ)? (STP_DBG_LOG_ENTRY_SZ):(len)));
            stp_dbg->logsys->size++;
            stp_dbg->logsys->size = (stp_dbg->logsys->size > STP_DBG_LOG_ENTRY_NUM) ? STP_DBG_LOG_ENTRY_NUM : stp_dbg->logsys->size;
            stp_dbg->logsys->in = (stp_dbg->logsys->in >= (STP_DBG_LOG_ENTRY_NUM - 1))?(0):(stp_dbg->logsys->in + 1);
            STP_DBG_DBG_FUNC("logsys size = %d, in = %d\n", stp_dbg->logsys->size, stp_dbg->logsys->in);
    } else {
        STP_DBG_WARN_FUNC("logsys FULL!\n");
    }

    osal_unlock_unsleepable_lock(&(stp_dbg->logsys->lock));

    return 0;
}

INT32 stp_gdb_notify_btm_dmp_wq(MTKSTP_DBG_T *stp_dbg, CHAR *buf, INT32 len)
{
    INT32 retval = 0;

    retval = _stp_dbg_dmp_in(stp_dbg, buf, len);

#ifndef CONFIG_LOG_STP_INTERNAL
    if (stp_dbg->btm != NULL){
        retval += stp_btm_notify_wmt_dmp_wq((MTKSTP_BTM_T *)stp_dbg->btm);
    }
#endif

    return retval;
}

INT32 stp_dbg_dmp_in (MTKSTP_DBG_T *stp_dbg, CHAR *buf, INT32 len)
{
    return _stp_dbg_dmp_in(stp_dbg, buf, len);
}


INT32 stp_dbg_dmp_printk (MTKSTP_DBG_T *stp_dbg)
{
    CHAR *pBuf = NULL;
    INT32 len = 0;
    STP_DBG_HDR_T *pHdr = NULL;

    osal_lock_unsleepable_lock(&(stp_dbg->logsys->lock));

    if (STP_DBG_LOG_ENTRY_NUM == stp_dbg->logsys->size)
    {
        stp_dbg->logsys->out = stp_dbg->logsys->in;
    }
    else
    {
        stp_dbg->logsys->out = ((stp_dbg->logsys->in + STP_DBG_LOG_ENTRY_NUM) - stp_dbg->logsys->size) % STP_DBG_LOG_ENTRY_NUM;
    }
    STP_DBG_INFO_FUNC("loged packet size = %d, in(%d), out(%d)\n", stp_dbg->logsys->size, stp_dbg->logsys->in, stp_dbg->logsys->out);
    while(stp_dbg->logsys->size > 0){
        pHdr = (STP_DBG_HDR_T *)&(stp_dbg->logsys->queue[stp_dbg->logsys->out].buffer[0]);
        STP_DBG_INFO_FUNC("%d.%ds, %s:pT%sn(%d)l(%d)s(%d)a(%d)\n", \
            pHdr->sec,
            pHdr->usec,
            pHdr->dir == PKT_DIR_TX ? "Tx" : "Rx",
            gStpDbgType[pHdr->type],
            pHdr->no,
            pHdr->len,
            pHdr->seq,
            pHdr->ack
        );

        pBuf = &(stp_dbg->logsys->queue[stp_dbg->logsys->out].buffer[0]) + sizeof (STP_DBG_HDR_T);
        len = stp_dbg->logsys->queue[stp_dbg->logsys->out].len - sizeof (STP_DBG_HDR_T);
        if (0 < len){
            stp_dbg_dump_data(pBuf, pHdr->dir == PKT_DIR_TX ? "Tx" : "Rx", len);
        }
        stp_dbg->logsys->out = (stp_dbg->logsys->out >= (STP_DBG_LOG_ENTRY_NUM - 1))?(0):(stp_dbg->logsys->out + 1);
        stp_dbg->logsys->size--;

    }

     osal_unlock_unsleepable_lock(&(stp_dbg->logsys->lock));

    return 0;
}


INT32 stp_dbg_dmp_out (MTKSTP_DBG_T *stp_dbg, CHAR *buf, INT32 *len)
{
    INT32 remaining = 0;

    osal_lock_unsleepable_lock(&(stp_dbg->logsys->lock));
    if (stp_dbg->logsys->size > 0){
        osal_memcmp(buf, &(stp_dbg->logsys->queue[stp_dbg->logsys->out].buffer[0]),
             stp_dbg->logsys->queue[stp_dbg->logsys->out].len);

        (*len) = stp_dbg->logsys->queue[stp_dbg->logsys->out].len;
        stp_dbg->logsys->out = (stp_dbg->logsys->out >= (STP_DBG_LOG_ENTRY_NUM - 1))?(0):(stp_dbg->logsys->out + 1);
        stp_dbg->logsys->size--;

        STP_DBG_DBG_FUNC("logsys size = %d, out = %d\n", stp_dbg->logsys->size, stp_dbg->logsys->out);
    } else {
        STP_DBG_LOUD_FUNC("logsys EMPTY!\n");
    }

    remaining = (stp_dbg->logsys->size == 0)?(0):(1);

    osal_unlock_unsleepable_lock(&(stp_dbg->logsys->lock));

    return remaining;
}

static INT32 stp_dbg_fill_hdr (struct stp_dbg_pkt_hdr *hdr, INT32 type, INT32 ack, INT32 seq, INT32 crc, INT32 dir, INT32 len, INT32 dbg_type)
{
    INT32 sec = 0;
    INT32 usec = 0;

    if (!hdr){
        STP_DBG_ERR_FUNC("function invalid\n");
        return -22;
    } else {
        osal_gettimeofday(&sec,&usec);
        hdr->dbg_type = dbg_type;
        hdr->ack = ack;
        hdr->seq = seq;
        hdr->sec = sec;
        hdr->usec = usec;
        hdr->crc  = crc;
        hdr->dir  = dir;//rx
        hdr->dmy  = 0xffffffff;
        hdr->len  =  len;
        hdr->type = type;

        return 0;
    }
}

static INT32
stp_dbg_add_pkt (
    MTKSTP_DBG_T*stp_dbg,
    struct stp_dbg_pkt_hdr *hdr,
    const UINT8 *body
    )
{
    struct stp_dbg_pkt stp_pkt;
    UINT32 hdr_sz = osal_sizeof(struct stp_dbg_pkt_hdr);
    UINT32 body_sz = 0;

    osal_assert(stp_dbg);

    if (hdr->dbg_type == STP_DBG_PKT){
        body_sz = (hdr->len <= STP_PKT_SZ)?(hdr->len):(STP_PKT_SZ);
    }else{
        body_sz = (hdr->len <= STP_DMP_SZ)?(hdr->len):(STP_DMP_SZ);
    }

    hdr->no = stp_dbg->pkt_trace_no++;
    osal_memcpy((UINT8 *)&stp_pkt.hdr, (UINT8 *)hdr, hdr_sz);
    if (body != NULL){
        osal_memcpy((UINT8 *)&stp_pkt.raw[0], body, body_sz);
    }
    stp_gdb_notify_btm_dmp_wq(stp_dbg,(CHAR *) &stp_pkt, hdr_sz + body_sz);

    return 0;
}

INT32 stp_dbg_log_pkt(MTKSTP_DBG_T *stp_dbg, INT32 dbg_type,
    INT32 type, INT32 ack_no, INT32 seq_no, INT32 crc, INT32 dir, INT32 len, const UINT8 *body)
{
    struct stp_dbg_pkt_hdr hdr;

    if (stp_dbg->is_enable == 0) {
        /*dbg is disable,and not to log*/
    }
    else {
        stp_dbg_fill_hdr(&hdr,
            (INT32) type,
            (INT32) ack_no,
            (INT32) seq_no,
            (INT32) crc,
            (INT32) dir,
            (INT32) len,
            (INT32) dbg_type);

        stp_dbg_add_pkt(stp_dbg, &hdr, body);
    }

    return 0;
}

INT32 stp_dbg_enable(MTKSTP_DBG_T *stp_dbg)
{
    return _stp_dbg_enable(stp_dbg);
}

INT32 stp_dbg_disable(MTKSTP_DBG_T *stp_dbg)
{
    return _stp_dbg_disable(stp_dbg);
}

MTKSTP_DBG_T *stp_dbg_init(VOID *btm_half)
{
    MTKSTP_DBG_T *stp_dbg= NULL;
    STP_DBG_INFO_FUNC("stp-dbg init\n");

    stp_dbg = osal_kzalloc_sleep(osal_sizeof(MTKSTP_DBG_T));
    if (!stp_dbg){
        STP_DBG_ERR_FUNC("-ENOMEM\n");
        goto ERR_EXIT1;
    }

    stp_dbg->logsys = osal_malloc(osal_sizeof(MTKSTP_LOG_SYS_T));
    if (!stp_dbg->logsys){
        STP_DBG_ERR_FUNC("-ENOMEM stp_gdb->logsys\n");
        goto ERR_EXIT2;
    }
    osal_memset(stp_dbg->logsys, 0, osal_sizeof(MTKSTP_LOG_SYS_T));

    osal_unsleepable_lock_init(&(stp_dbg->logsys->lock));
    stp_dbg->pkt_trace_no = 0;
    stp_dbg->is_enable = 0;

    if (btm_half != NULL){
       stp_dbg->btm = btm_half;
    } else {
       stp_dbg->btm = NULL;
    }

    return stp_dbg;

ERR_EXIT2:
    osal_kfree(stp_dbg);
    return NULL;

ERR_EXIT1:
    return NULL;
}

INT32 stp_dbg_deinit(MTKSTP_DBG_T *stp_dbg)
{
    STP_DBG_INFO_FUNC("stp-dbg deinit\n");

    if (&(stp_dbg->logsys->lock)){
    osal_unsleepable_lock_deinit(&(stp_dbg->logsys->lock));
    }
    if (stp_dbg->logsys){
        osal_vfree(stp_dbg->logsys);
    stp_dbg->logsys = NULL;
    }

    if (stp_dbg){
        osal_kfree(stp_dbg);
    stp_dbg = NULL;
    }

    return 0;
}


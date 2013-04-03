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


#ifndef _STP_DEBUG_H_
#define _STP_DEBUG_H_

#include "osal.h"

#define CONFIG_LOG_STP_INTERNAL

#ifndef CONFIG_LOG_STP_INTERNAL
#define STP_PKT_SZ  16
#define STP_DMP_SZ 128
#define STP_PKT_NO 128

#define STP_DBG_LOG_ENTRY_NUM 2048
#define STP_DBG_LOG_ENTRY_SZ 128

#else
#define STP_PKT_SZ  16
#define STP_DMP_SZ 16
#define STP_PKT_NO 16

#define STP_DBG_LOG_ENTRY_NUM 28
#define STP_DBG_LOG_ENTRY_SZ 64


#endif


typedef enum {
    STP_DBG_EN         = 0,
    STP_DBG_PKT        = 1,
    STP_DBG_DR         = 2,
    STP_DBG_FW_ASSERT  = 3,
    STP_DBG_FW_LOG = 4,
    STP_DBG_MAX
}STP_DBG_OP_T;

typedef enum {
    STP_DBG_PKT_FIL_ALL = 0,
    STP_DBG_PKT_FIL_BT  = 1,
    STP_DBG_PKT_FIL_GPS = 2,
    STP_DBG_PKT_FIL_FM  = 3,
    STP_DBG_PKT_FIL_WMT = 4,
    STP_DBG_PKT_FIL_MAX
} STP_DBG_PKT_FIL_T;

static  CHAR * const gStpDbgType[]={
    "< BT>",
    "< FM>",
    "<GPS>",
    "<WiFi>",
    "<WMT>",
    "<STP>",
    "<DBG>",
    "<UNKOWN>"
};


typedef enum {
    STP_DBG_DR_MAX = 0,
} STP_DBG_DR_FIL_T;

typedef enum {
    STP_DBG_FW_MAX = 0,
} STP_DBG_FW_FIL_T;

typedef enum {
    PKT_DIR_RX = 0,
    PKT_DIR_TX
} STP_DBG_PKT_DIR_T;

/*simple log system ++*/

typedef struct {
    INT32  id; /*type: 0. pkt trace 1. fw info 2. assert info 3. trace32 dump . -1. linked to the the previous*/
    INT32  len;
    UINT8 buffer[STP_DBG_LOG_ENTRY_SZ];
} MTKSTP_LOG_ENTRY_T;

typedef struct log_sys {
    MTKSTP_LOG_ENTRY_T  queue[STP_DBG_LOG_ENTRY_NUM];
    UINT32 size;
    UINT32 in;
    UINT32 out;
    OSAL_UNSLEEPABLE_LOCK lock;
} MTKSTP_LOG_SYS_T;
/*--*/

typedef struct stp_dbg_pkt_hdr{
    //packet information

    UINT32   sec;
    UINT32   usec;
    UINT32   dbg_type;
    UINT32   dmy;
    UINT32   no;
    UINT32   dir;


    //packet content
    UINT32  type;
    UINT32  len;
    UINT32  ack;
    UINT32  seq;
    UINT32  chs;
    UINT32  crc;
}STP_DBG_HDR_T;

typedef struct stp_dbg_pkt{
    struct stp_dbg_pkt_hdr hdr;
    UINT8 raw[STP_DMP_SZ];
}STP_PACKET_T;

typedef struct mtkstp_dbg_t{
    /*log_sys*/
    INT32 pkt_trace_no;
    VOID *btm;
    INT32 is_enable;
    MTKSTP_LOG_SYS_T *logsys;
}MTKSTP_DBG_T;

extern INT32 stp_dbg_enable(MTKSTP_DBG_T *stp_dbg);
extern INT32 stp_dbg_disable(MTKSTP_DBG_T *stp_dbg);
extern MTKSTP_DBG_T *stp_dbg_init(VOID *);
extern INT32 stp_dbg_deinit(MTKSTP_DBG_T *stp_dbg);
extern INT32 stp_dbg_dmp_out(MTKSTP_DBG_T *stp_dbg, CHAR *buf, INT32 *len);
extern INT32 stp_dbg_dmp_printk(MTKSTP_DBG_T *stp_dbg);

extern INT32
stp_dbg_log_pkt (
    MTKSTP_DBG_T *stp_dbg,
    INT32 dbg_type,
    INT32 type,
    INT32 ack_no,
    INT32 seq_no,
    INT32 crc,
    INT32 dir,
    INT32 len,
    const UINT8 *body);
#endif /* end of _STP_DEBUG_H_ */


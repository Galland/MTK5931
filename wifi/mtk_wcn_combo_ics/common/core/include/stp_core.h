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

#ifndef _STP_CORE_H
#define _STP_CORE_H

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

#include "osal.h"
#include "stp_exp.h"
#include "psm_core.h"
#include "btm_core.h"
#include "core_exp.h"
/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/
#define CFG_STP_CORE_PS_SUPPORT (1)
/* configure enabling power saving support in STP-CORE */
#define CFG_STP_CORE_CTX_SPIN_LOCK (0)
/* configure using SPINLOCK or just semophore(mutex) for STP-CORE */
#define CFG_STP_CORE_FRB_SPIN_LOCK (0)
/* configure using SPINLOCK or just semophore(mutex) for STP-CORE ring buffer of each function */

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
#define PFX "[STP-C]"
#define STP_LOG_DBG (4)
#define STP_LOG_PKHEAD (3)
#define STP_LOG_INFO (2)
#define STP_LOG_WARN (1)
#define STP_LOG_ERR (0)

#define MTKSTP_UART_FULL_MODE (0x01)
#define MTKSTP_UART_MAND_MODE (0x02)
#define MTKSTP_SDIO_MODE (0x04)

#define MTKSTP_BUFFER_SIZE (16384)

/*To check function driver's status by the the interface*/
/*Operation definition*/
#if 0/*move to core_exp.h*/
#define OP_FUNCTION_ACTIVE (0)

/*Driver's status*/
#define STATUS_OP_INVALID (0)
#define STATUS_FUNCTION_INVALID (1)

#define STATUS_FUNCTION_ACTIVE (31)
#define STATUS_FUNCTION_INACTIVE (32)
#endif 

#define MTKSTP_CRC_SIZE     (2)
#define MTKSTP_HEADER_SIZE  (4)
#define MTKSTP_SEQ_SIZE     (8)

/*#define MTKSTP_WINSIZE      (4)*/
#define MTKSTP_WINSIZE      (7)
#define MTKSTP_TX_TIMEOUT   (180) /*TODO: Baudrate to decide this*/
//#define MTKSTP_TX_TIMEOUT (360) /*TODO: Baudrate to decide this*/
#define MTKSTP_RETRY_LIMIT  (10)

#define INDEX_INC(idx)  \
{                       \
    idx++;              \
    idx &= 0x7;         \
}

#define INDEX_DEC(idx)  \
{                       \
    idx--;              \
    idx &= 0x7;         \
}

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
#if 0
typedef INT32 (*IF_TX)(const UINT8 *data, const UINT32 size, UINT32 *written_size);
/* event/signal */
typedef INT32 (*EVENT_SET)(UINT8 function_type);
typedef INT32 (*EVENT_TX_RESUME)(UINT8 winspace);
typedef INT32 (*FUNCTION_STATUS)(UINT8 type, UINT8 op);
#endif 
typedef INT32   (*WMT_NOTIFY_FUNC_T)(UINT32 action);
typedef INT32   (*BTM_NOTIFY_WMT_FUNC_T)(INT32);

#if CFG_STP_CORE_CTX_SPIN_LOCK
typedef OSAL_UNSLEEPABLE_LOCK STP_CTX_LOCK, *PSTP_CTX_LOCK;
#else
typedef OSAL_SLEEPABLE_LOCK STP_CTX_LOCK, *PSTP_CTX_LOCK;
#endif

#if CFG_STP_CORE_FRB_SPIN_LOCK
typedef OSAL_UNSLEEPABLE_LOCK STP_FRB_LOCK, *PSTP_FRB_LOCK;
#else
typedef OSAL_SLEEPABLE_LOCK STP_FRB_LOCK, *PSTP_FRB_LOCK;
#endif

#if 0
typedef struct
{
    /* common interface */
    IF_TX           cb_if_tx;
    /* event/signal */
    EVENT_SET       cb_event_set;
    EVENT_TX_RESUME cb_event_tx_resume;
    FUNCTION_STATUS cb_check_funciton_status;
}mtkstp_callback;
#endif

typedef enum
{
    MTKSTP_SYNC = 0,
    MTKSTP_SEQ,
    MTKSTP_ACK,
    MTKSTP_NAK,
    MTKSTP_TYPE,
    MTKSTP_LENGTH,
    MTKSTP_CHECKSUM,
    MTKSTP_DATA,
    MTKSTP_CRC1,
    MTKSTP_CRC2,
    MTKSTP_RESYNC1,
    MTKSTP_RESYNC2,
    MTKSTP_RESYNC3,
    MTKSTP_RESYNC4,
    MTKSTP_FW_MSG,
} mtkstp_parser_state;

typedef struct
{
    mtkstp_parser_state  state;
    UINT8            seq;
    UINT8            ack;
    UINT8            nak;
    UINT8            type;
    UINT16           length;
    UINT8            checksum;
    UINT16           crc;
} mtkstp_parser_context_struct;

typedef struct
{
    UINT8           txseq;  // last tx pkt's seq + 1
    UINT8           txack;  // last tx pkt's ack
    UINT8           rxack;  // last rx pkt's ack
    UINT8           winspace;   // current sliding window size
    UINT8           expected_rxseq;  // last rx pkt's seq + 1
    UINT8           retry_times;
} mtkstp_sequence_context_struct;

typedef struct
{
    STP_FRB_LOCK mtx;
    UINT8           buffer[MTKSTP_BUFFER_SIZE];
    UINT32          read_p;
    UINT32          write_p;
} mtkstp_ring_buffer_struct;

typedef struct
{
    UINT8  inband_rst_set;
    UINT32 rx_counter;  // size of current processing pkt in rx_buf[]
    UINT8  rx_buf[MTKSTP_BUFFER_SIZE];  // input buffer of STP, room for current processing pkt
    UINT32 tx_read;     // read ptr of tx_buf[]
    UINT32 tx_write;    // write ptr of tx_buf[]
    UINT8  tx_buf[MTKSTP_BUFFER_SIZE];  // output buffer of STP
    UINT32 tx_start_addr[MTKSTP_SEQ_SIZE];  // ptr of each pkt in tx_buf[]
    UINT32 tx_length[MTKSTP_SEQ_SIZE];      // length of each pkt in tx_buf[]
    mtkstp_ring_buffer_struct ring[MTKSTP_MAX_TASK_NUM];    // ring buffers for each function driver
    mtkstp_parser_context_struct parser;        // current rx pkt's content
    mtkstp_sequence_context_struct sequence;    // state machine's current status
    STP_CTX_LOCK stp_mutex;
    OSAL_TIMER tx_timer;// timer for tx timeout handling

    MTKSTP_PSM_T *psm;
    MTKSTP_BTM_T *btm;
    UINT8 f_enable; /* default disabled */
    UINT8 f_ready; /* default non-ready */
    UINT8 f_pending_type;

    /* Flag to identify Blueztooth is Bluez/or MTK Stack*/
    MTK_WCN_BOOL f_bluez;
    MTK_WCN_BOOL f_dbg_en;
    MTK_WCN_BOOL f_autorst_en;

    /* Flag to identify STP by SDIO or UART */
    UINT32 f_mode;

}mtkstp_context_struct;

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/


/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

INT32 stp_send_data_no_ps(UINT8 *buffer, UINT32 length, UINT8 type);


/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_enable
* DESCRIPTION
*  enable/disable STP
* PARAMETERS
*  value        [IN]        0 = disable, others = enable
* RETURNS
*  INT32    0 = success, others = error
*****************************************************************************/
extern INT32 mtk_wcn_stp_enable(INT32 value);

/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_ready
* DESCRIPTION
*  ready/non-ready STP
* PARAMETERS
*  value        [IN]        0 = non-ready, others = ready
* RETURNS
*  INT32    0 = success, others = error
*****************************************************************************/
extern INT32 mtk_wcn_stp_ready(INT32 value);

/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_set_sdio_mode
* DESCRIPTION
*  Set stp for SDIO mode
* PARAMETERS
*  sdio_flag  [IN]        sdio mode flag (TRUE:SDIO mode, FALSE:UART mode)
* RETURNS
*  void
*****************************************************************************/
extern VOID mtk_wcn_stp_set_mode(UINT32 sdio_flag);

/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_is_uart_fullset_mode
* DESCRIPTION
*  Is stp use UART Fullset  mode?
* PARAMETERS
*  none.
* RETURNS
*  MTK_WCN_BOOL    TRUE:UART Fullset, FALSE:UART Fullset
*****************************************************************************/
extern MTK_WCN_BOOL mtk_wcn_stp_is_uart_fullset_mode(VOID);

/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_is_uart_mand_mode
* DESCRIPTION
*  Is stp use UART Mandatory  mode?
* PARAMETERS
*  none.
* RETURNS
*  MTK_WCN_BOOL    TRUE:UART Mandatory, FALSE:UART Mandatory
*****************************************************************************/
extern MTK_WCN_BOOL mtk_wcn_stp_is_uart_mand_mode(VOID);


/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_is_sdio_mode
* DESCRIPTION
*  Is stp use SDIO mode?
* PARAMETERS
*  none.
* RETURNS
*  MTK_WCN_BOOL    TRUE:SDIO mode, FALSE:UART mode
*****************************************************************************/
extern MTK_WCN_BOOL mtk_wcn_stp_is_sdio_mode(VOID);


/*****************************************************************************
* FUNCTION
*  stp_send_inband_reset
* DESCRIPTION
*  To sync to oringnal stp state with f/w stp
* PARAMETERS
*  none.
* RETURNS
*  none
*****************************************************************************/
extern void mtk_wcn_stp_inband_reset(VOID);

/*****************************************************************************
* FUNCTION
*  stp_send_inband_reset
* DESCRIPTION
*  To send testing command to chip
* PARAMETERS
*  none.
* RETURNS
*  none
*****************************************************************************/
extern VOID mtk_wcn_stp_test_cmd(INT32 no);

/*****************************************************************************
* FUNCTION
*  stp_send_inband_reset
* DESCRIPTION
* To control STP debugging mechanism
* PARAMETERS
*  func_no: function control, func_op: dumpping filer, func_param: dumpping parameter
* RETURNS
*  none
*****************************************************************************/
extern VOID mtk_wcn_stp_debug_ctrl(INT32 func_no, INT32 func_op, INT32 func_param);
/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_flush
* DESCRIPTION
*  flush all stp context
* PARAMETERS
*  none.
* RETURNS
*  none
*****************************************************************************/
extern VOID mtk_wcn_stp_flush_context(VOID);

/*****************************************************************************
* FUNCTION
*  set stp debugging mdoe
* DESCRIPTION
*  set stp debugging mdoe
* PARAMETERS
* dbg_mode: switch to dbg mode ?
* RETURNS
*  void
*****************************************************************************/
extern VOID mtk_wcn_stp_set_dbg_mode(MTK_WCN_BOOL dbg_mode);

/*****************************************************************************
* FUNCTION
*  set stp auto reset mdoe
* DESCRIPTION
*  set stp auto reset mdoe
* PARAMETERS
* auto_rst: switch to auto reset mode ?
* RETURNS
*  void
*****************************************************************************/
extern VOID mtk_wcn_stp_set_auto_rst(MTK_WCN_BOOL auto_rst);

/*stp_psm support*/

/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_psm_notify_stp
* DESCRIPTION
*  WMT notification to STP that power saving job is done or not
* PARAMETERS
*
* RETURNS
*  0: Sccuess  Negative value: Fail
*****************************************************************************/
extern INT32 mtk_wcn_stp_psm_notify_stp(const UINT32 action);


/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_psm_enabla
* DESCRIPTION
*  enable STP PSM
* PARAMETERS
*  int idle_time_to_sleep: IDLE time to sleep
* RETURNS
*  0: Sccuess  Negative value: Fail
*****************************************************************************/
extern INT32 mtk_wcn_stp_psm_enable(INT32 idle_time_to_sleep);

/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_psm_disable
* DESCRIPTION
*  disable STP PSM
* PARAMETERS
*  void
* RETURNS
*  0: Sccuess  Negative value: Fail
*****************************************************************************/
extern INT32 mtk_wcn_stp_psm_disable(VOID);

/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_psm_reset
* DESCRIPTION
*  reset STP PSM (used on whole chip reset)
* PARAMETERS
*  void
* RETURNS
*  0: Sccuess  Negative value: Fail
*****************************************************************************/
extern INT32 mtk_wcn_stp_psm_reset(VOID);
extern VOID stp_do_tx_timeout(VOID);

/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_btm_get_dmp
* DESCRIPTION
*  get stp dump related information
* PARAMETERS
*  buffer: dump placement, len: dump size
* RETURNS
*   0: Success Negative Value: Fail
*****************************************************************************/
extern INT32 mtk_wcn_stp_btm_get_dmp(CHAR *buf, INT32 *len);

extern INT32 mtk_wcn_stp_dbg_enable(VOID);

extern INT32 mtk_wcn_stp_dbg_disable(VOID);

extern VOID mtk_wcn_stp_set_if_tx_type (ENUM_STP_TX_IF_TYPE stp_if_type);

extern INT32 mtk_wcn_sys_if_rx(UINT8 *data, INT32 size);

extern MTK_WCN_BOOL mtk_wcn_stp_dbg_level(UINT32 dbglevel);

extern INT32 mtk_wcn_stp_dbg_dump_package(VOID);

#if 0 /*move to core_exp.h*/
extern INT32  stp_drv_init(VOID);
extern VOID stp_drv_exit(VOID);

/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_init
* DESCRIPTION
*  init STP kernel
* PARAMETERS
*  cb_func      [IN] function pointers of system APIs
* RETURNS
*  INT32    0 = success, others = failure
*****************************************************************************/
extern INT32 mtk_wcn_stp_init(const mtkstp_callback * const cb_func);

/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_deinit
* DESCRIPTION
*  deinit STP kernel
* PARAMETERS
*  void
* RETURNS
*  INT32    0 = success, others = failure
*****************************************************************************/
extern INT32 mtk_wcn_stp_deinit(VOID);

/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_rx_queue
* DESCRIPTION
*  flush all stp rx queue
* PARAMETERS
*  none.
* RETURNS
*  none
*****************************************************************************/
extern VOID mtk_wcn_stp_flush_rx_queue(UINT32 type);

/*****************************************************************************
* FUNCTION
*  mtk_wcn_stp_send_data_raw
* DESCRIPTION
*  send raw data to common interface, bypass STP
* PARAMETERS
*  buffer      [IN]        data buffer
*  length      [IN]        data buffer length
*  type        [IN]        subfunction type
* RETURNS
*  INT32    length transmitted
*****************************************************************************/
extern INT32 mtk_wcn_stp_send_data_raw(const UINT8 *buffer, const UINT32 length, const UINT8 type);


/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
#endif

#endif /* _STP_CORE_H_ */

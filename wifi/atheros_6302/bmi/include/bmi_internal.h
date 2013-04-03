//------------------------------------------------------------------------------
// <copyright file="bmi_internal.h" company="Atheros">
//    Copyright (c) 2004-2008 Atheros Corporation.  All rights reserved.
// 
// The software source and binaries included in this development package are
// licensed, not sold. You, or your company, received the package under one
// or more license agreements. The rights granted to you are specifically
// listed in these license agreement(s). All other rights remain with Atheros
// Communications, Inc., its subsidiaries, or the respective owner including
// those listed on the included copyright notices.  Distribution of any
// portion of this package must be in strict compliance with the license
// agreement(s) terms.
// </copyright>
// 
// <summary>
// 	Wifi driver for AR6002
// </summary>
//
//------------------------------------------------------------------------------
//==============================================================================
//
// Author(s): ="Atheros"
//==============================================================================
#ifndef BMI_INTERNAL_H
#define BMI_INTERNAL_H

#include "a_config.h"
#include "athdefs.h"
#include "a_types.h"
#include "a_osapi.h"
#define ATH_MODULE_NAME bmi
#include "a_debug.h"
#include "AR6002/hw2.0/hw/mbox_host_reg.h"
#include "bmi_msg.h"

#define ATH_DEBUG_BMI  ATH_DEBUG_MAKE_MODULE_MASK(0)


#define BMI_COMMUNICATION_TIMEOUT       100000

/* ------ Global Variable Declarations ------- */
A_BOOL bmiDone;

A_STATUS
bmiBufferSend(HIF_DEVICE *device,
              A_UCHAR *buffer,
              A_UINT32 length);

A_STATUS
bmiBufferReceive(HIF_DEVICE *device,
                 A_UCHAR *buffer,
                 A_UINT32 length,
                 A_BOOL want_timeout);

#endif

/*
 *
 * Copyright (c) 2004-2010 Atheros Communications Inc.
 * All rights reserved.
 *
 * 
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
 *
 */

/*
 * Implementation of system power management 
 */

#include "ar6000_drv.h"
#include <linux/inetdevice.h>
#include <linux/platform_device.h>

#ifdef CONFIG_HAS_WAKELOCK
#include <linux/wakelock.h>
#endif

#define WOW_ENABLE_MAX_INTERVAL 0
#define WOW_SET_SCAN_PARAMS     0

#ifdef TARGET_EUROPA
extern void wlan_setup_power(int on, int detect);
#endif /* TARGET_EUROPA */

extern unsigned int wmitimeout;
extern wait_queue_head_t arEvent;

#ifdef CONFIG_PM
#ifdef CONFIG_HAS_WAKELOCK
struct wake_lock ar6k_suspend_wake_lock;
struct wake_lock ar6k_wow_wake_lock;
#endif
#endif /* CONFIG_PM */

#ifdef ANDROID_ENV
extern void android_ar6k_check_wow_status(AR_SOFTC_T *ar, struct sk_buff *skb, A_BOOL isEvent);
#endif 

#define  ATH_DEBUG_PM       ATH_DEBUG_MAKE_MODULE_MASK(0)

#ifdef DEBUG
static ATH_DEBUG_MASK_DESCRIPTION pm_debug_desc[] = {
    { ATH_DEBUG_PM     , "System power management"},
};

ATH_DEBUG_INSTANTIATE_MODULE_VAR(pm,
                                 "pm",
                                 "System Power Management",
                                 ATH_DEBUG_MASK_DEFAULTS | ATH_DEBUG_PM,
                                 ATH_DEBUG_DESCRIPTION_COUNT(pm_debug_desc),
                                 pm_debug_desc);

#endif /* DEBUG */

A_STATUS ar6000_exit_cut_power_state(AR_SOFTC_T *ar);

static void ar6k_send_asleep_event_to_app(AR_SOFTC_T *ar, A_BOOL asleep)
{
    char buf[128];
    union iwreq_data wrqu;

    snprintf(buf, sizeof(buf), "HOST_ASLEEP=%s", asleep ? "asleep" : "awake");
    A_MEMZERO(&wrqu, sizeof(wrqu));
    wrqu.data.length = strlen(buf);
    wireless_send_event(ar->arNetDev, IWEVCUSTOM, &wrqu, buf);
}

#ifdef CONFIG_PM
static void ar6000_wow_resume(AR_SOFTC_T *ar)
{
    if (ar->arWowState!= WLAN_WOW_STATE_NONE) {
        A_UINT16 fg_start_period = (ar->scParams.fg_start_period==0) ? 1 : ar->scParams.fg_start_period;
        A_UINT16 bg_period = (ar->scParams.bg_period==0) ? 60 : ar->scParams.bg_period;
        WMI_SET_HOST_SLEEP_MODE_CMD hostSleepMode = {TRUE, FALSE};
        ar->arWowState = WLAN_WOW_STATE_NONE;
#ifdef CONFIG_HAS_WAKELOCK
        wake_lock_timeout(&ar6k_wow_wake_lock, 3*HZ);
#endif
        if (wmi_set_host_sleep_mode_cmd(ar->arWmi, &hostSleepMode)!=A_OK) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("Fail to setup restore host awake\n"));
        }
#if WOW_SET_SCAN_PARAMS
        wmi_scanparams_cmd(ar->arWmi, fg_start_period,
                                   ar->scParams.fg_end_period,
                                   bg_period,
                                   ar->scParams.minact_chdwell_time,
                                   ar->scParams.maxact_chdwell_time,
                                   ar->scParams.pas_chdwell_time,
                                   ar->scParams.shortScanRatio,
                                   ar->scParams.scanCtrlFlags,
                                   ar->scParams.max_dfsch_act_time,
                                   ar->scParams.maxact_scan_per_ssid);
#else
       (void)fg_start_period; 
       (void)bg_period;
#endif 


#if WOW_ENABLE_MAX_INTERVAL /* we don't do it if the power consumption is already good enough. */
        if (wmi_listeninterval_cmd(ar->arWmi, ar->arListenIntervalT, ar->arListenIntervalB) == A_OK) {
        }
#endif
        ar6k_send_asleep_event_to_app(ar, FALSE); 
        AR_DEBUG_PRINTF(ATH_DEBUG_PM, ("Resume WoW successfully\n"));
    } else {
        AR_DEBUG_PRINTF(ATH_DEBUG_PM, ("WoW does not invoked. skip resume"));
    }
    ar->arWlanPowerState = WLAN_POWER_STATE_ON;
}

static void ar6000_wow_suspend(AR_SOFTC_T *ar)
{
#define WOW_LIST_ID 1
    if (ar->arNetworkType != AP_NETWORK) {
        /* Setup WoW for unicast & Arp request for our own IP
        disable background scan. Set listen interval into 1000 TUs
        Enable keepliave for 110 seconds
        */
        struct in_ifaddr **ifap = NULL;
        struct in_ifaddr *ifa = NULL;
        struct in_device *in_dev;
        A_UINT8 macMask[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
        A_STATUS status;
        WMI_ADD_WOW_PATTERN_CMD addWowCmd = { .filter = { 0 } };
        WMI_DEL_WOW_PATTERN_CMD delWowCmd;
        WMI_SET_HOST_SLEEP_MODE_CMD hostSleepMode = {FALSE, TRUE};
        WMI_SET_WOW_MODE_CMD wowMode = {    .enable_wow = TRUE, 
                                            .hostReqDelay = 500 };/*500 ms delay*/
        
        if (ar->arWowState!= WLAN_WOW_STATE_NONE) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("System already go into wow mode!\n"));
            return;
        }

        ar6000_TxDataCleanup(ar); /* IMPORTANT, otherwise there will be 11mA after listen interval as 1000*/

#if WOW_ENABLE_MAX_INTERVAL /* we don't do it if the power consumption is already good enough. */
        if (wmi_listeninterval_cmd(ar->arWmi, A_MAX_WOW_LISTEN_INTERVAL, 0) == A_OK) {
        }
#endif

#if WOW_SET_SCAN_PARAMS
        status = wmi_scanparams_cmd(ar->arWmi, 0xFFFF, 0, 0xFFFF, 0, 0, 0, 0, 0, 0, 0);
#endif 
        /* clear up our WoW pattern first */
        delWowCmd.filter_list_id = WOW_LIST_ID;
        delWowCmd.filter_id = 0;
        wmi_del_wow_pattern_cmd(ar->arWmi, &delWowCmd);

        /* setup unicast packet pattern for WoW */
        if (ar->arNetDev->dev_addr[1]) {
            addWowCmd.filter_list_id = WOW_LIST_ID;
            addWowCmd.filter_size = 6; /* MAC address */
            addWowCmd.filter_offset = 0;
            status = wmi_add_wow_pattern_cmd(ar->arWmi, &addWowCmd, ar->arNetDev->dev_addr, macMask, addWowCmd.filter_size);
            if (status != A_OK) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("Fail to add WoW pattern\n"));
            }
        }
        /* setup ARP request for our own IP */
        if ((in_dev = __in_dev_get_rtnl(ar->arNetDev)) != NULL) {
            for (ifap = &in_dev->ifa_list; (ifa = *ifap) != NULL; ifap = &ifa->ifa_next) {
                if (!strcmp(ar->arNetDev->name, ifa->ifa_label)) {
                    break; /* found */
                }
            }
        }
        if (ifa && ifa->ifa_local) {
            WMI_SET_IP_CMD ipCmd;
            memset(&ipCmd, 0, sizeof(ipCmd));
            ipCmd.ips[0] = ifa->ifa_local;
            status = wmi_set_ip_cmd(ar->arWmi, &ipCmd);
            if (status != A_OK) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("Fail to setup IP for ARP agent\n"));
            }
        }

#ifndef ATH6K_CONFIG_OTA_MODE
        wmi_powermode_cmd(ar->arWmi, REC_POWER);
#endif

        status = wmi_set_wow_mode_cmd(ar->arWmi, &wowMode);
        if (status != A_OK) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("Fail to enable wow mode\n"));
        }
        ar6k_send_asleep_event_to_app(ar, TRUE);

        status = wmi_set_host_sleep_mode_cmd(ar->arWmi, &hostSleepMode);
        if (status != A_OK) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("Fail to set host asleep\n"));
        }

        ar->arWowState = WLAN_WOW_STATE_SUSPENDING;
        if (ar->arTxPending[ar->arControlEp]) {
            A_UINT32 timeleft = wait_event_interruptible_timeout(arEvent,
            ar->arTxPending[ar->arControlEp] == 0, wmitimeout * HZ);
            if (!timeleft || signal_pending(current)) {
               /* what can I do? wow resume at once */
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("Fail to setup WoW. Pending wmi control data %d\n", ar->arTxPending[ar->arControlEp]));
            }
        }

        status = hifWaitForPendingRecv(ar->arHifDevice);

        ar->arWowState = WLAN_WOW_STATE_SUSPENDED;
        ar->arWlanPowerState = WLAN_POWER_STATE_WOW;
    } else {
        AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("Not allowed to go to WOW at this moment.\n"));
    }
}

A_STATUS ar6000_suspend_ev(void *context)
{
    A_STATUS status = A_OK;
    AR_SOFTC_T *ar = (AR_SOFTC_T *)context;
    A_INT16 pmmode = ar->arSuspendConfig;
    A_BOOL cutPower;

wow_not_connected:
    switch (pmmode) {
    case WLAN_SUSPEND_DEEP_SLEEP:
        ar->arWlanState = WLAN_DISABLED;
        ar6000_enter_exit_deep_sleep_state(ar, FALSE);
        AR_DEBUG_PRINTF(ATH_DEBUG_PM,("%s:Suspend for deep sleep mode %d\n", __func__, ar->arWlanPowerState));
        break;
    case WLAN_SUSPEND_WOW:
        if (ar->arWmiReady && ar->arWlanState==WLAN_ENABLED && ar->arConnected) {
            ar6000_wow_suspend(ar);
            AR_DEBUG_PRINTF(ATH_DEBUG_PM,("%s:Suspend for wow mode %d\n", __func__, ar->arWlanPowerState));
            /* leave for pm_device to setup wow */
        } else {
            pmmode = ar->arWow2Config;
            goto wow_not_connected;
        }
        break;
    case WLAN_SUSPEND_CUT_PWR:
        /* fall through */
    default:
        ar->arWlanState = WLAN_DISABLED;
        cutPower = TRUE;
        if (ar->arSuspendCutPwrConfig == CUT_POWER_ALWAYS) {
            cutPower = TRUE;
        } else {
            /* 
             * Both radio's are OFF, enter cut power 
             * Check BT only for BT clock sharing designs 
             */
            if ((ar->arWlanOff) &&
                ((ar->arBTOff) || (!ar->arBTSharing)))
            {
                cutPower = TRUE;
            } else {
                cutPower = FALSE;
            }
        }
        if (cutPower) {
            ar6000_enter_exit_cut_power_state(ar, FALSE);
        } else {
            ar6000_enter_exit_deep_sleep_state(ar, FALSE);
        }
        AR_DEBUG_PRINTF(ATH_DEBUG_PM,("%s:Suspend for cut power mode %d\n", __func__, ar->arWlanPowerState));
        break;
    }

    ar->scan_triggered = 0;
    return status;
}

A_STATUS ar6000_resume_ev(void *context)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)context;
    A_UINT16 powerState = ar->arWlanPowerState;

#ifdef CONFIG_HAS_WAKELOCK
    wake_lock(&ar6k_suspend_wake_lock);
#endif
    AR_DEBUG_PRINTF(ATH_DEBUG_PM, ("%s: enter previous state %d wowState %d\n", __func__, powerState, ar->arWowState));
    switch (powerState) {
    case WLAN_POWER_STATE_WOW:
        ar6000_wow_resume(ar);
        break;
    case WLAN_POWER_STATE_CUT_PWR:
        /* WiFi is ON before suspend */
        if (!ar->arWlanOff) {
            ar6000_enter_exit_cut_power_state(ar, TRUE);
            ar->arWlanState = WLAN_ENABLED;
        } else {
            /* 
             * WiFi is OFF before suspend.
             * For BT clock sharing designs, exit cut_power mode 
             * and enter deep sleep mode, if BT is ON.
             */
            if ((ar->arBTSharing) && (!ar->arBTOff)) {
                ar6000_enter_exit_cut_power_state(ar, TRUE);
                ar6000_enter_exit_deep_sleep_state(ar, FALSE);
            }
        }
        break;
    case WLAN_POWER_STATE_DEEP_SLEEP:
     /* WiFi is ON before suspend */
        if (!ar->arWlanOff) {
            ar6000_enter_exit_deep_sleep_state(ar, TRUE);
            ar->arWlanState = WLAN_ENABLED;
        }
        break;
    case WLAN_POWER_STATE_ON:
        break;
    default:
        AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("Strange SDIO bus power mode!!\n"));
        break;
    }
#ifdef CONFIG_HAS_WAKELOCK
    wake_unlock(&ar6k_suspend_wake_lock);
#endif
    return A_OK;
}

void ar6000_check_wow_status(AR_SOFTC_T *ar, struct sk_buff *skb, A_BOOL isEvent)
{
    if (ar->arWowState!=WLAN_WOW_STATE_NONE) {
        if (ar->arWowState==WLAN_WOW_STATE_SUSPENDING) {
            AR_DEBUG_PRINTF(ATH_DEBUG_PM,("\n%s: Received IRQ while we are wow suspending!!!\n\n", __func__));
            return;
        }
        /* Wow resume from irq interrupt */
        AR_DEBUG_PRINTF(ATH_DEBUG_PM, ("%s: WoW resume from irq thread status %d\n", __func__, ar->arWlanPowerState));
        ar6000_wow_resume(ar);
    } else {
#ifdef ANDROID_ENV
        android_ar6k_check_wow_status(ar, skb, isEvent);
#endif 
    }
}

A_STATUS ar6000_power_change_ev(void *context, A_UINT32 config)
{
    AR_SOFTC_T *ar = (AR_SOFTC_T *)context;
    A_STATUS status = A_OK;
  
    AR_DEBUG_PRINTF(ATH_DEBUG_PM, ("%s: power change event callback %d \n", __func__, config));
    switch (config) {
       case HIF_DEVICE_POWER_UP:
            status = ar6000_exit_cut_power_state(ar);
            break;
       case HIF_DEVICE_POWER_DOWN:
       case HIF_DEVICE_POWER_CUT:
            status = A_OK;
            break;
    }
    return status;
}

static void ar6000_pwr_on(AR_SOFTC_T *ar)
{
    AR_DEBUG_PRINTF(ATH_DEBUG_INFO,("%s enter\n", __func__));
    if (ar == NULL) {
        /* turn on for all cards */
    }
#ifdef TARGET_EUROPA
    wlan_setup_power(1,1);
#endif

}

static void ar6000_pwr_down(AR_SOFTC_T *ar)
{
    AR_DEBUG_PRINTF(ATH_DEBUG_INFO,("%s enter\n", __func__));
    if (ar == NULL) {
        /* shutdown for all cards */
    }
#ifdef TARGET_EUROPA
    wlan_setup_power(0,0);
#endif

}

static int ar6000_pm_probe(struct platform_device *pdev)
{
    ar6000_pwr_on(NULL);
    return 0;
}

static int ar6000_pm_remove(struct platform_device *pdev)
{
    ar6000_pwr_down(NULL);
    return 0;
}

static int ar6000_pm_suspend(struct platform_device *pdev, pm_message_t state)
{
    return 0;
}

static int ar6000_pm_resume(struct platform_device *pdev)
{
    return 0;
}

static struct platform_driver ar6000_pm_device = {
    .probe      = ar6000_pm_probe,
    .remove     = ar6000_pm_remove,
    .suspend    = ar6000_pm_suspend,
    .resume     = ar6000_pm_resume,
    .driver     = {
        .name = "wlan_ar6000_pm",
    },
};
#endif /* CONFIG_PM */

A_STATUS ar6000_exit_cut_power_state(AR_SOFTC_T *ar) 
{
    WMI_REPORT_SLEEP_STATE_EVENT  wmiSleepEvent ;

    ar6000_restart_endpoint(ar->arNetDev);

    wmiSleepEvent.sleepState = WMI_REPORT_SLEEP_STATUS_IS_DEEP_SLEEP;
    ar6000_send_event_to_app(ar, WMI_REPORT_SLEEP_STATE_EVENTID, (A_UINT8*)&wmiSleepEvent, sizeof(WMI_REPORT_SLEEP_STATE_EVENTID));
    
    return A_OK;
}

A_STATUS 
ar6000_enter_exit_cut_power_state(AR_SOFTC_T *ar, A_BOOL exit)
{
    WMI_REPORT_SLEEP_STATE_EVENT  wmiSleepEvent ;
    A_STATUS                      status = A_OK;
    HIF_DEVICE_POWER_CHANGE_TYPE  config;

    AR_DEBUG_PRINTF(ATH_DEBUG_PM, ("%s: Cut power %d %d \n", __func__,exit, ar->arWlanPowerState));
#ifdef CONFIG_PM
    AR_DEBUG_PRINTF(ATH_DEBUG_PM, ("Wlan OFF %d BT OFf %d \n", ar->arWlanOff, ar->arBTOff));
#endif
    do {
        if (exit) {
            /* Not in cut power state.. exit */
            if (ar->arWlanPowerState != WLAN_POWER_STATE_CUT_PWR) {
                break;
            }

#ifdef TARGET_EUROPA
            wlan_setup_power(1,0);
#endif

            /* Change the state to ON */
            ar->arWlanPowerState = WLAN_POWER_STATE_ON;

    
            /* Indicate POWER_UP to HIF */
            config = HIF_DEVICE_POWER_UP; 
            status = HIFConfigureDevice(ar->arHifDevice,
                                HIF_DEVICE_POWER_STATE_CHANGE,
                                &config,
                                sizeof(HIF_DEVICE_POWER_CHANGE_TYPE));

            if (status == A_PENDING) {
                /* Previously, we decided to wait here until the device becomes fully functional since there is a chance that some entity tries to access the device once we return from the resume callback. However, it was observed that the resume process gets delayed too because of this wait. Commenting it out to speed up the process of resuming */
#if 0
                 /* Wait for WMI ready event */
                A_UINT32 timeleft = wait_event_interruptible_timeout(arEvent,
                            (ar->arWmiReady == TRUE), wmitimeout * HZ);
                if (!timeleft || signal_pending(current)) {
                    AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("ar6000 : Failed to get wmi ready \n"));
                    status = A_ERROR;
                    break;
                }
#endif
                status = A_OK;
            }  else if (status == A_OK) {
                ar6000_exit_cut_power_state(ar);
            }
        } else {
            /* Already in cut power state.. exit */
            if (ar->arWlanPowerState == WLAN_POWER_STATE_CUT_PWR) {
                break;
            }

    	    wmiSleepEvent.sleepState = WMI_REPORT_SLEEP_STATUS_IS_AWAKE;
            ar6000_send_event_to_app(ar, WMI_REPORT_SLEEP_STATE_EVENTID, (A_UINT8*)&wmiSleepEvent, sizeof(WMI_REPORT_SLEEP_STATE_EVENTID));

            ar6000_stop_endpoint(ar->arNetDev, TRUE, FALSE);

            config = HIF_DEVICE_POWER_CUT; 
            status = HIFConfigureDevice(ar->arHifDevice,
                                HIF_DEVICE_POWER_STATE_CHANGE,
                                &config,
                                sizeof(HIF_DEVICE_POWER_CHANGE_TYPE));

#ifdef TARGET_EUROPA
            wlan_setup_power(0,0);
#endif

            ar->arWlanPowerState = WLAN_POWER_STATE_CUT_PWR;
        }
    } while (0);

    return status;
}

A_STATUS 
ar6000_enter_exit_deep_sleep_state(AR_SOFTC_T *ar, A_BOOL exit)
{
    A_STATUS status = A_OK;

    if (down_interruptible(&ar->arSem)) {
        return A_ERROR;
    }

    AR_DEBUG_PRINTF(ATH_DEBUG_PM, ("%s: Deep sleep %d %d \n", __func__,exit, ar->arWlanPowerState));
#ifdef CONFIG_PM
    AR_DEBUG_PRINTF(ATH_DEBUG_PM, ("Wlan OFF %d BT OFf %d \n", ar->arWlanOff, ar->arBTOff));
#endif
    do {
        WMI_REPORT_SLEEP_STATE_EVENT  wmiSleepEvent ;
        WMI_SET_HOST_SLEEP_MODE_CMD hostSleepMode;

        if (exit) {
            A_UINT16 fg_start_period;

            /* Not in deep sleep state.. exit */
            if (ar->arWlanPowerState != WLAN_POWER_STATE_DEEP_SLEEP) {
                break;
            }

            fg_start_period = (ar->scParams.fg_start_period==0) ? 1 : ar->scParams.fg_start_period;
            hostSleepMode.awake = TRUE;
            hostSleepMode.asleep = FALSE;

            if ((status=wmi_set_host_sleep_mode_cmd(ar->arWmi, &hostSleepMode)) != A_OK) {
                break;    
            }

    	    wmiSleepEvent.sleepState = WMI_REPORT_SLEEP_STATUS_IS_AWAKE;
            ar6000_send_event_to_app(ar, WMI_REPORT_SLEEP_STATE_EVENTID, (A_UINT8*)&wmiSleepEvent, sizeof(WMI_REPORT_SLEEP_STATE_EVENTID));

            /* Enable foreground scanning */
            if ((status=wmi_scanparams_cmd(ar->arWmi, fg_start_period,
                                    ar->scParams.fg_end_period,
                                    ar->scParams.bg_period,
                                    ar->scParams.minact_chdwell_time,
                                    ar->scParams.maxact_chdwell_time,
                                    ar->scParams.pas_chdwell_time,
                                    ar->scParams.shortScanRatio,
                                    ar->scParams.scanCtrlFlags,
                                    ar->scParams.max_dfsch_act_time,
                                    ar->scParams.maxact_scan_per_ssid)) != A_OK) 
            {
                break;
            }

            if (ar->arSsidLen) {
                if (ar6000_connect_to_ap(ar) != A_OK) {
                    /* no need to report error if connection failed */
                    break;
                }
            }

            /* Change the state to ON */
            ar->arWlanPowerState = WLAN_POWER_STATE_ON;
        } else {
            WMI_SET_WOW_MODE_CMD wowMode = { .enable_wow = FALSE };

            /* Already in deep sleep state.. exit */
            if (ar->arWlanPowerState == WLAN_POWER_STATE_DEEP_SLEEP) {
                break;
            }

            /* make sure we disable wow for deep sleep */
            if ((status=wmi_set_wow_mode_cmd(ar->arWmi, &wowMode))!=A_OK) {
                break;
            }

            wmiSleepEvent.sleepState = WMI_REPORT_SLEEP_STATUS_IS_DEEP_SLEEP;
            ar6000_send_event_to_app(ar, WMI_REPORT_SLEEP_STATE_EVENTID, (A_UINT8*)&wmiSleepEvent, sizeof(WMI_REPORT_SLEEP_STATE_EVENTID));

            /* Disconnect from the AP and disable foreground scanning */
            AR6000_SPIN_LOCK(&ar->arLock, 0);
            if (ar->arConnected == TRUE || ar->arConnectPending == TRUE) {
                AR6000_SPIN_UNLOCK(&ar->arLock, 0);
                wmi_disconnect_cmd(ar->arWmi);
            } else {
                AR6000_SPIN_UNLOCK(&ar->arLock, 0);
            }

            ar->scan_triggered = 0;

            if ((status=wmi_scanparams_cmd(ar->arWmi, 0xFFFF, 0, 0, 0, 0, 0, 0, 0, 0, 0)) != A_OK) {
                break;
            }
            ar6000_TxDataCleanup(ar);
#ifndef ATH6K_CONFIG_OTA_MODE
            wmi_powermode_cmd(ar->arWmi, REC_POWER);
#endif

            hostSleepMode.awake = FALSE;
            hostSleepMode.asleep = TRUE;
            if ((status=wmi_set_host_sleep_mode_cmd(ar->arWmi, &hostSleepMode))!=A_OK) {
                break;
            }
            if (ar->arTxPending[ar->arControlEp]) {
                A_UINT32 timeleft = wait_event_interruptible_timeout(arEvent,
                                ar->arTxPending[ar->arControlEp] == 0, wmitimeout * HZ);
                if (!timeleft || signal_pending(current)) {
                    status = A_ERROR;
                    break;
                }
            }   
            status = hifWaitForPendingRecv(ar->arHifDevice);

            ar->arWlanPowerState = WLAN_POWER_STATE_DEEP_SLEEP;
        }
    } while (0);

    if (status!=A_OK) {
        AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("Fail to enter/exit deep sleep %d\n", exit));
    }
    up(&ar->arSem);    
    
    return status;
}

A_STATUS 
ar6000_set_bt_hw_state(AR_SOFTC_T *ar, A_UINT32 state)
{
    A_STATUS status = A_OK;
#ifdef CONFIG_PM
    A_UINT32 oldState;
    A_BOOL off;

    AR_DEBUG_PRINTF(ATH_DEBUG_PM, ("%s: Set BT state %d \n", __func__,state));

    off = !state;

    oldState = ar->arBTOff;
    ar->arBTOff = off;
    do {
        if (off) {
            /* BT is OFF */
            if (ar->arWlanOff) {
                /* If wlan is OFF and configured for CUT_POWER, transition now */
                if ((ar->arWlanOffConfig == WLAN_OFF_CUT_PWR) &&
                    (ar->arWlanPowerState == WLAN_POWER_STATE_DEEP_SLEEP)) 
                {
                    /* ??? Do we need to exit deep sleep */
                    /* Enter cut power */
                    status = ar6000_enter_exit_cut_power_state(ar, FALSE);
                    if (status != A_OK) {
                        break;
                    }
                }
            }
        } else {
            /* BT is ON */
            if (ar->arWlanOff) {
                /* WiFi is OFF, For BT clock sharing designs switch to DEEP SLEEP */
                if ((ar->arBTSharing) &&
                    (ar->arWlanPowerState == WLAN_POWER_STATE_CUT_PWR)) 
                {
                    /* Exit from cut power state */
                    status = ar6000_enter_exit_cut_power_state(ar, TRUE);
                    if (status != A_OK) {
                        break;
                    }
                    /* Enter deep sleep */
                    status = ar6000_enter_exit_deep_sleep_state(ar, FALSE);
                    if (status != A_OK) {
                        break;
                    }
                }
            }
        }
    } while(0);

    if (status != A_OK) {
        AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("Fail to setup BT HW state %d\n", state));
        ar->arBTOff = oldState;
    }
   
#endif /* CONFIG_PM */ 
    return status;
}

A_STATUS 
ar6000_set_wlan_state(AR_SOFTC_T *ar, AR6000_WLAN_STATE state)
{
    A_STATUS status = A_OK;
    A_UINT16 powerState;
    AR6000_WLAN_STATE oldstate = ar->arWlanState;
#ifdef CONFIG_PM
    A_BOOL oldWlanOff = ar->arWlanOff;
#endif

    AR_DEBUG_PRINTF(ATH_DEBUG_PM, ("%s: Set WLAN state %d \n", __func__,state));
    if (((ar->arWmiReady == FALSE) && 
         (ar->arWlanPowerState != WLAN_POWER_STATE_CUT_PWR)) ||
        (state!=WLAN_DISABLED && state!=WLAN_ENABLED)) 
    {
        return A_ERROR;
    }

    if (state == ar->arWlanState) {
        return A_OK;
    }

    if (ar->bIsDestroyProgress) {
        return A_EBUSY;
    }

    ar->arWlanState = state;
    do {
        if (ar->arWlanState == WLAN_ENABLED) {
#ifdef CONFIG_PM
            ar->arWlanOff = FALSE;
#endif /* CONFIG_PM */
            powerState = ar->arWlanPowerState;

            if (powerState == WLAN_POWER_STATE_DEEP_SLEEP) {
                status = ar6000_enter_exit_deep_sleep_state(ar, TRUE);
            } else if (powerState == WLAN_POWER_STATE_CUT_PWR) {
                status = ar6000_enter_exit_cut_power_state(ar, TRUE);
            }
        } else {
            powerState = WLAN_POWER_STATE_DEEP_SLEEP;
#ifdef CONFIG_PM
            if (ar->arWlanOffConfig == WLAN_OFF_CUT_PWR) {
              /* For BT clock sharing designs, CUT_POWER depend on BT state */  
                if ((!ar->arBTSharing) || (ar->arBTOff)) {
                    powerState = WLAN_POWER_STATE_CUT_PWR;
                }
            }
#endif /* CONFIG_PM */
            if (powerState == WLAN_POWER_STATE_DEEP_SLEEP) {
                status = ar6000_enter_exit_deep_sleep_state(ar, FALSE);
            } else if (powerState == WLAN_POWER_STATE_CUT_PWR) {
                status = ar6000_enter_exit_cut_power_state(ar, FALSE);
            }

#ifdef CONFIG_PM
            ar->arWlanOff = TRUE;
#endif /* CONFIG_PM */
        }
    } while (0);

    if (status!=A_OK) {
        AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("Fail to setup WLAN state %d\n", ar->arWlanState));
        ar->arWlanState = oldstate;
#ifdef CONFIG_PM
        ar->arWlanOff = oldWlanOff;
#endif /* CONFIG_PM */
    }
    
    return status;
}

void ar6000_pm_init()
{
#ifdef CONFIG_PM
#ifdef CONFIG_HAS_WAKELOCK
    wake_lock_init(&ar6k_suspend_wake_lock, WAKE_LOCK_SUSPEND, "ar6k_suspend");
    wake_lock_init(&ar6k_wow_wake_lock, WAKE_LOCK_SUSPEND, "ar6k_wow");
#endif
    /* 
     * Register ar6000_pm_device into system.
     * We should also add platform_device into the first item of array
     * of devices[] in file arch/xxx/mach-xxx/board-xxxx.c
     */
    if (platform_driver_register(&ar6000_pm_device)) {
        AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("ar6000: fail to register the power control driver.\n"));
    }
#endif /* CONFIG_PM */
}

void ar6000_pm_exit()
{
#ifdef CONFIG_PM
    platform_driver_unregister(&ar6000_pm_device);
#ifdef CONFIG_HAS_WAKELOCK
    wake_lock_destroy(&ar6k_suspend_wake_lock);
    wake_lock_destroy(&ar6k_wow_wake_lock);
#endif
#endif /* CONFIG_PM */
}

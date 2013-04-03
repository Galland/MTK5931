//------------------------------------------------------------------------------
// Copyright (c) 2004-2010 Atheros Communications Inc.
// All rights reserved.
//
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
//
// Author(s): ="Atheros"
//------------------------------------------------------------------------------
#include "ar6000_drv.h"
#undef ATH_MODULE_NAME
#define ATH_MODULE_NAME android
#include "htc.h"
#include <linux/vmalloc.h>
#include <linux/fs.h>

#ifdef CONFIG_HAS_WAKELOCK
#include <linux/wakelock.h>
#endif
#include <linux/earlysuspend.h>

A_BOOL enable_mmc_host_detect_change = 0;
static void ar6000_enable_mmchost_detect_change(int enable);

#ifdef DEBUG
static ATH_DEBUG_MASK_DESCRIPTION android_debug_desc[] = {
};

ATH_DEBUG_INSTANTIATE_MODULE_VAR(android,
                                 "android",
                                 "Android Driver Interface",
                                 ATH_DEBUG_MASK_DEFAULTS,
                                 ATH_DEBUG_DESCRIPTION_COUNT(android_debug_desc),
                                 android_debug_desc);

#endif /* DEBUG */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
char fwpath[256] = "/system/wifi";
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) */
int wowledon;
unsigned int enablelogcat;

extern int bmienable;
extern struct net_device *ar6000_devices[];
extern char ifname[];
extern unsigned int bypasswmi;
extern struct wake_lock ar6k_wow_wake_lock;

const char def_ifname[] = "eth0";
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
module_param_string(fwpath, fwpath, sizeof(fwpath), 0644);
module_param(enablelogcat, uint, 0644);
module_param(wowledon, int, 0644);
#else
#define __user
/* for linux 2.4 and lower */
MODULE_PARAM(wowledon,"i");
#endif 
#ifdef CONFIG_PM
struct wake_lock ar6k_init_wake_lock;
#endif 
static int screen_is_off;
static struct early_suspend ar6k_early_suspend;
static A_STATUS (*ar6000_avail_ev_p)(void *, void *);

#if !defined(CONFIG_MMC_MSM) || LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32)
int logger_write(const enum logidx index,
                const unsigned char prio,
                const char __kernel * const tag,
                const char __kernel * const fmt,
                ...)
{
    int ret = 0;
    va_list vargs;
    struct file *filp = (struct file *)-ENOENT;
    mm_segment_t oldfs;
    struct iovec vec[3];
    int tag_bytes = strlen(tag) + 1, msg_bytes;
    char *msg;      
    va_start(vargs, fmt);
    msg = kvasprintf(GFP_ATOMIC, fmt, vargs);
    va_end(vargs);
    if (!msg)
        return -ENOMEM;
    if (in_interrupt()) {
        /* we have no choice since aio_write may be blocked */
        printk(KERN_ALERT "%s", msg);
        goto out_free_message;
    }
    msg_bytes = strlen(msg) + 1;
    if (msg_bytes <= 1) /* empty message? */
        goto out_free_message; /* don't bother, then */
    if ((msg_bytes + tag_bytes + 1) > 2048) {
        ret = -E2BIG;
        goto out_free_message;
    }
            
    vec[0].iov_base  = (unsigned char *) &prio;
    vec[0].iov_len    = 1;
    vec[1].iov_base   = (void *) tag;
    vec[1].iov_len    = strlen(tag) + 1;
    vec[2].iov_base   = (void *) msg;
    vec[2].iov_len    = strlen(msg) + 1; 

    oldfs = get_fs();
    set_fs(KERNEL_DS);
    do {
        filp = filp_open("/dev/log/main", O_WRONLY, S_IRUSR);
        if (IS_ERR(filp) || !filp->f_op) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("%s: filp_open /dev/log/main error\n", __FUNCTION__));
            ret = -ENOENT;
            break;
        }

        if (filp->f_op->aio_write) {
            int nr_segs = sizeof(vec) / sizeof(vec[0]);
            int len = vec[0].iov_len + vec[1].iov_len + vec[2].iov_len;
            struct kiocb kiocb;
            init_sync_kiocb(&kiocb, filp);
            kiocb.ki_pos = 0;
            kiocb.ki_left = len;
            kiocb.ki_nbytes = len;
            ret = filp->f_op->aio_write(&kiocb, vec, nr_segs, kiocb.ki_pos);
        }
        
    } while (0);

    if (!IS_ERR(filp)) {
        filp_close(filp, NULL);
    }
    set_fs(oldfs);
out_free_message:
    if (msg) {
        kfree(msg);
    }
    return ret;
}
#endif

int android_logger_lv(void *module, int mask)
{
    switch (mask) {
    case ATH_DEBUG_ERR:
        return 6;
    case ATH_DEBUG_INFO:
        return 4;
    case ATH_DEBUG_WARN:
        return 5; 
    case ATH_DEBUG_TRC:        
        return 3; 
    default:
#ifdef DEBUG
        if (!module) {
            return 3;
        } else if (module == &GET_ATH_MODULE_DEBUG_VAR_NAME(driver)) {
            return (mask <=ATH_DEBUG_MAKE_MODULE_MASK(3)) ? 3 : 2;
        } else if (module == &GET_ATH_MODULE_DEBUG_VAR_NAME(htc)) {
            return 2;
        } else {
            return 3;
        }
#else
        return 3; /* DEBUG */
#endif
    }
}

static int android_readwrite_file(const A_CHAR *filename, A_CHAR *rbuf, const A_CHAR *wbuf, size_t length)
{
    int ret = 0;
    struct file *filp = (struct file *)-ENOENT;
    mm_segment_t oldfs;
    oldfs = get_fs();
    set_fs(KERNEL_DS);
    do {
        int mode = (wbuf) ? O_RDWR : O_RDONLY;
        filp = filp_open(filename, mode, S_IRUSR);
        if (IS_ERR(filp) || !filp->f_op) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("%s: file %s filp_open error\n", __FUNCTION__, filename));
            ret = -ENOENT;
            break;
        }
    
        if (length==0) {
            /* Read the length of the file only */
            struct inode    *inode;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,20)
            inode = filp->f_path.dentry->d_inode;
#else
            inode = filp->f_dentry->d_inode;
#endif
		    if (!inode) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("%s: Get inode from %s failed\n", __FUNCTION__, filename));
                ret = -ENOENT;
                break;
            }
            ret = i_size_read(inode->i_mapping->host);
            break;
        }

        if (wbuf) {
            if ( (ret=filp->f_op->write(filp, wbuf, length, &filp->f_pos)) < 0) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("%s: Write %u bytes to file %s error %d\n", __FUNCTION__, 
                                length, filename, ret));
                break;
            }
        } else {
            if ( (ret=filp->f_op->read(filp, rbuf, length, &filp->f_pos)) < 0) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("%s: Read %u bytes from file %s error %d\n", __FUNCTION__,
                                length, filename, ret));
                break;
            }
        }
    } while (0);

    if (!IS_ERR(filp)) {
        filp_close(filp, NULL);
    }
    set_fs(oldfs);

    return ret;
}

int android_request_firmware(const struct firmware **firmware_p, const char *name,
                     struct device *device)
{
    int ret = 0;
    struct firmware *firmware;
    char filename[256];
    const char *raw_filename = name;
	*firmware_p = firmware = kzalloc(sizeof(*firmware), GFP_KERNEL);
    if (!firmware) 
		return -ENOMEM;
	sprintf(filename, "%s/%s", fwpath, raw_filename);
    do {
        size_t length, bufsize, bmisize;

        if ( (ret=android_readwrite_file(filename, NULL, NULL, 0)) < 0) {
            break;
        } else {
            length = ret;
        }
    
        bufsize = ALIGN(length, PAGE_SIZE);
        bmisize = A_ROUND_UP(length, 4);
        bufsize = max(bmisize, bufsize);
        firmware->data = vmalloc(bufsize);
        firmware->size = length;
        if (!firmware->data) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("%s: Cannot allocate buffer for firmware\n", __FUNCTION__));
            ret = -ENOMEM;
            break;
        }
    
        if ( (ret=android_readwrite_file(filename, (char*)firmware->data, NULL, length)) != length) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("%s: file read error, ret %d request %d\n", __FUNCTION__, ret, length));
            ret = -1;
            break;
        }
    
    } while (0);

    if (ret<0) {
        if (firmware) {
            if (firmware->data)
                vfree(firmware->data);
            kfree(firmware);
        }
        *firmware_p = NULL;
    } else {
        ret = 0;
    }
    return ret;    
}

void android_release_firmware(const struct firmware *firmware)
{
	if (firmware) {
        if (firmware->data)
            vfree(firmware->data);
        kfree(firmware);
    }
}

static A_STATUS ar6000_android_avail_ev(void *context, void *hif_handle)
{
    A_STATUS ret;
#ifdef CONFIG_PM
   wake_lock(&ar6k_init_wake_lock);
#endif
    ar6000_enable_mmchost_detect_change(0);
    ret = ar6000_avail_ev_p(context, hif_handle);
#ifdef CONFIG_PM
   wake_unlock(&ar6k_init_wake_lock);
#endif
    return ret;
}

/* Useful for qualcom platform to detect our wlan card for mmc stack */
static void ar6000_enable_mmchost_detect_change(int enable)
{
#ifdef CONFIG_MMC_MSM
#define MMC_MSM_DEV "msm_sdcc.1"
    char buf[3];
    int length;

    if (!enable_mmc_host_detect_change) {
        return;
    }
    length = snprintf(buf, sizeof(buf), "%d\n", enable ? 1 : 0);
    if (android_readwrite_file("/sys/devices/platform/" MMC_MSM_DEV "/detect_change", 
                               NULL, buf, length) < 0) {
        /* fall back to polling */
        android_readwrite_file("/sys/devices/platform/" MMC_MSM_DEV "/polling", NULL, buf, length);
    }
#endif
}

#ifdef CONFIG_HAS_EARLYSUSPEND
static void android_early_suspend(struct early_suspend *h)
{
    screen_is_off = 1;
}

static void android_late_resume(struct early_suspend *h)
{
    screen_is_off = 0;
}
#endif

void android_module_init(OSDRV_CALLBACKS *osdrvCallbacks)
{
    bmienable = 1;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    if (ifname[0] == '\0')
        strcpy(ifname, def_ifname);
#endif 
#ifdef CONFIG_PM
    wake_lock_init(&ar6k_init_wake_lock, WAKE_LOCK_SUSPEND, "ar6k_init");
#endif 
#ifdef CONFIG_HAS_EARLYSUSPEND
    ar6k_early_suspend.suspend = android_early_suspend;
    ar6k_early_suspend.resume  = android_late_resume;
    ar6k_early_suspend.level   = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
    register_early_suspend(&ar6k_early_suspend);
#endif

    ar6000_avail_ev_p = osdrvCallbacks->deviceInsertedHandler;
    osdrvCallbacks->deviceInsertedHandler = ar6000_android_avail_ev;

    ar6000_enable_mmchost_detect_change(1);
}

void android_module_exit(void)
{
#ifdef CONFIG_HAS_EARLYSUSPEND
    unregister_early_suspend(&ar6k_early_suspend);
#endif
#ifdef CONFIG_PM
    wake_lock_destroy(&ar6k_init_wake_lock);
#endif
    ar6000_enable_mmchost_detect_change(1);
}

A_STATUS android_ar6k_start(AR_SOFTC_T *ar)
{
    if (!bypasswmi) {
#ifdef ATH6K_CONFIG_OTA_MODE
        wmi_powermode_cmd(ar->arWmi, MAX_PERF_POWER);
#endif
        wmi_disctimeout_cmd(ar->arWmi, 3);

    }
    return A_OK;
}

#ifdef CONFIG_PM
void android_ar6k_check_wow_status(AR_SOFTC_T *ar, struct sk_buff *skb, A_BOOL isEvent)
{
    if (screen_is_off && skb && ar->arConnected) {
        A_BOOL needWake = FALSE;
        if (isEvent) {
            if (A_NETBUF_LEN(skb) >= sizeof(A_UINT16)) {
                A_UINT16 cmd = *(const A_UINT16 *)A_NETBUF_DATA(skb);
                switch (cmd) {
                case WMI_CONNECT_EVENTID:
                case WMI_DISCONNECT_EVENTID:
                    needWake = TRUE;
                    break;
                default:
                    /* dont wake lock the system for other event */
                    break;
                }
            }
        } else if (A_NETBUF_LEN(skb) >= sizeof(ATH_MAC_HDR)) {
            ATH_MAC_HDR *datap = (ATH_MAC_HDR *)A_NETBUF_DATA(skb);
            if (!IEEE80211_IS_MULTICAST(datap->dstMac)) {
                switch (A_BE2CPU16(datap->typeOrLen)) {
                case 0x0800: /* IP */
                case 0x888e: /* EAPOL */
                case 0x88c7: /* RSN_PREAUTH */
                case 0x88b4: /* WAPI */
                     needWake = TRUE;
                     break;
                case 0x0806: /* ARP is not important to hold wake lock */
                default:
                    break;
                }
            }
        }
        if (needWake) {
            /* keep host wake up if there is any event and packate comming in*/
            wake_lock_timeout(&ar6k_wow_wake_lock, 3*HZ);
            if (wowledon) {
                char buf[32];
                int len = sprintf(buf, "on");
                android_readwrite_file("/sys/power/state", NULL, buf, len);

                len = sprintf(buf, "%d", 127);
                android_readwrite_file("/sys/class/leds/lcd-backlight/brightness",
                                       NULL, buf,len);
            }
        }
    }
}
#endif /* CONFIG_PM */

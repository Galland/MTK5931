# Support ATE function
HAS_ATE=y

# Support QA ATE function
HAS_QA_SUPPORT=n

HAS_RSSI_FEEDBACK=n

# Support XLINK mode
HAS_XLINK=n

HAS_NINTENDO=n

# Support LLTD function
HAS_LLTD=n

# Support WDS function
HAS_WDS=n

# Support AP-Client function
HAS_APCLI=n

# Support Wpa_Supplicant
HAS_WPA_SUPPLICANT=y

# Support Native WpaSupplicant for Network Maganger
HAS_NATIVE_WPA_SUPPLICANT_SUPPORT=y

#Support Net interface block while Tx-Sw queue full
HAS_BLOCK_NET_IF=n

#Support IGMP-Snooping function.
HAS_IGMP_SNOOP_SUPPORT=n

#Support DFS function
HAS_DFS_SUPPORT=n

#Support Carrier-Sense function
HAS_CS_SUPPORT=n

# Support user specific transmit rate of Multicast packet.
HAS_MCAST_RATE_SPECIFIC_SUPPORT=n

# Support for Multiple Cards
HAS_MC_SUPPORT=n

#Support for PCI-MSI
HAS_MSI_SUPPORT=n

#Support for IEEE802.11e DLS
HAS_QOS_DLS_SUPPORT=n

#Support for EXT_CHANNEL
HAS_EXT_BUILD_CHANNEL_LIST=n

#Support for IDS 
HAS_IDS_SUPPORT=n

#Support for Net-SNMP
HAS_SNMP_SUPPORT=n

#Support features of 802.11n Draft3
HAS_DOT11N_DRAFT3_SUPPORT=y

#Support features of Single SKU. 
HAS_SINGLE_SKU_SUPPORT=n

#Support features of 802.11n
HAS_DOT11_N_SUPPORT=y

#Support for 2860/2880 co-exist 
HAS_RT2880_RT2860_COEXIST=n

HAS_KTHREAD_SUPPORT=y

#Support for Auto channel select enhance
HAS_AUTO_CH_SELECT_ENHANCE=n

#Support statistics count
HAS_STATS_COUNT=n

#Client support WDS function
HAS_CLIENT_WDS_SUPPORT=n

#Support for Bridge Fast Path & Bridge Fast Path function open to other module
HAS_BGFP_SUPPORT=n
HAS_BGFP_OPEN_SUPPORT=n

# Support HOSTAPD function
HAS_HOSTAPD_SUPPORT=n

#Support GreenAP function
HAS_GREENAP_SUPPORT=n

#Support MAC80211 LINUX-only function
HAS_CFG80211_SUPPORT=n

OSABL = n
HAS_STA_SUPPORT = y
#################################################

EXTRA_CFLAGS += -DAGGREGATION_SUPPORT -DPIGGYBACK_SUPPORT -DWMM_SUPPORT  -DLINUX -Wall -Wstrict-prototypes -Wno-trigraphs 
EXTRA_CFLAGS += -DSYSTEM_LOG_SUPPORT -DLED_CONTROL_SUPPORT
#EXTRA_CFLAGS += -DRESOURCE_PRE_ALLOC


ifeq ($(HAS_KTHREAD_SUPPORT),y)
EXTRA_CFLAGS += -DKTHREAD_SUPPORT
endif

#################################################

# config for STA mode
ifeq ($(HAS_STA_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_STA_SUPPORT -DDBG
endif

ifeq ($(HAS_XLINK),y)
EXTRA_CFLAGS += -DXLINK_SUPPORT
endif

ifeq ($(HAS_WPA_SUPPLICANT),y)
EXTRA_CFLAGS += -DWPA_SUPPLICANT_SUPPORT
ifeq ($(HAS_NATIVE_WPA_SUPPLICANT_SUPPORT),y)
EXTRA_CFLAGS += -DNATIVE_WPA_SUPPLICANT_SUPPORT
endif
endif

ifeq ($(HAS_ATE),y)
EXTRA_CFLAGS += -DRALINK_ATE
ifeq ($(HAS_QA_SUPPORT),y)
EXTRA_CFLAGS += -DRALINK_28xx_QA
endif
endif

ifeq ($(HAS_SNMP_SUPPORT),y)
EXTRA_CFLAGS += -DSNMP_SUPPORT
endif

ifeq ($(HAS_QOS_DLS_SUPPORT),y)
EXTRA_CFLAGS += -DQOS_DLS_SUPPORT
endif

ifeq ($(HAS_DOT11_N_SUPPORT),y)
EXTRA_CFLAGS += -DDOT11_N_SUPPORT
ifeq ($(HAS_DOT11N_DRAFT3_SUPPORT),y)
EXTRA_CFLAGS += -DDOT11N_DRAFT3
endif
endif

ifeq ($(HAS_CS_SUPPORT),y)
EXTRA_CFLAGS += -DCARRIER_DETECTION_SUPPORT
endif

ifeq ($(HAS_STATS_COUNT),y)
EXTRA_CFLAGS += -DSTATS_COUNT_SUPPORT
endif

ifeq ($(HAS_CFG80211_SUPPORT),y)
EXTRA_CFLAGS += -DRT_CFG80211_SUPPORT
endif

ifeq ($(OSABL),y)
EXTRA_CFLAGS += -DOS_ABL_SUPPORT
endif

#################################################

#################################################

#
# Common compiler flag
#

ifeq ($(HAS_EXT_BUILD_CHANNEL_LIST),y)
EXTRA_CFLAGS += -DEXT_BUILD_CHANNEL_LIST
endif

ifeq ($(HAS_IDS_SUPPORT),y)
EXTRA_CFLAGS += -DIDS_SUPPORT
endif


ifeq ($(OSABL),y)
EXTRA_CFLAGS += -DEXPORT_SYMTAB
endif

ifeq ($(HAS_CLIENT_WDS_SUPPORT),y)
EXTRA_CFLAGS += -DCLIENT_WDS
endif

ifeq ($(HAS_BGFP_SUPPORT),y)
EXTRA_CFLAGS += -DBG_FT_SUPPORT
endif

ifeq ($(HAS_BGFP_OPEN_SUPPORT),y)
EXTRA_CFLAGS += -DBG_FT_OPEN_SUPPORT
endif

#################################################
# ChipSet specific definitions.
#
EXTRA_CFLAGS +=-DRTMP_MAC_USB -DRT30xx -DRT3070 -DRTMP_USB_SUPPORT -DRTMP_TIMER_TASK_SUPPORT -DRTMP_RF_RW_SUPPORT -DRTMP_EFUSE_SUPPORT
#################################################
ifeq ($(HAS_BLOCK_NET_IF),y)
EXTRA_CFLAGS += -DBLOCK_NET_IF
endif

ifeq ($(HAS_DFS_SUPPORT),y)
EXTRA_CFLAGS += -DDFS_SUPPORT
endif

ifeq ($(HAS_MC_SUPPORT),y)
EXTRA_CFLAGS += -DMULTIPLE_CARD_SUPPORT
endif

ifeq ($(HAS_LLTD),y)
EXTRA_CFLAGS += -DLLTD_SUPPORT
endif

#kernel build options for 2.4
# move to Makefile outside LINUX_SRC := /opt/star/kernel/linux-2.4.27-star

#CFLAGS := -D__KERNEL__ -I$(LINUX_SRC)/include -I$(RT28xx_DIR)/include -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fno-strict-aliasing -fno-common -fomit-frame-pointer -G 0 -mno-abicalls -fno-pic -pipe  -finline-limit=100000 -march=mips2 -mabi=32 -Wa,--trap -DLINUX -nostdinc -iwithprefix include $(WFLAGS)
#EXTRA_CFLAGS += -I$(RT28xx_DIR)/include -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fno-strict-aliasing -fno-common -fomit-frame-pointer -fno-pic -pipe  -finline-limit=100000 -DLINUX -nostdinc -iwithprefix include $(WFLAGS)
EXTRA_CFLAGS += -D__KERNEL__ -I$(KERNEL_DIR)/include -I$(src)/include -Wall -Wstrict-prototypes -Wno-trigraphs -O2 -fno-strict-aliasing -fno-common -fomit-frame-pointer -fno-pic -pipe  -finline-limit=100000 -DLINUX -nostdinc -iwithprefix -D"KBUILD_MODNAME=rt3070sta"


obj-$(CONFIG_RALINK_3070_USB_WIFI) += rt3070sta.o

ifeq ($(HAS_STA_SUPPORT),y)
rt3070sta-objs += \
               common/crypt_md5.o\
               common/crypt_sha2.o\
               common/crypt_hmac.o\
               common/crypt_aes.o\
               common/crypt_arc4.o\
               common/mlme.o\
               common/cmm_wep.o\
               common/action.o\
               common/cmm_data.o\
               common/rtmp_init.o\
               common/cmm_tkip.o\
               common/cmm_aes.o\
               common/cmm_sync.o\
               common/eeprom.o\
               common/cmm_sanity.o\
               common/cmm_info.o\
               common/cmm_cfg.o\
               common/cmm_wpa.o\
               common/dfs.o\
               common/spectrum.o\
               common/rtmp_timer.o\
               common/rt_channel.o\
               common/cmm_profile.o\
               common/cmm_asic.o\
               common/cmm_cmd.o\
               sta/assoc.o\
               sta/auth.o\
               sta/auth_rsp.o\
               sta/sync.o\
               sta/sanity.o\
               sta/rtmp_data.o\
               sta/connect.o\
               sta/wpa.o\
               sta/sta_cfg.o

ifeq ($(OSABL),n)
rt3070sta-objs += \
               common/rtmp_init_inf.o\
               os/linux/rt_profile.o\
               os/linux/sta_ioctl.o\
               os/linux/rt_linux.o\
               os/linux/rt_main_dev.o
endif

ifeq ($(HAS_DOT11_N_SUPPORT),y)
rt3070sta-objs += \
               common/ba_action.o
endif

ifeq ($(HAS_ETH_CONVERT_SUPPORT), y)
rt3070sta-objs += \
               common/cmm_mat.o \
               common/cmm_mat_iparp.o \
               common/cmm_mat_pppoe.o \
               common/cmm_mat_ipv6.o
endif

ifeq ($(HAS_BLOCK_NET_IF),y)
rt3070sta-objs += common/netif_block.o
endif

ifeq ($(HAS_QOS_DLS_SUPPORT),y)
rt3070sta-objs += sta/dls.o
endif

rt3070sta-objs += \
               common/cmm_mac_usb.o\
               common/rtusb_io.o\
               common/rtusb_bulk.o\
               common/rtusb_data.o\
               common/cmm_data_usb.o\
               common/ee_prom.o\
               common/ee_efuse.o\
               common/rtmp_mcu.o\
               chips/rt30xx.o\
               common/rt_rf.o\
               chips/rt3070.o

ifeq ($(OSABL),n)
rt3070sta-objs += \
               common/rtusb_dev_id.o\
               os/linux/rt_usb.o\
               os/linux/rt_usb_util.o\
               os/linux/usb_main_dev.o
endif

ifeq ($(HAS_ATE),y)
rt3070sta-objs += os/linux/rt_ate.o
endif

ifdef CRDA_SUPPORT
ifeq ($(OSABL),n)
ifeq ($(HAS_CFG80211_SUPPORT),y)
rt3070sta-objs += \
               os/linux/cfg80211.o
endif
endif
endif #CRDA_SUPPORT

endif #HAS_STA_SUPPORT



#------------------------------------------------------------------------------
# <copyright file="makefile" company="Atheros">
#    Copyright (c) 2005-2010 Atheros Corporation.  All rights reserved.
# 
# The software source and binaries included in this development package are
# licensed, not sold. You, or your company, received the package under one
# or more license agreements. The rights granted to you are specifically
# listed in these license agreement(s). All other rights remain with Atheros
# Communications, Inc., its subsidiaries, or the respective owner including
# those listed on the included copyright notices.  Distribution of any
# portion of this package must be in strict compliance with the license
# agreement(s) terms.
# </copyright>
# 
# <summary>
# 	Wifi driver for AR6002
# </summary>
#
#------------------------------------------------------------------------------
#==============================================================================
# Author(s): ="Atheros"
#==============================================================================
#######################################################################################
#  AR6K Kernel Module makefile.
#
#    This makefile is invoked by the master makefile in the linux kernel, the linux kernel
#    source must be initially compiled.  The top level make will invoke the main linux kernel
#    makefile which will subsequently pick up this makefile.
#
#######################################################################################


REV ?= 2

EXTRA_CFLAGS += -I$(ATH_SRC_BASE)/include
EXTRA_CFLAGS += -I$(ATH_SRC_BASE)/../include
EXTRA_CFLAGS += -I$(ATH_SRC_BASE)/../include/AR6002/hw4.0
EXTRA_CFLAGS += -I$(ATH_SRC_BASE)/wlan/include
EXTRA_CFLAGS += -I$(ATH_SRC_BASE)/os/linux/include
EXTRA_CFLAGS += -I$(ATH_SRC_BASE)/os/
EXTRA_CFLAGS += -I$(ATH_SRC_BASE)/bmi/include

EXTRA_CFLAGS += -DLINUX -D__KERNEL__ \
                -DTCMD -DSEND_EVENT_TO_APP -DUSER_KEYS \
                -DNO_SYNC_FLUSH  -DHTC_EP_STAT_PROFILING -DWLAN_HEADERS\
                -DATH_AR6K_11N_SUPPORT -DWAPI_ENABLE #\
                -DMULTIPLE_FRAMES_PER_INTERRUPT -DAR6000REV$(REV) \
                -DBLOCK_TX_PATH_FLAG -DCHECKSUM_OFFLOAD\


ifeq ($(ATH_CFG80211_ENV),yes)
   EXTRA_CFLAGS += -DATH6K_CONFIG_CFG80211
endif

ifeq ($(ATH_ANDROID_ENV),yes)
    EXTRA_CFLAGS += -DANDROID_ENV -D__linux__ $(ATH_ANDROID_BUILD_FLAGS)
    EXTRA_CFLAGS += -DINIT_MODE_DRV_ENABLED -DBMIENABLE_SET -DAR600x_SD31_XXX \
                    -DATH6K_CONFIG_HIF_VIRTUAL_SCATTER
ifeq ($(ATH_AR6K_HCI_BRIDGE),yes)
    EXTRA_CFLAGS += -DSETUPHCI_ENABLED -DSETUPBTDEV_ENABLED \
                    -DATH6KL_CONFIG_GPIO_BT_RESET -DAR600x_BT_AR3001
endif
endif
ifeq ($(ATH_AR6K_HCI_PAL),yes)
    EXTRA_CFLAGS += -DSETUPHCIPAL_ENABLED
ifeq ($(ATH_AR6K_BUILTIN_HCI_PAL),no)
EXTRA_CFLAGS += -DEXPORT_HCI_PAL_INTERFACE
endif
ifeq ($(ATH_AR6K_DEBUG_HCI_PAL),yes)
EXTRA_CFLAGS += -DHCIPAL_DEBUG
endif
endif

ifeq ($(ATH_SOFTMAC_FILE_USED),yes)
    EXTRA_CFLAGS += -DSOFTMAC_FILE_USED
endif 

ifeq ($(ATH_AR6K_HCI_BRIDGE),yes)
EXTRA_CFLAGS += -I$(ATH_SRC_BASE)/miscdrv/ar3kps

EXTRA_CFLAGS += -DATH_AR6K_ENABLE_GMBOX \
                -DHCI_TRANSPORT_SDIO

ifeq ($(ATH_AR6K_BUILTIN_HCI_TRANSPORT),no)
EXTRA_CFLAGS += -DEXPORT_HCI_BRIDGE_INTERFACE
endif
endif


ifeq ($(ATH_HTC_RAW_INT_ENV),yes)
  EXTRA_CFLAGS += -DHTC_RAW_INTERFACE
endif

ifeq ($(ATH_DEBUG_DRIVER),yes)
   EXTRA_CFLAGS += -DDEBUG
endif


ifneq ($(ATH_OS_SUB_TYPE),linux_2_4)
# linux 2.6 and higher kernel module build settings
EXTRA_CFLAGS += -DKERNEL_2_6
obj-m += ar6000.o
ifeq ($(ATH_AR6K_BUILTIN_HCI_TRANSPORT),no)
obj-m += bt_hci_sdio.o
endif
-include $(ATH_SRC_BASE)/hif/$(ATH_HIF_TYPE)/Makefile
ar6000-objs += ../../htc2/AR6000/ar6k.o        \
			   ../../htc2/AR6000/ar6k_events.o \
               ../../htc2/htc_send.o       \
               ../../htc2/htc_recv.o       \
               ../../htc2/htc_services.o   \
               ../../htc2/htc.o     		  \
               ../../bmi/src/bmi.o            \
               ar6000_drv.o           			\
               ar6000_pm.o           			\
               ar6000_raw_if.o					\
               netbuf.o							\
               wireless_ext.o					\
               ioctl.o							\
               ../../miscdrv/common_drv.o		\
               ../../miscdrv/credit_dist.o		\
               ../../wmi/wmi.o                       \
               ../../reorder/rcv_aggr.o              \
               ../../wlan/src/wlan_node.o            \
               ../../wlan/src/wlan_recv_beacon.o     \
               ../../wlan/src/wlan_utils.o			 

ifeq ($(ATH_CFG80211_ENV),yes)
ar6000-objs += cfg80211.o
endif

ifneq ($(ATH_AR6K_BUILTIN_HCI_TRANSPORT),no)
ar6000-objs += hci_bridge.o
endif

ifneq ($(ATH_AR6K_BUILTIN_HCI_PAL),no)
ar6000-objs += hci_pal.o
else
obj-m += hci_pal.o
endif

ifeq ($(ATH_AR6K_HCI_BRIDGE),yes)
ar6000-objs += ../../htc2/AR6000/ar6k_gmbox.o          \
			   ../../htc2/AR6000/ar6k_gmbox_hciuart.o
ifneq ($(ATH_AR6K_BUILTIN_HCI_TRANSPORT),no)
ar6000-objs += ../../miscdrv/ar3kconfig.o				\
               ../../miscdrv/ar3kps/ar3kpsconfig.o		\
               ../../miscdrv/ar3kps/ar3kpsparser.o
else
ar6000-objs += export_hci_transport.o
endif

ifeq ($(ATH_AR6K_BUILTIN_HCI_TRANSPORT),no)
bt_hci_sdio-objs += hci_bridge.o        \
                ../../miscdrv/ar3kconfig.o				\
                ../../miscdrv/ar3kps/ar3kpsconfig.o		\
                ../../miscdrv/ar3kps/ar3kpsparser.o
endif
endif

ifeq ($(ATH_ANDROID_ENV),yes)
   ar6000-objs += ar6000_android.o
endif
endif

ifeq ($(ATH_OS_SUB_TYPE),linux_2_4)
# linux 2.4 kernel module build settings
EXTRA_CFLAGS += -DKERNEL_2_4

O_TARGET := ar6000.o
# need export line for module export
export-objs := ar6000_drv.o
-include $(ATH_SRC_BASE)/hif/$(ATH_HIF_TYPE)/Makefile
obj-y += $(ATH_SRC_BASE)/htc2/htc.o \
         $(ATH_SRC_BASE)/htc2/htc_send.o \
         $(ATH_SRC_BASE)/htc2/htc_recv.o  \
         $(ATH_SRC_BASE)/htc2/htc_services.o \
         $(ATH_SRC_BASE)/htc2/AR6000/ar6k.o  \
         $(ATH_SRC_BASE)/htc2/AR6000/ar6k_events.o  \
         $(ATH_SRC_BASE)/bmi/src/bmi.o \
         $(ATH_SRC_BASE)/wmi/wmi.o \
         $(ATH_SRC_BASE)/reorder/rcv_aggr.o \
         $(ATH_SRC_BASE)/wlan/src/wlan_node.o \
         $(ATH_SRC_BASE)/wlan/src/wlan_recv_beacon.o \
         $(ATH_SRC_BASE)/wlan/src/wlan_utils.o \
         $(ATH_SRC_BASE)/os/linux/ar6000_drv.o \
         $(ATH_SRC_BASE)/os/linux/ar6000_raw_if.o \
         $(ATH_SRC_BASE)/os/linux/wireless_ext.o \
         $(ATH_SRC_BASE)/os/linux/ioctl.o 		\
         $(ATH_SRC_BASE)/os/linux/netbuf.o     \
         $(ATH_SRC_BASE)/os/linux/hci_bridge.o     \
         $(ATH_SRC_BASE)/miscdrv/common_drv.o   \
         $(ATH_SRC_BASE)/miscdrv/credit_dist.o

obj-m += htc.o \
         htc_send.o \
         htc_recv.o \
         htc_services.o  \
         ar6k_events.o \
         ar6k.o \
         ar6k_gmbox.o  \
         ar6k_gmbox_hciuart.o \
         hif.o  \
         bmi.o  \
         wmi.o  \
         wlan_node.o \
         wlan_recv_beacon.o \
         wlan_utils.o \
         ar6000_drv.o \
         ar6000_raw_if.o \
         common_drv.o \
         credit_dist.o \
         netbuf.o     \
         wireless_ext.o \
         rcv_aggr.o        \
         ioctl.o        \
         hci_bridge.o    \
         ar6000.o
              
include $(ATH_LINUXPATH)/Rules.make
endif

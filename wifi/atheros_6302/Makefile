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
EXTRA_CFLAGS += -Idrivers/amlogic/wifi/atheros_6302/include/
EXTRA_CFLAGS += -Idrivers/amlogic/wifi/atheros_6302/include/AR6002/hw4.0
EXTRA_CFLAGS += -Idrivers/amlogic/wifi/atheros_6302/wlan/include
EXTRA_CFLAGS += -Idrivers/amlogic/wifi/atheros_6302/os/linux/include
EXTRA_CFLAGS += -Idrivers/amlogic/wifi/atheros_6302/bmi/include
EXTRA_CFLAGS += -Idrivers/amlogic/wifi/atheros_6302/hif/sdio/linux_sdio/include

EXTRA_CFLAGS += -DLINUX -D__KERNEL__ \
                -DTCMD -DSEND_EVENT_TO_APP -DUSER_KEYS \
                -DNO_SYNC_FLUSH  -DHTC_EP_STAT_PROFILING -DWLAN_HEADERS\
                -DATH_AR6K_11N_SUPPORT -DWAPI_ENABLE #\
                -DMULTIPLE_FRAMES_PER_INTERRUPT -DAR6000REV2 \
                -DBLOCK_TX_PATH_FLAG -DCHECKSUM_OFFLOAD\







    EXTRA_CFLAGS += -DANDROID_ENV -D__linux__
    EXTRA_CFLAGS += -DINIT_MODE_DRV_ENABLED -DBMIENABLE_SET -DAR600x_SD31_XXX \
                    -DATH6K_CONFIG_HIF_VIRTUAL_SCATTER







#EXTRA_CFLAGS += -DEXPORT_HCI_PAL_INTERFACE maybe need















#EXTRA_CFLAGS += -DEXPORT_HCI_BRIDGE_INTERFACE maybe need








# linux 2.6 and higher kernel module build settings
EXTRA_CFLAGS += -DKERNEL_2_6
obj-$(CONFIG_SDIO_ATHEROS_6302_WIFI) += ar6000.o

#obj-$(CONFIG_SDIO_ATHEROS_6302_WIFI) += bt_hci_sdio.o maybe need


ar6000-objs += htc2/AR6000/ar6k.o        \
			   			htc2/AR6000/ar6k_events.o \
               htc2/htc_send.o       \
               htc2/htc_recv.o       \
               htc2/htc_services.o   \
               htc2/htc.o     		  \
               bmi/src/bmi.o            \
               os/linux/ar6000_drv.o           			\
               os/linux/ar6000_pm.o           			\
               os/linux/ar6000_raw_if.o					\
               os/linux/netbuf.o							\
               os/linux/wireless_ext.o					\
               os/linux/ioctl.o							\
               miscdrv/common_drv.o		\
               miscdrv/credit_dist.o		\
               wmi/wmi.o                       \
               reorder/rcv_aggr.o              \
               wlan/src/wlan_node.o            \
               wlan/src/wlan_recv_beacon.o     \
               wlan/src/wlan_utils.o		\
							hif/sdio/linux_sdio/src/hif.o \
			   			hif/sdio/linux_sdio/src/hif_scatter.o	\
			   			os/linux/hci_bridge.o \
			   			os/linux/hci_pal.o \
							os/linux/ar6000_android.o















#ar6000-objs += export_hci_transport.o maybe need



#bt_hci_sdio-objs += hci_bridge.o        \ maybe need
#                miscdrv/ar3kconfig.o				\
#                miscdrv/ar3kps/ar3kpsconfig.o		\
#                miscdrv/ar3kps/ar3kpsparser.o


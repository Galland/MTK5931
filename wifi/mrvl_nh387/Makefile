# File: Makefile
#
# Copyright (C) 2008-2010, Marvell International Ltd. 
#
# This software file (the "File") is distributed by Marvell International 
# Ltd. under the terms of the GNU General Public License Version 2, June 1991 
# (the "License").  You may use, redistribute and/or modify this File in 
# accordance with the terms and conditions of the License, a copy of which 
# is available by writing to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
# worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
#
# A copy of the GPL is available in file gpl-2.0.txt accompanying in this 
# deliverables.
#
# THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE 
# IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE 
# ARE EXPRESSLY DISCLAIMED.  The License provides additional details about 
# this warranty disclaimer.

CC=		$(CROSS_COMPILE)gcc
LD=		$(CROSS_COMPILE)ld
BACKUP=		/root/backup
YMD=		`date +%Y%m%d%H%M`

#############################################################################
# Configuration Options
#############################################################################


#############################################################################
# Select Platform Tools
#############################################################################


EXTRA_CFLAGS += -Idrivers/amlogic/wifi/mrvl_nh387/mlan
EXTRA_CFLAGS += -DLINUX



#############################################################################
# Compiler Flags
#############################################################################

	EXTRA_CFLAGS += -DFPNUM='"57"'

ifeq ($(CONFIG_MRVLNH387_DEBUG),"1")
	EXTRA_CFLAGS += -DDEBUG_LEVEL1
endif

ifeq ($(CONFIG_MRVLNH387_DEBUG),"2")
	EXTRA_CFLAGS += -DDEBUG_LEVEL1
	EXTRA_CFLAGS += -DDEBUG_LEVEL2
	DBG=	-dbg
endif

ifdef CONFIG_MRVLNH387_PROC_DEBUG
	EXTRA_CFLAGS += -DPROC_DEBUG
endif

ifdef CONFIG_MRVLNH387_STA_SUPPORT
	EXTRA_CFLAGS += -DSTA_SUPPORT
ifdef CONFIG_MRVLNH387_REASSOCIATION
	EXTRA_CFLAGS += -DREASSOCIATION
endif
endif

ifdef CONFIG_MRVLNH387_UAP_SUPPORT
	EXTRA_CFLAGS += -DUAP_SUPPORT
endif

ifdef CONFIG_MRVLNH387_MFG_CMD_SUPPORT
	EXTRA_CFLAGS += -DMFG_CMD_SUPPORT
endif

ifdef CONFIG_MRVLNH387_BIG_ENDIAN
	EXTRA_CFLAGS += -DBIG_ENDIAN_SUPPORT
endif

ifdef CONFIG_MRVLNH387_SDIO_MULTI_PORT_TX_AGGR
	EXTRA_CFLAGS += -DSDIO_MULTI_PORT_TX_AGGR
endif

ifdef CONFIG_MRVLNH387_SDIO_MULTI_PORT_RX_AGGR
	EXTRA_CFLAGS += -DSDIO_MULTI_PORT_RX_AGGR
endif


ifdef CONFIG_MRVLNH387_SDIO_SUSPEND_RESUME
	EXTRA_CFLAGS += -DSDIO_SUSPEND_RESUME
endif

#############################################################################
# Make Targets
#############################################################################

obj-$(CONFIG_SDIO_MARVELL_NH387_WIFI) += mlan.o
mlan-objs += mlinux/moal_main.o \
           mlinux/moal_ioctl.o \
           mlinux/moal_shim.o \
           mlinux/moal_sdio_mmc.o
ifdef CONFIG_MRVLNH387_STA_SUPPORT
mlan-objs += mlinux/moal_priv.o \
             mlinux/moal_wext.o
endif
ifdef CONFIG_MRVLNH387_UAP_SUPPORT
mlan-objs += mlinux/moal_uap.o \
             mlinux/moal_uap_priv.o \
             mlinux/moal_uap_wext.o
endif
ifdef CONFIG_PROC_FS
mlan-objs += mlinux/moal_proc.o
ifdef CONFIG_MRVLNH387_PROC_DEBUG
mlan-objs += mlinux/moal_debug.o
endif
endif

mlan-objs += mlan/mlan_shim.o mlan/mlan_init.o \
           mlan/mlan_txrx.o \
           mlan/mlan_cmdevt.o mlan/mlan_misc.o \
           mlan/mlan_module.o \
           mlan/mlan_wmm.o \
           mlan/mlan_sdio.o \
           mlan/mlan_11n_aggr.o \
           mlan/mlan_11n_rxreorder.o \
           mlan/mlan_11n.o
ifdef CONFIG_MRVLNH387_STA_SUPPORT
mlan-objs += mlan/mlan_11d.o \
             mlan/mlan_11h.o \
             mlan/mlan_meas.o \
             mlan/mlan_cfp.o \
             mlan/mlan_scan.o \
             mlan/mlan_sta_ioctl.o \
             mlan/mlan_sta_rx.o \
             mlan/mlan_sta_tx.o \
             mlan/mlan_sta_event.o \
             mlan/mlan_sta_cmd.o \
             mlan/mlan_sta_cmdresp.o \
             mlan/mlan_join.o
endif
ifdef CONFIG_MRVLNH387_UAP_SUPPORT
mlan-objs += mlan/mlan_uap_ioctl.o \
             mlan/mlan_uap_cmdevent.o \
             mlan/mlan_uap_txrx.o
endif
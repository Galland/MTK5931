# bcm4329
DHDCFLAGS = -DLINUX -DBCMDRIVER -DBCMDONGLEHOST -DDHDTHREAD -DBCMWPA2     \
	-DUNRELEASEDCHIP -DDHD_GPL -DDHD_SCHED -DDHD_DEBUG -DSDTEST -DBDC -DTOE \
	-DDHD_BCMEVENTS -DSHOW_EVENTS -DBCMSDIO -DSDIO_ISR_THREAD -DBCMLXSDMMC \
	-DBCMPLATFORM_BUS -DOEM_ANDROID -DEMBEDDED_PLATFORM -DARP_OFFLOAD_SUPPORT \
	-DPKT_FILTER_SUPPORT -DCHROMIUMOS_COMPAT_WIRELESS -DCONFIG_HAS_EARLYSUSPEND -DBCMWAPI_WPI\
	-Idrivers/amlogic/wifi/bcm4329/include/ -Idrivers/amlogic/wifi/bcm4329/dongle/ \
	-Idrivers/amlogic/wifi/bcm4329/dhd/sys/ -Idrivers/amlogic/wifi/bcm4329/wl/sys/ \
	-Idrivers/amlogic/wifi/bcm4329/shared/

#DHDOFILES = dhd_linux.o linux_osl.o bcmutils.o dhd_common.o dhd_custom_gpio.o \
#	wl_iw.o siutils.o sbutils.o aiutils.o hndpmu.o bcmwifi.o dhd_sdio.o   \
#	dhd_linux_sched.o dhd_cdc.o bcmsdh_sdmmc.o bcmsdh.o bcmsdh_linux.o    \
#	bcmsdh_sdmmc_linux.o

#DHDCFLAGS += -DCSCAN -DSOFTAP

DHDOFILES = dhd/sys/dhd_linux.o \
            shared/linux_osl.o \
            shared/bcmutils.o \
            dhd/sys/dhd_common.o \
            dhd/sys/dhd_custom_gpio.o \
            shared/sbutils.o \
            shared/siutils.o \
            shared/aiutils.o \
            shared/hndpmu.o \
            shared/bcmwifi.o \
            dhd/sys/dhd_linux_sched.o \
            dhd/sys/dhd_cdc.o \
            dhd/sys/dhd_sdio.o \
            bcmsdio/sys/bcmsdh_sdmmc.o \
            bcmsdio/sys/bcmsdh.o \
            bcmsdio/sys/bcmsdh_linux.o \
            bcmsdio/sys/bcmsdh_sdmmc_linux.o \
            wl/sys/wl_iw.o 


obj-$(CONFIG_SAMSUNG_WIFI_4329) += bcm4329.o
bcm4329-objs += $(DHDOFILES)

EXTRA_CFLAGS = $(DHDCFLAGS)
EXTRA_LDFLAGS += --strip-debug

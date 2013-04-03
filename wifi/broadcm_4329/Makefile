# bcm4329
DHDCFLAGS = -DLINUX -DBCMDRIVER -DBCMDONGLEHOST -DDHDTHREAD -DBCMWPA2     \
	-DUNRELEASEDCHIP -DDHD_GPL -DDHD_SCHED -DDHD_DEBUG -DSDTEST -DBDC -DTOE \
	-DDHD_BCMEVENTS -DSHOW_EVENTS -DBCMSDIO -DSDIO_ISR_THREAD -DBCMLXSDMMC \
	-DBCMPLATFORM_BUS -DOEM_ANDROID -DEMBEDDED_PLATFORM -DARP_OFFLOAD_SUPPORT \
	-DPKT_FILTER_SUPPORT -DCHROMIUMOS_COMPAT_WIRELESS -DCONFIG_HAS_EARLYSUSPEND -DBCMWAPI_WPI\
	-Idrivers/amlogic/wifi/broadcm_4329/include/

DHDCFLAGS += -DCSCAN

#DHDCFLAGS += -DENABLE_DEEP_SLEEP

DHDOFILES = dhd_linux.o linux_osl.o bcmutils.o dhd_common.o dhd_custom_gpio.o \
	wl_iw.o siutils.o sbutils.o aiutils.o hndpmu.o bcmwifi.o dhd_sdio.o   \
	dhd_linux_sched.o dhd_cdc.o bcmsdh_sdmmc.o bcmsdh.o bcmsdh_linux.o    \
	bcmsdh_sdmmc_linux.o

obj-$(CONFIG_SDIO_DHD_CDC_WIFI_4329) += bcm4329.o
obj-$(CONFIG_SDIO_DHD_CDC_WIFI_4319) += bcm4329.o
bcm4329-objs += $(DHDOFILES)
EXTRA_CFLAGS = $(DHDCFLAGS)
EXTRA_LDFLAGS += --strip-debug

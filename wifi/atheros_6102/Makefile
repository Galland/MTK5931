EXTRA_CFLAGS += -Idrivers/amlogic/wifi/atheros_6102/include
EXTRA_CFLAGS += -Idrivers/amlogic/wifi/atheros_6102/include/AR6002
EXTRA_CFLAGS += -Idrivers/amlogic/wifi/atheros_6102/os/linux/include
EXTRA_CFLAGS += -Idrivers/amlogic/wifi/atheros_6102/wlan/include
EXTRA_CFLAGS += -Idrivers/amlogic/wifi/atheros_6102/bmi/include
EXTRA_CFLAGS += -Idrivers/amlogic/wifi/atheros_6102/hif/sdio/linux_sdio/include

EXTRA_CFLAGS += -DLINUX -D__KERNEL__ \
                -DTCMD -DSEND_EVENT_TO_APP -DUSER_KEYS \
                -DNO_SYNC_FLUSH -DWAPI_ENABLE #\
                -DMULTIPLE_FRAMES_PER_INTERRUPT -DAR6000REV2 \
                -DBLOCK_TX_PATH_FLAG \
                
EXTRA_CFLAGS +=  -DANDROID_ENV \
                 -D__linux__ -Wno-error \
                 -DEEPROM_FILE_USED \
                 -DKERNEL_2_6 \
                 -DSDIO
                 
#EXTRA_CFLAGS += -DDEBUG
                 
obj-$(CONFIG_SDIO_ATHEROS_6102_WIFI) += ar6102.o
ar6102-objs += htc2/AR6000/ar6k.o        \
               htc2/AR6000/ar6k_events.o \
               htc2/htc_send.o       \
               htc2/htc_recv.o       \
               htc2/htc_services.o   \
               htc2/htc.o            \
               bmi/src/bmi.o         \
               os/linux/ar6000_drv.o \
               os/linux/ar6000_raw_if.o \
               os/linux/netbuf.o        \
               os/linux/wireless_ext.o  \
               os/linux/ioctl.o         \
               os/linux/engine.o        \
               miscdrv/common_drv.o     \
               miscdrv/credit_dist.o    \
               wmi/wmi.o                \
               wlan/src/wlan_node.o     \
               wlan/src/wlan_recv_beacon.o \
               wlan/src/wlan_utils.o       \
               os/linux/eeprom.o           \
               hif/sdio/linux_sdio/src/hif.o
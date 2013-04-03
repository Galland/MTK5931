如何把4329改成module模式：

1，外面1层文件夹里kconfig把bool改成tristate，这样make menuconfig里
device driver---->amlogic device driver--->AMLOGIC WIFI SUPPORT--->最下面的4329可以选成"M"
它下面的3个都不选;

2,修改board-8726m-refb03.c

4，make clean后make uImage ;

5,make modules SUBDIRS=drivers/amlogic/wifi/broadcm_4329_new/

1)在drivers/amlogic/wifi/broadcm_4329/里生产dhd.ko
copy到\\10.28.8.14\nfsroot\sandy.luo\rootfs_out_03\target\product\b03ref\system\lib
和\\10.28.8.14\nfsroot\sandy.luo\rootfs\hardware\amlogic\wifi\dhd
2)把spike给的sdio-g-cdc-full11n-reclaim-roml-wme-idsup.bin和nvram.txt
copy到\\10.28.8.14\nfsroot\sandy.luo\rootfs_out_03\target\product\b03ref\system\etc里
和\\10.28.8.14\nfsroot\sandy.luo\rootfs\hardware\amlogic\wifi\dhd

6,init.rc的最后增加
    chmod 0777 /system/FCC_CE_TOOL_013

service FCC_CE_TOOL_013 /system/FCC_CE_TOOL_013

184行改成BT:
    chmod 0777 /sys/class/rfkill/rfkill0/state
    write /sys/class/rfkill/rfkill0/state 1


注意WIFI是连不上的，能连上说明sdio-g-cdc-full11n-reclaim-roml-wme-idsup.bin和nvram.txt不对，
这两个文件不能是自己编译的.



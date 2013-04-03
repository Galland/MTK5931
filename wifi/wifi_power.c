#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <plat/wifi_power.h>
#include <linux/cdev.h>
#include <linux/fs.h>


#define WIFI_POWER_MODULE_NAME   "wifi_power"
#define WIFI_POWER_DRIVER_NAME "wifi_power"
#define WIFI_POWER_DEVICE_NAME   "wifi_power"
#define WIFI_POWER_CLASS_NAME   "wifi_power"

static dev_t wifi_power_devno;
static struct cdev *wifi_power_cdev = NULL;
static struct device *devp=NULL;

static int wifi_power_probe(struct platform_device *pdev);
static int wifi_power_remove(struct platform_device *pdev);
static int  wifi_power_open(struct inode *inode,struct file *file);
static int  wifi_power_release(struct inode *inode,struct file *file);

static struct platform_driver wifi_power_driver = {
    .probe = wifi_power_probe,
    .remove = wifi_power_remove,
    .driver = {
    .name = WIFI_POWER_DRIVER_NAME,
    .owner = THIS_MODULE,
    },
};

static const struct file_operations wifi_power_fops = {
    .open	= wifi_power_open,
    .release	= wifi_power_release,
};

static struct class wifi_power_class = {
    .name = WIFI_POWER_CLASS_NAME,
    .owner = THIS_MODULE,
};

static int  wifi_power_open(struct inode *inode,struct file *file)
{
    int ret = 0;
    struct cdev * cdevp = inode->i_cdev;
    file->private_data = cdevp;
    return ret;
}

static int  wifi_power_release(struct inode *inode,struct file *file)
{
    int ret = 0;
    return ret;
}

int wifi_set_power(int val)
{
    struct wifi_power_platform_data *pdata = NULL;
    pdata = (struct wifi_power_platform_data*)devp->platform_data;
    if(pdata == NULL){
        printk("%s platform data is required!\n",__FUNCTION__);
        return -1;
    }
    if(pdata->set_power){
        pdata->set_power(val);
    }
    else{
        printk( "%s:%s No wifi set_power !\n",WIFI_POWER_MODULE_NAME,__FUNCTION__);
        return -EINVAL;
    }
    return 0;
}

EXPORT_SYMBOL(wifi_set_power);

int wifi_set_reset(int val)
{
    struct wifi_power_platform_data *pdata = NULL;
    pdata = (struct wifi_power_platform_data*)devp->platform_data;
    if(pdata == NULL){
        printk("%s platform data is required!\n",__FUNCTION__);
        return -1;
    }
    if(pdata->set_reset){
        pdata->set_reset(val);
    }
    else{
        printk( "%s:%s No wifi set_reset !\n",WIFI_POWER_MODULE_NAME,__FUNCTION__);
        return -EINVAL;
    }
    return 0;
}

EXPORT_SYMBOL(wifi_set_reset);

int wifi_set_carddetect(int val)
{
    struct wifi_power_platform_data *pdata = NULL;
    pdata = (struct wifi_power_platform_data*)devp->platform_data;
    if(pdata == NULL){
        printk("%s platform data is required!\n",__FUNCTION__);
        return -1;
    }
    if(pdata->set_carddetect){
        pdata->set_carddetect(val);
    }
    else{
        printk( "%s:%s No wifi set_carddetect !\n",WIFI_POWER_MODULE_NAME,__FUNCTION__);
        return -EINVAL;
    }
    return 0;
}

EXPORT_SYMBOL(wifi_set_carddetect);

void *wifi_mem_prealloc(int section, unsigned long size)
{
    struct wifi_power_platform_data *pdata = NULL;
    void * ret;
    pdata = (struct wifi_power_platform_data*)devp->platform_data;
    if(pdata == NULL){
        printk("%s platform data is required!\n",__FUNCTION__);
        return NULL;
    }
    if(pdata->mem_prealloc){
        ret = pdata->mem_prealloc(section,size);
    }
    else{
        printk( "%s:%s No wifi mem_prealloc !\n",WIFI_POWER_MODULE_NAME,__FUNCTION__);
        return NULL;
    }
    return ret;
}

EXPORT_SYMBOL(wifi_mem_prealloc);

int wifi_get_mac_addr(unsigned char *buf)
{
    struct wifi_power_platform_data *pdata = NULL;
    pdata = (struct wifi_power_platform_data*)devp->platform_data;
    if(pdata == NULL){
        printk("%s platform data is required!\n",__FUNCTION__);
        return -1;
    }
    if(pdata->get_mac_addr){
        pdata->get_mac_addr(buf);
    }
    else{
        printk( "%s:%s No wifi get_mac_addr !\n",WIFI_POWER_MODULE_NAME,__FUNCTION__);
        return -EINVAL;
    }
    return 0;
}

EXPORT_SYMBOL(wifi_get_mac_addr);

void *wifi_get_country_code(char *ccode)
{
    struct wifi_power_platform_data *pdata = NULL;
    void * ret;
    pdata = (struct wifi_power_platform_data*)devp->platform_data;
    if(pdata == NULL){
        printk("%s platform data is required!\n",__FUNCTION__);
        return NULL;
    }
    if(pdata->get_country_code){
        ret = pdata->get_country_code(ccode);
    }
    else{
        printk( "%s:%s No wifi get_mac_addr !\n",WIFI_POWER_MODULE_NAME,__FUNCTION__);
        return NULL;
    }
    return ret;
}

EXPORT_SYMBOL(wifi_get_country_code);


int wifi_usb_set_power(int val)
{
    struct wifi_power_platform_data *pdata = NULL;
    if(devp==NULL)
        return -1;
    pdata = (struct wifi_power_platform_data*)devp->platform_data;
    if(pdata == NULL){
        printk("%s platform data is required!\n",__FUNCTION__);
        return -1;
    }
    if(pdata->usb_set_power){
        pdata->usb_set_power(val);
    }
    else{
        printk( "%s:%s No wifi usb_set_power !\n",WIFI_POWER_MODULE_NAME,__FUNCTION__);
        return -EINVAL;
    }
    return 0;
}

EXPORT_SYMBOL(wifi_usb_set_power);


static int wifi_power_probe(struct platform_device *pdev)
{
    int ret;
    struct wifi_power_platform_data *pdata = pdev->dev.platform_data;
    if (!pdata) {
        dev_err(&pdev->dev, "platform data is required!\n");
        ret = -EINVAL;
        goto out;
    }
    ret = alloc_chrdev_region(&wifi_power_devno, 0, 1, WIFI_POWER_DRIVER_NAME);
    if (ret < 0) {
        printk(KERN_ERR "%s:%s failed to allocate major number\n",WIFI_POWER_MODULE_NAME,__FUNCTION__);
        ret = -ENODEV;
        goto out;
    }
    ret = class_register(&wifi_power_class);
    if (ret < 0) {
        printk(KERN_ERR "%s:%s  failed to register class\n",WIFI_POWER_MODULE_NAME,__FUNCTION__);
        goto error1;
    }
    wifi_power_cdev = cdev_alloc();
    if(!wifi_power_cdev){
        printk(KERN_ERR "%s:%s failed to allocate memory\n",WIFI_POWER_MODULE_NAME,__FUNCTION__);
        goto error2;
    }
    cdev_init(wifi_power_cdev,&wifi_power_fops);
    wifi_power_cdev->owner = THIS_MODULE;
    ret = cdev_add(wifi_power_cdev,wifi_power_devno,1);
    if(ret){
        printk(KERN_ERR "%s:%s failed to add device\n",WIFI_POWER_MODULE_NAME,__FUNCTION__);
        goto error3;
    }
    devp = device_create(&wifi_power_class,NULL,wifi_power_devno,NULL,WIFI_POWER_DEVICE_NAME);
    if(IS_ERR(devp)){
        printk(KERN_ERR "%s:%s failed to create device node\n",WIFI_POWER_MODULE_NAME,__FUNCTION__);
        ret = PTR_ERR(devp);
        goto error3;
    }
    devp->platform_data = pdata;
    return 0;
error3:
    cdev_del(wifi_power_cdev);
error2:
    class_unregister(&wifi_power_class);
error1:
    unregister_chrdev_region(wifi_power_devno,1);
out:
    return ret;
}

static int wifi_power_remove(struct platform_device *pdev)
{
    unregister_chrdev_region(wifi_power_devno,1);
    class_unregister(&wifi_power_class);
    device_destroy(NULL, wifi_power_devno);
    cdev_del(wifi_power_cdev);
    return 0;
}

static int __init init_wifi(void)
{
    int ret = -1;
    ret = platform_driver_register(&wifi_power_driver);
    if (ret != 0) {
        printk(KERN_ERR "failed to register wifi power module, error %d\n", ret);
        return -ENODEV;
    }
    return ret;
}

module_init(init_wifi);

static void __exit unload_wifi(void)
{
    platform_driver_unregister(&wifi_power_driver);
}
module_exit(unload_wifi);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AMLOGIC");
MODULE_DESCRIPTION("WIFI power driver");



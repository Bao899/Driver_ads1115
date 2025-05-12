#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/delay.h>

#define DRIVER_NAME     "ads1115_driver"
#define DEVICE_NAME     "ads1115"
#define CLASS_NAME      "ads"

#define ADS1115_ADDR            0x48
#define ADS1115_REG_CONVERSION  0x00
#define ADS1115_REG_CONFIG      0x01

// IOCTL
#define ADS1115_MAGIC           'a'
#define ADS1115_READ_CHANNEL_0  _IOR(ADS1115_MAGIC, 1, int)

static struct i2c_client *ads1115_client;
static struct class *ads_class;
static struct device *ads_device;
static int major;

static int ads1115_start_conversion(struct i2c_client *client)
{
    u8 config[3];

    config[0] = ADS1115_REG_CONFIG;
    config[1] = 0xC2; // OS=1 (start single), MUX=000 (AIN0), PGA=±4.096V, MODE=1
    config[2] = 0x83; // DR=128SPS, disable comparator

    return i2c_master_send(client, config, 3);
}

static int ads1115_read_data(struct i2c_client *client)
{
    u8 reg = ADS1115_REG_CONVERSION;
    u8 data[2];
    s16 value;

    i2c_master_send(client, &reg, 1);
    msleep(10); // đợi ADC chuyển đổi

    if (i2c_master_recv(client, data, 2) != 2) {
        return -EIO;
    }

    value = (data[0] << 8) | data[1];
    return value;
}

// IOCTL
static long ads1115_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int val;

    switch (cmd) {
        case ADS1115_READ_CHANNEL_0:
            ads1115_start_conversion(ads1115_client);
            val = ads1115_read_data(ads1115_client);
            if (val < 0) return val;
            if (copy_to_user((int __user *)arg, &val, sizeof(val)))
                return -EFAULT;
            return 0;
        default:
            return -EINVAL;
    }
}

// File operations
static int ads1115_open(struct inode *inode, struct file *file) {
    return 0;
}

static int ads1115_release(struct inode *inode, struct file *file) {
    return 0;
}

static struct file_operations fops = {
    .open = ads1115_open,
    .release = ads1115_release,
    .unlocked_ioctl = ads1115_ioctl,
};

// Probe & remove
static int ads1115_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret __maybe_unused;

    ads1115_client = client;

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) return major;

    ads_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(ads_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(ads_class);
    }

    ads_device = device_create(ads_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(ads_device)) {
        class_destroy(ads_class);
        unregister_chrdev(major, DEVICE_NAME);
        return PTR_ERR(ads_device);
    }

    printk(KERN_INFO "ADS1115 driver loaded\n");
    return 0;
}

static void ads1115_remove(struct i2c_client *client)
{
    device_destroy(ads_class, MKDEV(major, 0));
    class_unregister(ads_class);
    class_destroy(ads_class);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "ADS1115 driver removed\n");
}

// Device ID table
static const struct i2c_device_id ads1115_id[] = {
    { "ads1115", 0 },
    { }
};


MODULE_DEVICE_TABLE(i2c, ads1115_id);

// I2C driver struct
static struct i2c_driver ads1115_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe = ads1115_probe,
    .remove = ads1115_remove,
    .id_table = ads1115_id,
};

// Init & exit
static int __init ads1115_init(void) {
    printk(KERN_INFO "Initializing ADS1115 driver\n");
    return i2c_add_driver(&ads1115_driver);
}

static void __exit ads1115_exit(void) {
    printk(KERN_INFO "Exiting ADS1115 driver\n");
    i2c_del_driver(&ads1115_driver);
}

module_init(ads1115_init);
module_exit(ads1115_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YOUR NAME");
MODULE_DESCRIPTION("Simple ADS1115 I2C Driver with IOCTL");


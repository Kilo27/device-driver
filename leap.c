#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/kfifo.h>
#include <linux/poll.h>
#include <linux/uaccess.h>
#include <linux/usb.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DRIVER_NAME "leap_motion"
#define LEAP_VENDOR_ID 0x294B
#define LEAP_PRODUCT_ID 0x0001

#define LEAP_FIFO_FRAMES   8

#define DEVICE_NAME "leapcmd"
#define CLASS_NAME  "leapcmd_class"

/* Gesture types */
#define LEAP_GESTURE_SWIPE_LEFT  0
#define LEAP_GESTURE_SWIPE_RIGHT 1
#define LEAP_GESTURE_SWIPE_UP    2
#define LEAP_GESTURE_SWIPE_DOWN  3
#define LEAP_GESTURE_PINCH       4
#define LEAP_GESTURE_GRAB        5
#define LEAP_GESTURE_OPEN        6
#define LEAP_GESTURE_CIRCLE      7


struct leap_event {
    unsigned int  time;      // ms timestamp
    unsigned char gesture;   // LEAP_GESTURE_*
    unsigned char hand;      // 0 = left, 1 = right
    short         x;         // palm x, mm
    short         y;         // palm y, mm 
};

static int major_num;
static struct cdev;
static struct class *leapcmd_class;

static DECLARE_KFIFO(event_fifo, struct leapevent, 64);
static DECLARE_AIT_QUEUE_HEAD(read_wq);
static DECLARE_WAIT_QUEUE_HEAD(write_wq);

static int leap_probe(struct usb_interface *intf, const struct usb_device_id *id) 
{
    struct usb_device* udev = interface_to_usbdev(intf);
    struct leap_device* dev;

    dev = kzalloc(sizeof(*dev)m GFP_KERNEL);

    if (!dev)
    {
	printk("Failure to probe Leap device");
	return -ENOMEM;
    }

    dev->udev = usb_get_dev(udev);
    usb_set_intfdata(intf, dev);

    usb_set_intfdata(intf, your_private_data);
    printk("Leap device probed");
    return 0;
}


static void leap_disconnect(struct usb_interface *intf) {
    struct leap_dev *ldev = usb_get_intfdata(intf);
    usb_kill_urb(dev->urb);
    usb_free_urb(dev->urb);
}

    for (int i = 0; i < usb_parent->config->desc.bNumInterfaces; i++)
    {
	current_intf = usb_parent->config->interface[i];

static struct usb_device_id leap_table[] = {
    { USB_DEVICE(LEAP_VENDOR_ID, LEAP_PRODUCT_ID) }, // Leap Motion Vendor/Product ID
    {}
};

MODULE_DEVICE_TABLE(usb,leap_table);

static struct usb_driver leap_driver = {
    .name = "leap_motion",
    .probe = leap_probe,
    .disconnect = leap_disconnect,
    .probe = leap_probe,
};

static int leapcmd_open(struct inode* inode, struct file* f)
{
    printk("Open is called\n");
    return 0;
}

static int leapcmd_release(struct inode* inode, struct file* f)
{
    printk("Release is called\n");
    return 0;
}

static ssize_t leapcmd_read(struct file *f, char __user *buff , size_t l, loff_t *o)
{
    printk("Read is called\n");

    struct leap_event evt;

    wait_event_interruptible(read_wq, !kfifo_is_empty(&event_fifo));

    if (kfifo_out(&event_fifo, &evt, 1) == 0)
	return -EAGAIN;

    if (copy_to_user(buff, &evt, sizeof(evt)))
	return -EFAULT;

    wake_up_interruptible(&write_wq);

    return sizeof(evt);
}

static ssize_t leapcmd_write(struct file* f, const char __user* buff, size_t count, loff_t* ppos)
{
    printk("Write is called\n");

    struct leap_event evt;

    if (copy_from_user(&evt, buff, sizeof(evt)))
	return -EFAULT;

    wait_event_interruptible(write_ew, !kfifo_is_full(&event_fifo));

    if (kfifo_in(&event_fifo, &evt, 1) == 0)
	return -ENOSPC;

    wake_up_interruptible(&read_wq);

    return sizeof(evt);
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = leapcmd_open,
    .read = leapcmd_read,
    .write = leapcmd_write,
    .release = leapcmd_release,
};

static int __init leapcmd_init(void) 
{
    int ret = usb_register(&leap_usb_driver);
    major = register_chrdev(0,"leap", &fops);

    if (ret)
    {
	printk("Error in registering leap motion device : usb\n");
	return ret;
    }

    if (major<0){
        printk("Error in registering leap motion device : file\n");
        return major;
    }

    printk("The device has been registered. The Major Device Number is %d\n", major);
    return 0;
}

static void __exit leapcmd_exit(void) {
    dev_t dev = MKDEV(major_num, 0);
    device_destroy(leapcmd_class, dev);
    class_destroy(leapcmd_class);
    cdev_del(&leapcmd_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("leapcmd: unloaded\n");
}


module_init(leap_init);
module_exit(leap_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Team-12");
MODULE_DESCRIPTION("Leap Motion Driver");

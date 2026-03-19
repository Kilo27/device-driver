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

static struct usb_driver leap_usb_driver;

struct leap_dev {
  //Found this online, seems to be standard for linux usb devices using the interface_to_usbdev function - KJ
  struct usb_device   *udev;
  struct usb_interface *intf;

  struct urb          *urb;
  u8                  *urb_buf;
  dma_addr_t           urb_dma;
  u8                   ep_addr;
  int                  ep_interval;
 
  spinlock_t           fifo_lock;
  struct leap_frame    fifo[LEAP_FIFO_FRAMES];
  unsigned int         fifo_head;
  unsigned int         fifo_tail;
  unsigned int         fifo_count;
 
  wait_queue_head_t    wait;
 
  struct cdev          cdev;
  int                  minor;
 
  struct mutex         io_mutex;
  bool                 disconnected;
 
  int                  open_count;
 
  unsigned long        frames_received;
  unsigned long        frames_dropped;
  unsigned long        urb_errors;
};


static int leap_probe(struct usb_interface *intf, const struct usb_device_id *id) {
    struct usb_device *udev = interface_to_usbdev(intf);
    usb_set_intfdata(intf, your_private_data);
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

static int leap_open(struct inode* inode, struct file* f)
{
    printk("Open is called\n");
    return 0;
}

static int leap_release(struct inode* inode, struct file* f)
{
    printk("Release is called\n");
    return 0;
}

static ssize_t leap_read(struct file *f, char __user *buff , size_t l, loff_t *o)
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

static ssize_t leap_write(struct file* f, const char __user* buff, size_t count, loff_t* ppos)
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
    .open = leap_open,
    .read = leap_read,
    .write = leap_write,
    .release = leap_release,
};

static int __init leap_init(void) 
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

static void __exit leap_exit(void) {
    usb_deregister(&leap_usb_driver);
    unregister_chrdev(major, "leap");
    printk("The leap device has been deregistered\n");
    
}


module_init(leap_init);
module_exit(leap_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Team-12");
MODULE_DESCRIPTION("Leap Motion Driver");

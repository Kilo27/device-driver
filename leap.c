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
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#define DRIVER_NAME     "leap_motion"
#define LEAP_VENDOR_ID  0x294B
#define LEAP_PRODUCT_ID 0x0001
#define LEAP_FIFO_FRAMES 8
#define DEVICE_NAME "leapcmd"
#define CLASS_NAME  "leapcmd_class"
static const char *gesture_names[] = {
    "swipe_left", "swipe_right", "swipe_up", "swipe_down",
    "pinch",      "grab",        "open",     "circle"
};
#define NUM_GESTURES ARRAY_SIZE(gesture_names)
struct leap_event {
    unsigned int  time;     
    unsigned char gesture;  
    unsigned char hand;     
    short         x;        
    short         y;        
};
static struct cdev   leapcmd_cdev;
static int           major_num;
static struct class *leapcmd_class;
static DECLARE_KFIFO(event_fifo, struct leap_event, 64);
static DECLARE_WAIT_QUEUE_HEAD(read_wq);
static DECLARE_WAIT_QUEUE_HEAD(write_wq);
static int leap_probe(struct usb_interface *intf,
                      const struct usb_device_id *id)
{
    struct usb_device *udev = interface_to_usbdev(intf);
    struct leap_dev   *dev;
    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev) {
        pr_err(DRIVER_NAME ": failed to allocate device struct\n");
        return -ENOMEM;
    }
    dev->udev = usb_get_dev(udev);
    usb_set_intfdata(intf, dev);
    atomic_set(&device_connected, 1);
    pr_info(DRIVER_NAME ": device probed\n");
    return 0;
}
static void leap_disconnect(struct usb_interface *intf)
{
    struct leap_dev *dev = usb_get_intfdata(intf);
    atomic_set(&device_connected, 0);
    if (dev) {
        if (dev->urb) {
            usb_kill_urb(dev->urb);
            usb_free_urb(dev->urb);
        }
        usb_put_dev(dev->udev);
        kfree(dev);
    }
    usb_set_intfdata(intf, NULL);
    pr_info(DRIVER_NAME ": device disconnected\n");
}
static int leap_proc_show(struct seq_file *m, void *v)
{
    int i;
    seq_printf(m, "connected:  %d\n", atomic_read(&device_connected));
    seq_printf(m, "fifo_used:  %u\n", kfifo_len(&event_fifo));
    seq_printf(m, "fifo_avail: %u\n", kfifo_avail(&event_fifo));
    seq_puts(m,   "\ngesture counts:\n");
    for (i = 0; i < NUM_GESTURES; i++)
        seq_printf(m, "  %-14s %d\n",
                   gesture_names[i], atomic_read(&gesture_counts[i]));
    return 0;
}
static int leap_proc_open(struct inode *inode, struct file *f)
{
    return single_open(f, leap_proc_show, NULL);
}
static const struct proc_ops leap_proc_ops = {
    .proc_open    = leap_proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};
    if (evt.gesture < NUM_GESTURES)
        atomic_inc(&gesture_counts[evt.gesture]);
    ret = wait_event_interruptible(write_wq, !kfifo_is_full(&event_fifo));
    if (ret)
        return ret;
    if (kfifo_in(&event_fifo, &evt, 1) == 0)
        return -ENOSPC;
    wake_up_interruptible(&read_wq);
    return sizeof(evt);
}
static const struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = leapcmd_open,
    .read    = leapcmd_read,
    .write   = leapcmd_write,
    .release = leapcmd_release,
};
    for (i = 0; i < NUM_GESTURES; i++)
        atomic_set(&gesture_counts[i], 0);
    atomic_set(&device_connected, 0);
    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err(DRIVER_NAME ": alloc_chrdev_region failed: %d\n", ret);
        return ret;
    }
    major_num = MAJOR(dev);
    cdev_init(&leapcmd_cdev, &fops);
    leapcmd_cdev.owner = THIS_MODULE;
    ret = cdev_add(&leapcmd_cdev, dev, 1);
    if (ret) {
        pr_err(DRIVER_NAME ": cdev_add failed: %d\n", ret);
        goto err_cdev;
    }
    leapcmd_class = class_create(CLASS_NAME);
    if (IS_ERR(leapcmd_class)) {
        ret = PTR_ERR(leapcmd_class);
        pr_err(DRIVER_NAME ": class_create failed: %d\n", ret);
        goto err_class;
    }
    if (IS_ERR(device_create(leapcmd_class, NULL, dev, NULL, DEVICE_NAME))) {
        pr_err(DRIVER_NAME ": device_create failed\n");
        ret = -ENOMEM;
        goto err_device;
    }
    ret = usb_register(&leap_driver);
    if (ret) {
        pr_err(DRIVER_NAME ": usb_register failed: %d\n", ret);
        goto err_usb;
    }
    leap_proc = proc_create(DRIVER_NAME, 0444, NULL, &leap_proc_ops);
    if (!leap_proc)
        pr_warn(DRIVER_NAME ": failed to create /proc entry (non-fatal)\n");
    pr_info(DRIVER_NAME ": loaded, major=%d\n", major_num);
    return 0;
err_usb:
    device_destroy(leapcmd_class, dev);
err_device:
    class_destroy(leapcmd_class);
err_class:
    cdev_del(&leapcmd_cdev);
err_cdev:
    unregister_chrdev_region(dev, 1);
    return ret;
}
static void __exit leapcmd_exit(void)
{
    dev_t dev = MKDEV(major_num, 0);
    if (leap_proc)
        proc_remove(leap_proc);
    usb_deregister(&leap_driver);
    device_destroy(leapcmd_class, dev);
    class_destroy(leapcmd_class);
    cdev_del(&leapcmd_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info(DRIVER_NAME ": unloaded\n");
}
module_init(leapcmd_init);
module_exit(leapcmd_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Team-12");
MODULE_DESCRIPTION("Leap Motion Driver");
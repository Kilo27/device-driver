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

static int leap_probe(struct usb_interface *interface, const struct usb_device_id *id);
static void leap_disconnect(struct usb_interface *interface);

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


static struct usb_device_id leap_table[] = {
    { USB_DEVICE(LEAP_VENDOR_ID, LEAP_PRODUCT_ID) }, // Leap Motion Vendor/Product ID
    {}
};

MODULE_DEVICE_TABLE(usb,leap_table);

static struct usb_driver leap_driver = {
    .name = "leap_motion",
    .probe = leap_probe,
    .disconnect = leap_disconnect,
    .id_table = leap_table,
};

static int __init leap_init(void) {
    return usb_register(&leap_driver);
}

static void __exit leap_exit(void) {
    usb_deregister(&leap_driver);
}


module_init(leap_init);
module_exit(leap_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Authors");
MODULE_DESCRIPTION("Leap Motion Driver");

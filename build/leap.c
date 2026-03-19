#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/hid.h>

static int leap_probe(struct usb_interface *interface, const struct usb_device_id *id);
static void leap_disconnect(struct usb_interface *interface);


static int leap_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    printk(KERN_INFO "Leap Motion device plugged in\n");
    // Add your initialization code here
    return 0;
}

static void leap_disconnect(struct usb_interface *interface) {
    printk(KERN_INFO "Leap Motion device removed\n");
    // Add your cleanup code here
}



static struct usb_device_id leap_table[] = {
    { USB_DEVICE(0x294B, 0x0001) }, // Leap Motion Vendor/Product ID
    {}
};

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

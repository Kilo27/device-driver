#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/hid.h>

static int leap_probe(struct usb_interface *interface, const struct usb_device_id *id);
static void leap_disconnect(struct usb_interface *interface);




static int leap_probe(struct usb_interface *intf, const struct usb_device_id *id) {
    struct usb_device *udev = interface_to_usbdev(intf);
    usb_set_intfdata(intf, your_private_data);
    return 0;
}


static void leap_disconnect(struct usb_interface *intf) {
    struct your_dev *dev = usb_get_intfdata(intf);
    usb_kill_urb(dev->urb);
    usb_free_urb(dev->urb);
}


static struct usb_device_id leap_table[] = {
    { USB_DEVICE(0x294B, 0x0001) }, // Leap Motion Vendor/Product ID
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

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#define VENDOR_ID 0xf182
#define PRODUCT_ID 0x0003
static int major;

static struct usb_device_id usb_handles[] = { { USB_DEVICE(VENDOR_ID, PRODUCT_ID) }; };
MODULE_DEVICE_TABLE(usb, usb_handles);

static int leap_probe(struct usb_interfaces* usb_intfs, struct usb_device_id* usb_dID)
{
    struct usb_device* usb_parent = interface_to_usbdev(intf);
    struct usb_interface* current_intf;

    for (int i = 0; i < udev->config->desc.bNumInterfaces; i++)
    {
	current_intf = udev->config->interface[i];

	if (current_intf && current_intf->dev.driver)
	{
	    struct usb_driver* old_driver = to_usb_driver(cur_intf->dev.driver);

	    //continue checks here...
        }
    }
}

static ssize_t my_read(struct file *f, char __user *u , size_t l, loff_t *o){
    printk("Read is called\n");
    return 0;
}

static struct file_operations fops = {
    .read = my_read
};

static int __init leap_init(void) {

    major = register_chrdev(0,"leap", &fops);

    if (major<0){
        printk("Error in registering leap motion device\n");
        return major;
    }
    printk("The device has been registered. The Major Device Number is %d\n", major);
    return 0;
}

static void __exit leap_exit(void) {
    unregister_chrdev(major, "leap");
    printk("The leap device has been deregistered\n");
    
}


module_init(leap_init);
module_exit(leap_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Team-12");
MODULE_DESCRIPTION("Leap Motion Driver");

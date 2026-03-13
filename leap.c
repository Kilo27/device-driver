#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/usb.h>
#define VENDOR_ID 0xf182
#define PRODUCT_ID 0x0003
static int major;

static struct usb_driver leap_usb_driver;

static struct usb_device_id usb_table[] = {{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) }, {}};
MODULE_DEVICE_TABLE(usb, usb_table);

static void leap_disconnect(struct usb_interface* intf)
{
    printk("The leap device has been disconnected\n");
}

static int leap_probe(struct usb_interface* usb_intf, const struct usb_device_id* usb_dID)
{
    printk("Probing leap device");

    struct usb_device* usb_parent = interface_to_usbdev(usb_intf);
    struct usb_interface* current_intf;

    for (int i = 0; i < usb_parent->config->desc.bNumInterfaces; i++)
    {
	current_intf = usb_parent->config->interface[i];

	if (current_intf && current_intf->dev.driver)
	{
	    struct usb_driver* old_driver = to_usb_driver(current_intf->dev.driver);

	    if (old_driver && old_driver != &leap_usb_driver)
	    {
	        printk("Previous driver unbinded\n");
	        usb_driver_release_interface(old_driver, current_intf);
	    }
        }
    }

    return 0;
}

static struct usb_driver leap_usb_driver = 
{
    .name = "leap",
    .id_table = usb_table,
    .disconnect = leap_disconnect,
    .probe = leap_probe,
};

static ssize_t my_read(struct file *f, char __user *u , size_t l, loff_t *o){
    printk("Read is called\n");
    return 0;
}

static struct file_operations fops = {
    .read = my_read
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

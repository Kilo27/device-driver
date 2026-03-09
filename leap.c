#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
static int major;

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

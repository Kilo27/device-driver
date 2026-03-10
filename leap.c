#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
static int major;

static ssize_t my_read(struct file *f, char __user *u , size_t l, loff_t *o){
    printk("Read is called\n");
    return 0;
}

static ssize_t my_write(struct file *filep, const char __user *user_buf, size_t len, loff_t *off){
    int not_copied, delta, to_copy = (len + *off) < sizeof(text) ? len : (sizeof(text) - *off);

    pr_info("write is called, we want to add %ld bytes, but actually only copying %d bytes. The offset is %lld\n", len, to_copy, *off);

    if(*off >= sizeof(text)){
        return 0;
    }

    not_copied = copy_from_user(&text[*off], user_buf, to_copy);
    delta = to_copy - not_copied;

    if(not_copied){
        pr_warn("leap.c - Could only copy %d bytes\n",delta);

        *off += delta;
        return delta;
    }

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

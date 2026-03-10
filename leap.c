#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
static int major;

static ssize_t my_read(struct file *filep, char __user *buf , size_t len, loff_t *off){
    //printk("Read is called\n");

     if(copy_to_user(buf,kernel_buffer, mem_size)){
        printk("Failed to copy data to user space\n");
        return -EFAULT;
    }
    printk(KERN_INFO "Data read done\n");
    return mem_size;
    
}

static ssize_t my_write(struct file *filep, const char __user *user_buf, size_t len, loff_t *off){
   
    if (copy_from_user(kernel_buffer,buf,len)){
        printk("Failed to copy data to userspace\n");
        return -EFAULT;
    }

    printk(KERN_INFO "Write Function\n");
    return len;

}

static struct file_operations fops = {
    .read = my_read,
    .write = my_write
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

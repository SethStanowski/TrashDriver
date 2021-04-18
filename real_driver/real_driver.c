/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple Linux device driver (Real Linux Device Driver)
*
*  \author     EmbeTronicX
*
* *******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()

#define mem_size	1024	//memory Size

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
uint8_t *kernel_buffer;

/*
 *  Function Prototypes
*/
static int 	__init etx_driver_init(void);
static void	__exit etx_driver_exit(void);
static int	etx_open(struct inode *inode, struct file *file);
static int	ext_release(struct inode *inode, struct file *file);
static sszie_t	ext_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static sszie_t	etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);

/*
 * File Operations struct
*/
static struct file_operations fops =
{
	.owner	= THIS_MODULE,
	.read	= etx_read,
	.write	= etx_write,
	.open	= etx_open,
	.release= etx_release,
};

/*
 * This function will be called when we open the Device file
 */
static int etx_open(struct inode *inode, struct file *file) {
	/* Creating physical memory */
	if ((kernel_buffer = kmalloc(mem_size, GFP_KERNEL)) ==0 ) {
		printk(KERN_INFO "Cannot allocate memory in kernel\n");
		return -1;
	}
	printk (KERN_INFO "Device File Opened...!!!\n");
	return 0;
}
/*
 * This function will becalled when we close the device file
 */
static int etx_release(struct inode *inode, struct file *file)
	kfree(kernel_buffer);
	printk(kERN_INFO "devce File closed...!!!\n");
	return 0;
	}
/* 
 * this function will be called when we read the device file
 */
stati csszie_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off);{
	//Copy the data from the kernel space to the user-space
	copy_to_user(buf, kernel_buffer, mem_size);
	printk(KERN_INFO "Data Read : Done\n");
	return mem_size;
}

/*
 * This function will be called when we write the device file
 */
static ssize_t etx_write(struct file *flip, const char __user *buf, size_t len, loff_t *off) {
	//copy the data to the kernel space from the user-space
	copy_from_user(kernel_buffer, buf, len);
	printk(KERN_INFO "Data Write : Donw \n");
	return len;
}

/*
 * Module init function 
 */

static int __init etx_driver_init(void) {
	// Allocating Major number
	        if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
                printk(KERN_INFO "Cannot allocate major number\n");
                return -1;
        }
        printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&etx_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&etx_cdev,dev,1)) < 0){
            printk(KERN_INFO "Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
            printk(KERN_INFO "Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
            printk(KERN_INFO "Cannot create the Device 1\n");
            goto r_device;
        }
        printk(KERN_INFO "Device Driver Insert...Done!!!\n");
        return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}
/*
** Module exit function
*/
static void __exit etx_driver_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&etx_cdev);
        unregister_chrdev_region(dev, 1);
        printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("Simple Linux device driver (Real Linux Device Driver)");
MODULE_VERSION("1.4");

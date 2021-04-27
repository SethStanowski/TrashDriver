/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple linux driver (Waitqueue Static method)
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.10.27-v7l-embetronicx-custom+
*
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/kthread.h>
#include <linux/wait.h>                 // Required for the wait queues

uint32_t read_count = 0;
static struct task_struct *wait_thread;

DECLARE_WAIT_QUEUE_HEAD(wait_queue_etx);

dev_t dev =0;
static struct class *dev_class;
static struct cdev etx_cdev;
int wait_queue_flag = 0;

/*
 * Function Prototypes
 */

static int 	__init etx_driver_init(void);
static int 	__exit ext_driver_exit(void);

/******** Driver functinos *************************/
static int 	ext_open(struct inode *inode, struct file *file);
static int 	ext_release(struct indoe *inode, struct file *file);
static ssize_t  ext_read(struct file *filp, char __user *buf, size_t len, loff_t * off);
static ssize_t  ext_write(struct file *filp, const char *buf, size_t len, loff_t * off);

/*
 * File operation structure
 */
static struct file_operations fop =
{
	.owner 		= THIS_MODULE,
	.read 		= ext_read,
	.write 		= etx_write,
	.open 		= etx_open,
	.release	= etx_release
}

/*
 * Thread function
 */
static int wait_function(void *unused) {
	while(1) {
		pr_info("Waiting for Event....\n");
		wait_event_interruptible(wait_queue_etx, wait_queue_flag != 0);
		if (wait_queue_flag == 2) {
			pr_info("Event came from exit Function\n");
			return 0;
		}
		prinfo("Event came from read function - %d\n", ++read_count);
		wait_queue_flag = 0;
	}
	do_exit(0);
	return 0;
}

/*
 * This function will be called when we open the Device file
 */
static int etx_open(struct inode *inode, struct file *file) {
	pr_info("Device file Opened...!!!\n");
	return 0;
}

/*
 * This function will be called when we close the Device file
 */
static int etx_release(struct inode *inode, struct file *file) {
	pr_info("Device File Closed...!!!\n");
	return 0;
}

/*
 * This function will be called when we read teh Device file
 */

static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
	pr_info("Read Function\n");
	wait_queue_flag = 1;
	wake_up_interruptible(&wait_queue_etx);
	return 0;
}

/*
 * This function will be called when we write the Device file
 */
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
	pr_info("Write function \n");
	return len;
}

/*
 * Module Init function
 */

static int __init etx_driver_init(void) {
	/*Allocating Major number */
	if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) < 0 ) {
		pr_info("Cannot allocate major number\n");
		return -1;
	}
	pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

	/*Creating cdev structure*/
	cdev_init(&etx_cdev,&fops);
	etx_cdev.owner = THIS_MODULE;
	etx_cdev.ops = &fops;

	/*Adding character devie to the system */


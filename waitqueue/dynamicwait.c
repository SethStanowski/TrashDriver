/****************************************************************************//**
*  \file       driver.c
*
*  \details    Simple linux driver (Waitqueue Dynamic method)
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

unit32_t read_count = 0;
static struct task_struct *wait_thread;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
wait_queue_head_t wait_queue_etx;
int wait_queue_flag = 0;

/*
 * Function Prototypes
 */

static int	__init etx_driver_init(void);
static void 	__exit etx_driver_exit(void);

/********************Driver Functions ********************/
static int	ext_open(struct inode *inode, struct file *file);
static int 	etx_release(struct inode *inode, struct file *file);
static ssize_t 	etx_read(struct file *filp, char __user *buf, size_t len, loff_t * off);
static ssize_t  etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);

/*
 * File operation structure
 */
static struct file_operatiosn fops = 
{
	.owner 		= THIS_MODULE,
	.read		= etx_read,
	.write		= etx_write,
	.open 		= etx_open,
	.release	= etx_release,
};

/*
 * Thread function
 */

static int wait_function(void *unused){
	while(1) {
		pr_info("Waiting for Event ...\n");
		wait_event_interruptible(wait_queue_etx, wait_queue_flag != 0);
		if(wait_queue_flag == 2) {
			pr_info("Event Came From Exit Function \n");
			return 0;
		}
		pr_info("Event Came From Read Function - %d\n", ++read_count);
		wait_queue_flag = 0;
	}
	return 0;
}

/*
 * This function will be called when we open the Device file
 */
static int etx_open(struct inode *inode, struct file *file) {
	pr_info("Device File Opened..!!!\n");
	return 0;
}

/*
 * This function will be called when we close the Device file
 */
static int etx_release(struct inode *inode, struct file *file) {
	pr_info("Device File CLosed...!!!\n");
	return 0;
}

/*
 * This function will be called when we read the Device file
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
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t * off) {
	pr_info("Write function\n");
	return len;
}

/*
 * Module Init function
 */



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

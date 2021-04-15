/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple Linux device driver (File Operations)
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

dev_t dev =0;
static struct class *dev_class;
static struct cdev etx_cdev;

/*
 * Function Prototypes
 */

static int	__init etx_driver_init(void);
static void	__exit etx_driver_exit(void);
static int 	etx_open(struct inode *inode, struct file *file);
static int	etx_release(struct inode *inode, struct file *file);
static ssize_t	etx_read(struct file *filp, char __user *buf, size_t len, loff_t * off);
static ssize_t	etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);

static struct file_operations fops =
{
	.owner	= THIS_MODULE,
	.read	= etx_read,
	.write	= etx_write,
	.open	= etx_open,
	.release= etx_release,
};

/*
** This fuction will be called when we open the Device file
*/
static int etx_open(struct inode *inode, struct file *file) {
	printk(KERN_INFO "Driver Open Function Called...!!!\n");
	return 0;
}

/*
 * This function will be called when we close the Devie file
 */

static int etx_release(struct inode *inode, struct file *file) {
	printk(KERN_INFO "Driver Release Function Called...!!!\n");
	return 0;
}

/*
 * This function will be called when we read the device file
 */

static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off) {
	printk(KERN_INFO "Deriver Read Function Called..!!\n");
	return 0;
}

/*
 * This function will be called when we write the Device file
 */

static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {
	printk(KERN_NFO "Deriver write function called....!!!");
	return len;
}

/*
 * Module init function
 */



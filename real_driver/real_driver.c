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

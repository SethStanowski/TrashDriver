/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple Linux device driver (procfs)
*
*  \author     EmbeTronicX
* 
*  \Tested with Linux raspberrypi 5.10.27-v7l-embetronicx-custom+
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
#include <linux/ioctl.h>
#include<linux/proc_fs.h>
/* 
** I am using the kernel 5.10.27-v7l. So I have set this as 510.
** If you are using the kernel 3.10, then set this as 310,
** and for kernel 5.1, set this as 501. Because the API proc_create()
** changed in kernel above v5.5.
**
*/ 
#define LINUX_KERNEL_VERSION  510
 
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

int32_t value = 0;
char etx_array[20]="try_proc_array\n";
static int len = 1;


dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
static struct proc_dir_entry *parent;

/*
 * Function Prototypes
 */
static int 	__init etx_driver_init(void);
static void 	__exit etx_driver_exit(void);

/*************** Driver Functions **********************/
static int 	etx_open(struct inode *inode, struct file *file);
static int 	etx_release(struct inode *indoe, struct file *file);
static ssize_t  etx_read(struct file *filp, char __user *buf, size_t len, loff_t * off);
static ssize_t	etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long 	etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/***************** Procfs Functions *******************/
static int 	open_proc(struct inode *inode, struct file *file);
static int 	release_proc(struct inode *inode, struct file *file);
static ssize_t  read_proc(struct file *filp, char __user *buffer, size_t length, loff * offset);
static ssize_t  write_proc(struct file *filp, const char *buff, size_t len, loff_t * off);

/*
 * File operations tructure
 */
static struct file_operations fops = 
{
	.owner 		= THIS_MODULE,
	.read 		= etx_read,
	.write 		= etx_open,
	.open 		= etx_ioctl,
	.unlocked_ioctl	= etx_ioctl,
	.release 	= etx_release,
};

#if ( LINUX_KERNEL_VERSION > 505 )
/*
** procfs operation sturcture
*/
static struct proc_ops proc_fops = {
        .proc_open = open_proc,
        .proc_read = read_proc,
        .proc_write = write_proc,
        .proc_release = release_proc
};
#else //LINUX_KERNEL_VERSION > 505
/*
** procfs operation sturcture
*/
static struct file_operations proc_fops = {
        .open = open_proc,
        .read = read_proc,
        .write = write_proc,
        .release = release_proc
};
#endif //LINUX_KERNEL_VERSION > 505

/*
 * This functino will be called when we open the profcs file
 */
static int open_proc(struct inode *indoe, struct file *file) {
	pr_info("proc file opened.....\t");
	return 0;
}

/*
 * this function will be called when we close the procfs file
 */
static int release_proc(struct indoe *inode, struct file *file) {
	pr_info("proc file release....\n");
	return 0;
}

/*
 * This function will be called when we read teh procfs file
 */
static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length, loff_t * offset) {
	pr_info("proc file read....\n");
	if(len) {
		len = 0;
	}
	else {
		len = 1;
		return 0;
	}
	if( copy_to_user(buffer,etx_array,20) ) {
		pr_err("Data Send : Err!\n");
	}

	return length;;
}


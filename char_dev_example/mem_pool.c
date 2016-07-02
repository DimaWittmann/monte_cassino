#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h> /*mem alloc*/
#include <linux/cdev.h>
#include <asm/uaccess.h> /*user access*/ 

 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytrii Wittmann");
MODULE_DESCRIPTION("Persistent memory storage");
MODULE_VERSION("0.1");

#define DEV_NAME "mem_pool"
#define STORAGE_SIZE  PAGE_SIZE
#define __STRING(s) #s

static int major = 0;
module_param(major, int, 0644);
MODULE_PARM_DESC(major, "major char dev numver default to zero");

static unsigned long storage_size = STORAGE_SIZE;
module_param(storage_size, ulong, 0644);
MODULE_PARM_DESC(storage_size, "size of the storage");


int pool_open(struct inode *inode, struct file *file);
int pool_release(struct inode *inode, struct file *file);

ssize_t pool_read(struct file *file, char __user *buff, size_t buff_size, loff_t *offset);
ssize_t pool_write(struct file *file, const char __user *buff, size_t buff_size, loff_t *offset);

static void *storage = NULL;
static struct cdev *pool_cdev = 0;
static struct file_operations fops = 
{
	.owner = THIS_MODULE,
	.open = pool_open,
	.release = pool_release,
	.read = pool_read,
	.write = pool_write,
};


int pool_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO  "%s \n", __FUNCTION__);
	return 0;
}
int pool_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO  "%s \n", __FUNCTION__);
	return 0;
}

ssize_t pool_read(struct file *file, char __user *buff, size_t buff_size, loff_t *offset)
{
	printk(KERN_INFO  "%s \n", __FUNCTION__);

	if (*offset > storage_size) {
		return 0;
	}

	if (*offset + buff_size > storage_size) {
		buff_size = storage_size - *offset;
	}

	unsigned int res = copy_to_user(buff, storage + *offset, buff_size);
	if (res) {
		return -EIO;
	}

	*offset += buff_size;
	return buff_size;
}

ssize_t pool_write(struct file *file, const char __user *buff, size_t buff_size, loff_t *offset)
{
	printk(KERN_INFO  "%s \n", __FUNCTION__);
	if (*offset > storage_size) {
		return 0;
	}

	if (*offset + buff_size > storage_size) {
		buff_size = storage_size - *offset;
	}

	if (copy_from_user(storage + *offset, buff, buff_size)) {
		return -EIO;
	}

	*offset += buff_size;
	return buff_size;
}


static int __init init(void) 
{
	int ret = 0;
	printk(KERN_INFO " init " __FILE__"\n");

	if (major == 0) {
		dev_t dev;
		ret = alloc_chrdev_region(&dev, 0, 1, DEV_NAME);
		if (ret) {
			printk(KERN_WARNING "%s returned %d\n",  
				__STRING(alloc_chrdev_region), ret);
			goto out;
		}
		major = MAJOR(dev);
	} else {
		ret = register_chrdev_region(MKDEV(major, 0), 1, DEV_NAME);
		if (ret) {
			printk(KERN_WARNING  "%s returned %d\n", 
				__STRING(register_chrdev_region), ret);
			goto out;
		}
	}

	printk(KERN_INFO "device registered with name %s and major %d\n", DEV_NAME, major);

	storage = kzalloc(storage_size, GFP_KERNEL);
	if (!storage) {
		printk(KERN_WARNING "memory allocation error");
		goto unregister_out;
	}

	pool_cdev = cdev_alloc();
	if (pool_cdev) {
		cdev_init(pool_cdev, &fops);
		ret = cdev_add(pool_cdev, MKDEV(major, 0), 1);
		if (ret) {
			printk(KERN_WARNING "%s returned %d", __STRING(cdev_add),  ret);
			goto free_out;
		}
	} else {
		printk(KERN_WARNING "cdev allocation error");
		goto free_out;
	}

	return 0;

free_out:
	kfree(storage);

unregister_out:
	unregister_chrdev_region(MKDEV(major, 0), 1);

out:
	return -EFAULT;
}


static void __exit exit(void)
{
	printk(KERN_INFO "exit" __FILE__"\n");
	unregister_chrdev_region(MKDEV(major, 0), 1);
	kfree(storage);
	cdev_del(pool_cdev);
}

module_init(init);
module_exit(exit);

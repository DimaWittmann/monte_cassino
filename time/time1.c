#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytrii Wittmann");
MODULE_DESCRIPTION("Try ndelay");
MODULE_VERSION("0.1");


static int __init init(void) 
{
	printk(KERN_INFO "init " __FILE__);
	ndelay(10000);
	printk(KERN_INFO "delayed printk");
	return 0;
}


static void __exit exit(void)
{
	printk(KERN_INFO "exit " __FILE__);
}

module_init(init);
module_exit(exit);
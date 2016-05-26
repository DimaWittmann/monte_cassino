#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytrii Wittmann");
MODULE_DESCRIPTION("Expore how log it takes to lauched tasklet" 
			"after it was scheduled in process context");
MODULE_VERSION("0.1");


static struct tasklet_struct tasklet;

static void tasklet_func(unsigned long data)
{
	printk(KERN_INFO "It took %lu\n", jiffies - data);
}

static int __init init(void) 
{
	tasklet_init(&tasklet, tasklet_func, (unsigned long) jiffies);
	tasklet_schedule(&tasklet);

	return 0;
}


static void __exit exit(void)
{
	tasklet_kill(&tasklet);
}

module_init(init);
module_exit(exit);
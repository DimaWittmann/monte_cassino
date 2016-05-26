#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytrii Wittmann");
MODULE_DESCRIPTION("Try calling schedule in a tasklet");
MODULE_VERSION("0.1");


static struct tasklet_struct tasklet;

static void tasklet_func(unsigned long data)
{
	schedule();
}

static int __init init(void) 
{
	printk(KERN_INFO "init " __FILE__);

	tasklet_init(&tasklet, tasklet_func, (unsigned long) jiffies);
	tasklet_schedule(&tasklet);

	return 0;
}


static void __exit exit(void)
{
	printk(KERN_INFO "exit " __FILE__);

	tasklet_kill(&tasklet);
}

module_init(init);
module_exit(exit);
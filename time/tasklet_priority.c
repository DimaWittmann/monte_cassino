#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytrii Wittmann");
MODULE_DESCRIPTION("Examples of hi and normal priority tasklets");
MODULE_VERSION("0.1");

static struct tasklet_priority_data {
	struct tasklet_struct normal_tasklet;
	struct tasklet_struct hi_tasklet;	
} global_data;


static void tasklet_func(unsigned long data)
{
	char* str = (char*) data;
	printk(KERN_INFO " %s ", str);
}

static int __init init(void) 
{
	printk(KERN_INFO "init " __FILE__);

	tasklet_init(&global_data.normal_tasklet, tasklet_func, 
			(unsigned long) "Normal tasklet");
	tasklet_init(&global_data.hi_tasklet, tasklet_func, 
			(unsigned long) "Hi tasklet");

	tasklet_schedule(&global_data.normal_tasklet);
	tasklet_hi_schedule(&global_data.hi_tasklet);

	return 0;
}


static void __exit exit(void)
{
	printk(KERN_INFO "exit " __FILE__);

	tasklet_kill(&global_data.normal_tasklet);
	tasklet_kill(&global_data.hi_tasklet);
}

module_init(init);
module_exit(exit);
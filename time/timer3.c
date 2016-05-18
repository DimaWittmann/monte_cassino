#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytrii Wittmann");
MODULE_DESCRIPTION("First periodical timer");
MODULE_VERSION("0.1");

static char* data = "data to print on timer event";
module_param(data, charp, S_IRUGO);
MODULE_PARM_DESC(data, "Data to print on timer event");

static int delay = 1;
module_param(delay, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(delay, "Timer delay in seconds");


static struct timer_list one_shot_timer;
static struct timer_list periodical_timer;

static int number;

void periodical_timer_func(unsigned long data)
{
	char* str = (char*) data;
	printk(KERN_INFO " %s %d", str, number++);
	mod_timer(&periodical_timer, jiffies + delay * HZ);
}

void one_shot_timer_func(unsigned long data)
{
	periodical_timer.data = data;
	periodical_timer.expires = jiffies + delay * HZ;
	periodical_timer.function = periodical_timer_func;
	init_timer(&periodical_timer);
	add_timer(&periodical_timer);
}


static int __init init(void) 
{
	printk(KERN_INFO "init " __FILE__);

	one_shot_timer.data = (unsigned long) data;
	one_shot_timer.expires = jiffies + delay * HZ;
	one_shot_timer.function = one_shot_timer_func;
	init_timer(&one_shot_timer);
	add_timer(&one_shot_timer);

	return 0;
}


static void __exit exit(void)
{
	del_timer_sync(&periodical_timer);
	del_timer_sync(&one_shot_timer);
	printk(KERN_INFO "exit " __FILE__);

}

module_init(init);
module_exit(exit);
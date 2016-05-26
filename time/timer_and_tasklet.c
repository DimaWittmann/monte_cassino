#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/interrupt.h>
#include <asm/atomic.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytrii Wittmann");
MODULE_DESCRIPTION("Difference betweet a timer and tasklet that was scheduled by the timer");
MODULE_VERSION("0.1");


static struct {
	struct timer_list timer;
	struct tasklet_struct tasklet;

	atomic_t repeats;
	unsigned long max_difference;
} global_data;


void timer_func(unsigned long data)
{
	global_data.tasklet.data = jiffies;
	tasklet_schedule(&global_data.tasklet);

	if (!atomic_dec_and_test(&global_data.repeats)) {
		mod_timer(&global_data.timer, jiffies + 10);	
	} else {
		printk(KERN_INFO " Max difference %lu\n", 
			global_data.max_difference);
	}
}

void tasklet_func(unsigned long data)
{
	unsigned long difference = jiffies - data;
	if (difference > global_data.max_difference) {
		global_data.max_difference = difference;
		printk(KERN_INFO "difference update %lu\n", global_data.max_difference);
	}
}

static int __init init(void) 
{
	printk(KERN_INFO "init " __FILE__);


	global_data.timer.data = (unsigned long) jiffies;
	global_data.timer.expires = jiffies + 10;
	global_data.timer.function = timer_func;
	init_timer(&global_data.timer);
	add_timer(&global_data.timer);

	tasklet_init(&global_data.tasklet, tasklet_func, (unsigned long) jiffies);

	atomic_set(&global_data.repeats, 10000);

	global_data.max_difference = 0;

	return 0;
}


static void __exit exit(void)
{
	printk(KERN_INFO "step %d\n", atomic_read(&global_data.repeats));
	atomic_set(&global_data.repeats, 0);

	del_timer_sync(&global_data.timer);
	tasklet_kill(&global_data.tasklet);

	printk(KERN_INFO " Max difference %lu\n", global_data.max_difference);
	printk(KERN_INFO "exit " __FILE__"\n");
}

module_init(init);
module_exit(exit);
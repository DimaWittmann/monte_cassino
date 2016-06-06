#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/kthread.h>
#include <linux/completion.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytrii Wittmann");
MODULE_DESCRIPTION("100 threads");
MODULE_VERSION("0.1");

#define THREAD_NUMBER 100

struct thread_stat
{
	int number;
	pid_t pid;
	unsigned long jiffies;
};

static struct data{
	struct completion thread_complete[THREAD_NUMBER];
	struct thread_stat thread_stat[THREAD_NUMBER];
} g;


static int thread_fn(long arg)
{
	unsigned long start_jiffies = jiffies;
	int number = (int) arg;

	if (number >= 0 && number < THREAD_NUMBER) {
		g.thread_stat[number].number = number;
		g.thread_stat[number].pid = current->pid;
		g.thread_stat[number].jiffies = start_jiffies;
	

		printk(KERN_INFO "Thread %d pid %d jiffies %lu\n", 
			g.thread_stat[number].number, g.thread_stat[number].pid,
			g.thread_stat[number].jiffies);

		complete(&g.thread_complete[number]);
	}

	return 0;
}

static int __init init(void) 
{
	struct task_struct* threads[THREAD_NUMBER];
	int thread_index, i;

	printk(KERN_INFO "init " __FILE__"\n");

	for (thread_index = 0; thread_index < THREAD_NUMBER; thread_index++) {
		init_completion(&g.thread_complete[thread_index]);
	}

	for (thread_index = 0; thread_index < THREAD_NUMBER; thread_index++) {
		threads[thread_index] = kthread_create(thread_fn, (long) thread_index, 
					"KThread %d", thread_index);
		if (IS_ERR(threads[thread_index])) {
			goto ERROR;
		}
	}
	 
	for (thread_index = 0; thread_index < THREAD_NUMBER; thread_index++) {
		wake_up_process(threads[thread_index]);
	}

	return 0;

ERROR:
	
	for (i = 0; i < thread_index; i++) {
		kthread_stop(threads[i]);
	}
	
	return -1;
}


static void __exit exit(void)
{
	int i;
	printk(KERN_INFO "exit " __FILE__ "\n");

	for (i = 0; i < THREAD_NUMBER; i++) {
		wait_for_completion(&g.thread_complete[i]);
	}
}

module_init(init);
module_exit(exit);
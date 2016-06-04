#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/kthread.h>
#include <linux/semaphore.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytrii Wittmann");
MODULE_DESCRIPTION("Semaphore sinchronization example");
MODULE_VERSION("0.1");


static struct {
	// signals that array is inialized
	struct semaphore sema_data_ready; 
	// signals how many threads finished its work
	struct semaphore sema_threads_finished; 

	int array[10];
} g;

static int init_data_thread_fn(void* arg)
{
	int i;
	for(i = 0; i < sizeof(g.array); i++) {
		g.array[i] = i;
	}

	up(&g.sema_threads_finished);
	up(&g.sema_threads_finished);
	return 0;
}


static int multiply_data_thread_fn(int koef)
{
	int larr[sizeof(g.array)];

	down(&g.sema_threads_finished);

	int i;
	for(i = 0; i < sizeof(g.array); i++) {
		larr[i] = g.array[i] * koef;
	}

	printk("Multiplied array by %d: %d %d %d %d %d %d %d %d %d %d\n", koef,
		larr[0], larr[1], larr[2], larr[3], larr[4], larr[5], larr[6], 
		larr[7], larr[8], larr[9]);

	up(&g.sema_threads_finished);
	return 0;
}


static int __init init(void) 
{
	printk(KERN_INFO "init " __FILE__"\n");

	sema_init(&g.sema_threads_finished, 0);
	sema_init(&g.sema_data_ready, 0);

	struct task_struct* init_thread = kthread_create(init_data_thread_fn,
							NULL, "KThread 1");
	if (IS_ERR(init_thread)) {
		goto ERROR_THREAD_1;
	}

	struct task_struct* multiply_thread_2 = kthread_create(
				multiply_data_thread_fn, (int) 2, "KThread 2"
				);
	if (IS_ERR(multiply_thread_2)) {
		goto ERROR_THREAD_2;
	}

	struct task_struct* multiply_thread_3 = kthread_create(
				multiply_data_thread_fn, (int) 3, "KThread 3"
				);
	if (IS_ERR(multiply_thread_3)) {
		goto ERROR_THREAD_3;
	}

	wake_up_process(init_thread);
	wake_up_process(multiply_thread_2);
	wake_up_process(multiply_thread_3);

	return 0;

ERROR_THREAD_3:
	kthread_stop(multiply_thread_2);
ERROR_THREAD_2:
	kthread_stop(init_thread);
ERROR_THREAD_1:	

	return -1;
}


static void __exit exit(void)
{
	printk(KERN_INFO "exit " __FILE__ "\n");

	down(&g.sema_threads_finished);
	down(&g.sema_threads_finished);
	down(&g.sema_threads_finished);
}

module_init(init);
module_exit(exit);
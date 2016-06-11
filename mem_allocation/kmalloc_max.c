#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytrii Wittmann");
MODULE_DESCRIPTION("Search for max memory size that could be allocated by single call to kmalloc");
MODULE_VERSION("0.1");


static int __init init(void)
{
	printk(KERN_INFO "init " __FILE__);
	int l_edge = 0;
	int r_edge = 1024 * 1024 * 1024;
	int mid = l_edge + (r_edge - l_edge) / 2;

	while (r_edge - l_edge > 1) {
		void* m = kmalloc(mid, GFP_KERNEL);
		if (m) {
			l_edge = mid;
			kfree(m);
		} else {
			r_edge = mid;
		}
		mid = l_edge + (r_edge - l_edge) / 2;
	}

	printk(KERN_INFO "SUCCESSFULL ALLOCATION %dk", mid / 1024);

	return 0;
}


static void __exit exit(void)
{
	printk(KERN_INFO "exit " __FILE__"\n");
}

module_init(init);
module_exit(exit);
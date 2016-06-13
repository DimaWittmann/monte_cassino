#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmytrii Wittmann");
MODULE_DESCRIPTION("Comparison of allocated blocks by kmalloc and kmem_cache");
MODULE_VERSION("0.1");

#define ALLOC_SIZE 	84
#define ALLOC_N		4

static int __init init(void)
{
	void* kmalloc_m[ALLOC_N];
	void* cache_m[ALLOC_N];
	int i;
	struct kmem_cache* cache;

	printk(KERN_INFO "init " __FILE__);

	/* kmalloc */
	for (i = 0; i < ALLOC_N; i++) {
		kmalloc_m[i] = kmalloc(ALLOC_SIZE, GFP_KERNEL);
	} 

	printk(KERN_INFO "kmalloc allocated blocks differens %d %d %d\n", 
		kmalloc_m[1] - kmalloc_m[0], kmalloc_m[2] - kmalloc_m[1], 
		kmalloc_m[3] - kmalloc_m[2]);

	for (i = 0; i < ALLOC_N; i++) {
		kfree(kmalloc_m[i]);
	}

	/*kmem cache*/
	cache = kmem_cache_create("example", ALLOC_SIZE, 0, 0, NULL);
	if (!cache) {
		printk(KERN_INFO "kmem_cache error");
		return -1;
	}

	for (i = 0; i < ALLOC_N; i++) {
		cache_m[i] = kmem_cache_alloc(cache, GFP_KERNEL);
	}

	printk(KERN_INFO "kmem_cache allocated blocks differens %d %d %d\n", 
		cache_m[1] - cache_m[0], cache_m[2] - cache_m[1], 
		cache_m[3] - cache_m[2]);

	for (i = 0; i < ALLOC_N; i++) {
		kmem_cache_free(cache, cache_m[i]);
	}
	kmem_cache_destroy(cache);

	return 0;
}


static void __exit exit(void)
{
	printk(KERN_INFO "exit " __FILE__"\n");
}

module_init(init);
module_exit(exit);
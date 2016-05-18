#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrii Tymkiv");
MODULE_DESCRIPTION("Say hello and goodbye");
MODULE_VERSION("0.22");
 
static char *name = "user";
module_param(name, charp, S_IRUGO); //just can be read
MODULE_PARM_DESC(name, "The name to display");  ///< parameter description
 
static int __init hello_init(void){
   printk(KERN_INFO "Hello, %s!\n", name);
   return 0;
}
 
static void __exit hello_exit(void){
   printk(KERN_INFO "Goodbye, %s !\n", name);
}
 
module_init(hello_init);
module_exit(hello_exit);

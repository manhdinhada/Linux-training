#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manh Dinh");
MODULE_DESCRIPTION("Module B: Importing variable");

extern int x;

static int moduleB_init(void) {
 printk(KERN_INFO "Module B loaded. Accessed variable x = %d\n",x);
 return 0;
}

static void moduleB_exit(void) {
  printk(KERN_INFO "Module B unloaded.\n");
}

module_init(moduleB_init);
module_exit(moduleB_exit);

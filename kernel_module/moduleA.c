#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR(" Manh Dinh");
MODULE_DESCRIPTION("Module A: Exporting variable");

int x = 42;
EXPORT_SYMBOL(x);

static int  init_moduleA(void)
{
  printk(KERN_ALERT "Hello, word\n");
  return 0;
}

static void  exit_moduleA(void) 
{
  printk(KERN_ALERT "Goodbye.\n");
}

module_init(init_moduleA);
module_exit(exit_moduleA);

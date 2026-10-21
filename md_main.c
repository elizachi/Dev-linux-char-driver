#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init mydev_init(void)
{
  pr_info("my_device: loaded\n");
  return 0;
}

static void __exit mydev_exit(void)
{
  pr_info("my_device: unloaded\n");
}

module_init(mydev_init);
module_exit(mydev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chigvintseva Elizaveta");
MODULE_DESCRIPTION("Simple character device driver (Light)");

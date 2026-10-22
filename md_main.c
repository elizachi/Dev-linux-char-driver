#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/version.h>
#include "md_internal.h"

struct mydev_state mdev;

static int __init mydev_init(void)
{
  int ret;

  mutex_init(&mdev.lock);

  mdev.buf = kzalloc(MD_BUF_SIZE, GFP_KERNEL);
  if (!mdev.buf)
    return -ENOMEM;

  ret = alloc_chrdev_region(&mdev.devno, 0, 1, DEVICE_NAME);
  if (ret)
    goto err_buf;

  cdev_init(&mdev.cdev, &md_fops);
  mdev.cdev.owner = THIS_MODULE;
  ret = cdev_add(&mdev.cdev, mdev.devno, 1);
  if (ret)
    goto err_region;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
  mdev.cls = class_create(DEVICE_NAME);
#else
  mdev.cls = class_create(THIS_MODULE, DEVICE_NAME);
#endif
  if (IS_ERR(mdev.cls)) {
    ret = PTR_ERR(mdev.cls);
    goto err_cdev;
  }

  mdev.dev = device_create(mdev.cls, NULL, mdev.devno, NULL, DEVICE_NAME);
  if (IS_ERR(mdev.dev)) {
    ret = PTR_ERR(mdev.dev);
    goto err_class;
  }

  pr_info("my_device: registered, major=%d minor=%d\n",
    MAJOR(mdev.devno), MINOR(mdev.devno));
  return 0;

err_class:
  class_destroy(mdev.cls);
err_cdev:
  cdev_del(&mdev.cdev);
err_region:
  unregister_chrdev_region(mdev.devno, 1);
err_buf:
  kfree(mdev.buf);
  return ret;
}

static void __exit mydev_exit(void)
{
  device_destroy(mdev.cls, mdev.devno);
  class_destroy(mdev.cls);
  cdev_del(&mdev.cdev);
  unregister_chrdev_region(mdev.devno, 1);
  kfree(mdev.buf);
  pr_info("my_device: unloaded\n");
}

module_init(mydev_init);
module_exit(mydev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chigvintseva Elizaveta");
MODULE_DESCRIPTION("Simple character device driver (Light)");

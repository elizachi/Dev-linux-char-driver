#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include "md_internal.h"
#include "my_device.h"

long md_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  int val;

  if (_IOC_TYPE(cmd) != MY_DEVICE_MAGIC)
    return -ENOTTY;

  switch (cmd) {
  case MYDEV_IOC_CLEAR:
    mutex_lock(&mdev.lock);
    memset(mdev.buf, 0, MD_BUF_SIZE);
    mdev.len = 0;
    mutex_unlock(&mdev.lock);
    return 0;

  case MYDEV_IOC_GET_SIZE:
    val = MD_BUF_SIZE;
    return put_user(val, (int __user *)arg) ? -EFAULT : 0;

  case MYDEV_IOC_GET_LEN:
    mutex_lock(&mdev.lock);
    val = mdev.len;
    mutex_unlock(&mdev.lock);
    return put_user(val, (int __user *)arg) ? -EFAULT : 0;

  case MYDEV_IOC_SET_MODE:
    if (get_user(val, (int __user *)arg))
      return -EFAULT;
    if (val < 0 || val > 1)
      return -EINVAL;
    mutex_lock(&mdev.lock);
    mdev.mode = val;
    mutex_unlock(&mdev.lock);
    return 0;

  case MYDEV_IOC_GET_MODE:
    mutex_lock(&mdev.lock);
    val = mdev.mode;
    mutex_unlock(&mdev.lock);
    return put_user(val, (int __user *)arg) ? -EFAULT : 0;

  default:
    return -ENOTTY;
  }
}

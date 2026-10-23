#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>
#include "md_internal.h"

static int md_open(struct inode *inode, struct file *file)
{
  pr_info("my_device: open\n");
  return 0;
}

static int md_release(struct inode *inode, struct file *file)
{
  pr_info("my_device: release\n");
  return 0;
}

static ssize_t md_read(struct file *file, char __user *ubuf,
           size_t count, loff_t *ppos)
{
  ssize_t ret;

  if (mutex_lock_interruptible(&mdev.lock))
    return -ERESTARTSYS;

  if (*ppos >= (loff_t)mdev.len) {  /* конец данных */
    ret = 0;
    goto out;
  }
  if (count > mdev.len - (size_t)*ppos)
    count = mdev.len - (size_t)*ppos;

  if (copy_to_user(ubuf, mdev.buf + *ppos, count)) {
    ret = -EFAULT;
    goto out;
  }
  *ppos += count;
  mdev.reads++;
  ret = count;
out:
  mutex_unlock(&mdev.lock);
  return ret;
}

static ssize_t md_write(struct file *file, const char __user *ubuf,
      size_t count, loff_t *ppos)
{
  ssize_t ret;
  size_t i;

  if (count == 0)
    return 0;

  if (mutex_lock_interruptible(&mdev.lock))
    return -ERESTARTSYS;

  if (*ppos >= MD_BUF_SIZE) {
    ret = -ENOSPC;
    goto out;
  }
  if (count > MD_BUF_SIZE - (size_t)*ppos)
    count = MD_BUF_SIZE - (size_t)*ppos;

  if (copy_from_user(mdev.buf + *ppos, ubuf, count)) {
    ret = -EFAULT;
    goto out;
  }

  if (mdev.mode == 1)    /* режим "заглавные буквы" */
    for (i = 0; i < count; i++)
      mdev.buf[*ppos + i] = toupper((unsigned char)mdev.buf[*ppos + i]);

  if (*ppos == 0)      /* запись с начала заменяет содержимое */
    mdev.len = 0;
  *ppos += count;
  if ((size_t)*ppos > mdev.len)
    mdev.len = *ppos;
  mdev.writes++;
  ret = count;
out:
  mutex_unlock(&mdev.lock);
  return ret;
}

const struct file_operations md_fops = {
  .owner   = THIS_MODULE,
  .open    = md_open,
  .release = md_release,
  .read    = md_read,
  .write   = md_write,
  .llseek  = default_llseek,
  .unlocked_ioctl = md_ioctl,
};

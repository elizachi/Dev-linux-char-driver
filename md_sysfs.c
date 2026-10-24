#include <linux/module.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/kernel.h>
#include "md_internal.h"

static ssize_t size_show(struct device *dev, struct device_attribute *attr,
       char *buf)
{
  return sysfs_emit(buf, "%d\n", MD_BUF_SIZE);
}
static DEVICE_ATTR_RO(size);

static ssize_t len_show(struct device *dev, struct device_attribute *attr,
      char *buf)
{
  ssize_t ret;

  mutex_lock(&mdev.lock);
  ret = sysfs_emit(buf, "%zu\n", mdev.len);
  mutex_unlock(&mdev.lock);
  return ret;
}
static DEVICE_ATTR_RO(len);

static ssize_t mode_show(struct device *dev, struct device_attribute *attr,
       char *buf)
{
  ssize_t ret;

  mutex_lock(&mdev.lock);
  ret = sysfs_emit(buf, "%d\n", mdev.mode);
  mutex_unlock(&mdev.lock);
  return ret;
}

static ssize_t mode_store(struct device *dev, struct device_attribute *attr,
        const char *buf, size_t count)
{
  int val, ret;

  ret = kstrtoint(buf, 10, &val);
  if (ret)
    return ret;
  if (val < 0 || val > 1)
    return -EINVAL;

  mutex_lock(&mdev.lock);
  mdev.mode = val;
  mutex_unlock(&mdev.lock);
  return count;
}
static DEVICE_ATTR_RW(mode);

static struct attribute *md_attrs[] = {
  &dev_attr_size.attr,
  &dev_attr_len.attr,
  &dev_attr_mode.attr,
  NULL,
};

static const struct attribute_group md_group = {
  .attrs = md_attrs,
};

int md_sysfs_init(void)
{
  return sysfs_create_group(&mdev.dev->kobj, &md_group);
}

void md_sysfs_exit(void)
{
  sysfs_remove_group(&mdev.dev->kobj, &md_group);
}

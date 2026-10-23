#ifndef MD_INTERNAL_H
#define MD_INTERNAL_H

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/mutex.h>

#define DEVICE_NAME "my_device"
#define MD_BUF_SIZE 4096

struct mydev_state {
  char          *buf;      /* буфер данных */
  size_t         len;      /* сколько байт реально хранится */
  int            mode;     /* 0 - как есть, 1 - заглавные буквы при записи */
  unsigned long  reads;    /* счётчики для /proc */
  unsigned long  writes;
  struct mutex   lock;     /* защищает всё выше */

  dev_t          devno;
  struct cdev    cdev;
  struct class  *cls;
  struct device *dev;
};

extern struct mydev_state mdev;
extern const struct file_operations md_fops;

/* реализуются в следующих коммитах */
long md_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
int  md_proc_init(void);
void md_proc_exit(void);
int  md_sysfs_init(void);
void md_sysfs_exit(void);

#endif

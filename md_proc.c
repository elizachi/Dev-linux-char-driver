#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "md_internal.h"

static struct proc_dir_entry *md_proc_entry;

static int md_proc_show(struct seq_file *m, void *v)
{
  mutex_lock(&mdev.lock);
  seq_printf(m, "device:      %s\n", DEVICE_NAME);
  seq_printf(m, "major:       %d\n", MAJOR(mdev.devno));
  seq_printf(m, "minor:       %d\n", MINOR(mdev.devno));
  seq_printf(m, "buffer size: %d\n", MD_BUF_SIZE);
  seq_printf(m, "data length: %zu\n", mdev.len);
  seq_printf(m, "mode:        %d\n", mdev.mode);
  seq_printf(m, "reads:       %lu\n", mdev.reads);
  seq_printf(m, "writes:      %lu\n", mdev.writes);
  mutex_unlock(&mdev.lock);
  return 0;
}

static int md_proc_open(struct inode *inode, struct file *file)
{
  return single_open(file, md_proc_show, NULL);
}

static const struct proc_ops md_proc_ops = {
  .proc_open    = md_proc_open,
  .proc_read    = seq_read,
  .proc_lseek   = seq_lseek,
  .proc_release = single_release,
};

int md_proc_init(void)
{
  md_proc_entry = proc_create(DEVICE_NAME, 0444, NULL, &md_proc_ops);
  return md_proc_entry ? 0 : -ENOMEM;
}

void md_proc_exit(void)
{
  proc_remove(md_proc_entry);
}

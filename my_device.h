#ifndef MY_DEVICE_H
#define MY_DEVICE_H

#ifdef __KERNEL__
#include <linux/ioctl.h>
#else
#include <sys/ioctl.h>
#endif

#define MY_DEVICE_MAGIC 'k'

#define MYDEV_IOC_CLEAR     _IO(MY_DEVICE_MAGIC, 0)       /* очистить буфер */
#define MYDEV_IOC_GET_SIZE  _IOR(MY_DEVICE_MAGIC, 1, int) /* размер буфера */
#define MYDEV_IOC_GET_LEN   _IOR(MY_DEVICE_MAGIC, 2, int) /* длина данных */
#define MYDEV_IOC_SET_MODE  _IOW(MY_DEVICE_MAGIC, 3, int) /* 0 или 1 */
#define MYDEV_IOC_GET_MODE  _IOR(MY_DEVICE_MAGIC, 4, int)

#endif

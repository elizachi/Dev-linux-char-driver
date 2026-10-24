#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../my_device.h"

#define DEV "/dev/my_device"

static void die(const char *what)
{
  perror(what);
  exit(1);
}

int main(void)
{
  int fd = open(DEV, O_RDWR);
  int size, len, mode;
  char buf[128];
  const char *msg = "hello ioctl";
  ssize_t n;

  if (fd < 0)
    die("open " DEV);

  if (ioctl(fd, MYDEV_IOC_GET_SIZE, &size) < 0)
    die("GET_SIZE");
  printf("buffer size: %d\n", size);

  mode = 1;
  if (ioctl(fd, MYDEV_IOC_SET_MODE, &mode) < 0)
    die("SET_MODE");
  printf("mode set to 1 (uppercase)\n");

  if (write(fd, msg, strlen(msg)) < 0)
    die("write");

  if (ioctl(fd, MYDEV_IOC_GET_LEN, &len) < 0)
    die("GET_LEN");
  printf("data length: %d\n", len);

  lseek(fd, 0, SEEK_SET);
  n = read(fd, buf, sizeof(buf) - 1);
  if (n < 0)
    die("read");
  buf[n] = '\0';
  printf("read back:   \"%s\"\n", buf);

  if (ioctl(fd, MYDEV_IOC_CLEAR) < 0)
    die("CLEAR");
  ioctl(fd, MYDEV_IOC_GET_LEN, &len);
  printf("after CLEAR, data length: %d\n", len);

  mode = 0;
  ioctl(fd, MYDEV_IOC_SET_MODE, &mode);
  close(fd);
  return 0;
}

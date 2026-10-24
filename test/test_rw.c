#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define DEV "/dev/my_device"

int main(int argc, char **argv)
{
  const char *msg = argc > 1 ? argv[1] : "Hello, my_device!";
  char buf[256];
  ssize_t n;
  int fd = open(DEV, O_RDWR);

  if (fd < 0) {
    perror("open " DEV);
    return 1;
  }

  n = write(fd, msg, strlen(msg));
  if (n < 0) {
    perror("write");
    close(fd);
    return 1;
  }
  printf("written: %zd bytes\n", n);

  lseek(fd, 0, SEEK_SET);
  n = read(fd, buf, sizeof(buf) - 1);
  if (n < 0) {
    perror("read");
    close(fd);
    return 1;
  }
  buf[n] = '\0';
  printf("read:    %zd bytes: \"%s\"\n", n, buf);

  close(fd);
  return 0;
}

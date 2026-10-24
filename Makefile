MODULE_NAME := my_device

obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-objs := md_main.o md_fops.o md_ioctl.o md_proc.o md_sysfs.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

TESTS := test/test_rw test/test_ioctl

.PHONY: all load unload reload tests test clean

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

load: all
	sudo insmod $(MODULE_NAME).ko

unload:
	sudo rmmod $(MODULE_NAME)

reload:
	-sudo rmmod $(MODULE_NAME)
	$(MAKE) load

tests: $(TESTS)

test/%: test/%.c my_device.h
	$(CC) -Wall -Wextra -o $@ $<

test: tests
	sudo ./test/test_rw
	sudo ./test/test_ioctl

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f $(TESTS)

obj-m += leap.o

KDIR:= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc -Wall -pthread -o userspace_app userspace_app.c -l LeapC
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -rf userspace_app
install:
	install -m 644 config/blacklist-custom.conf /etc/modprobe.d/blacklist-custom.conf
	update-initramfs -u
	sudo insmod leap.ko
	sudo cp leap.ko /lin/modules/$(shell uname -r)/kernel/drivers/usb/
	sudo depmod -a
	sudo modprobe leap
	echo "Leap Driver Installed, reboot now for changes to take effect"
uninstall:
	sudo rmmod leap
run:
	sudo ./userspace_app

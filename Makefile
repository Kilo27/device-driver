obj-m += leap.o
 
KDIR    := /lib/modules/$(shell uname -r)/build
PWD     := $(shell pwd)
BUILDDIR := $(PWD)/build
 
all: $(BUILDDIR)
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc -Wall -pthread -o $(BUILDDIR)/userspace_app userspace_app.c -lLeapC
	cp leap.ko $(BUILDDIR)/
 
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -rf $(BUILDDIR)/userspace_app $(BUILDDIR)/leap.ko
 
install: all
	install -m 644 config/blacklist-custom.conf /etc/modprobe.d/blacklist-custom.conf
	update-initramfs -u
	sudo insmod $(BUILDDIR)/leap.ko
	sudo cp $(BUILDDIR)/leap.ko /lib/modules/$(shell uname -r)/kernel/drivers/usb/
	sudo depmod -a
	sudo modprobe leap
	echo "Leap Driver Installed, reboot now for changes to take effect"
 
uninstall:
	sudo rmmod leap
	sudo rm -f /lib/modules/$(shell uname -r)/kernel/drivers/usb/leap.ko
	sudo depmod -a
 
run:
	sudo $(BUILDDIR)/userspace_app
 
$(BUILDDIR):
	mkdir -p $(BUILDDIR)
 

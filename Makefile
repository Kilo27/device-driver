obj-m += leap.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	install -m 644 config/blacklist-custom.conf /etc/modprobe.d/blacklist-custom.conf
	update-initramfs -u
	sudo cp leap.ko /lin/modules/$(shell uname -r)/kernel/drivers/usb/
	sudo depmod -a
	sudo modprobe leap
	echo "Leap Driver Installed, reboot now for changes to take effect"

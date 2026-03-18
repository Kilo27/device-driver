obj-m += leap.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	sudo cp leap.ko /lin/modules/$(shell uname -r)/kernel/drivers/usb/
	sudo depmod -a
	sudo modprobe leap

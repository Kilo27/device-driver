BUILD_DIR := $(PWD)/build
obj-m += hello.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(BUILD_DIR) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(BUILD_DIR) clean

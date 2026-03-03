# Specify the build and source directories
BUILD_DIR := $(PWD)/build
SRC_DIR := $(PWD)

# Module name
obj-m := hello.o

# Explicitly reference the source file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(BUILD_DIR) $@

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(BUILD_DIR) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(BUILD_DIR) clean
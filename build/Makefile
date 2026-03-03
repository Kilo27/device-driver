BUILD_DIR := $(PWD)/build
SRC_DIR := $(PWD)

obj-m := leap.o

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(BUILD_DIR) $@

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(BUILD_DIR) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(BUILD_DIR) clean
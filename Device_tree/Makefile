# Define cross-compiler toolchain and kernel source paths
CROSS=/home/manh/work/beagle_bone_black/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
KERNEL=/home/manh/work/beagle_bone_black/kernelbuildscripts/KERNEL/

# Object file to be compiled
obj-m += alloc_module.o

# Build the kernel module
all:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KERNEL) M=$(PWD) modules

# Clean the build
clean:
	make -C $(KERNEL) M=$(PWD) clean

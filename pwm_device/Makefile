obj-m += pwm_device.o
KERN_DIR = /work/2011-12-08/linux-2.6.35.3
all:
	make -C $(KERN_DIR) M=`pwd` modules
	cp pwm_device.ko /nfsroot
	cp pwm_device.ko /tftproot
clear:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm modules.order

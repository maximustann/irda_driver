obj-m += pwm_driver.o
KERN_DIR = /work/2011-12-08/linux-2.6.35.3
all:
	make -C $(KERN_DIR) M=`pwd` modules
	cp pwm_driver.ko /nfsroot
	cp pwm_driver.ko /tftproot
clean:
	make -C  $(KERN_DIR) M=`pwd` modules clean

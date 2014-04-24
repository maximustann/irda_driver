CC=arm-linux-gcc
STRIP=arm-linux-strip
FLAGS=-static

all:
	${CC} pwm_test.c -o pwm_test.o ${FLAGS}
	${CC} read_send_file.c -o read_send_file.o ${FLAGS}
	${STRIP} pwm_test.o read_send_file.o
	mv pwm_test.o pwm_test
	cp pwm_test /tftproot
	cp read_send_file.o  /tftproot

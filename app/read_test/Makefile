CC=arm-linux-gcc
STRIP=arm-linux-strip
FLAGS=-static

all:
	${CC} app.c -o read.o ${FLAGS}
	${STRIP} read.o
	cp read.o /tftproot


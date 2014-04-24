CC=arm-linux-gcc
STRIP=arm-linux-strip
FLAGS=-static

all:
	${CC} pwm_test.c -o pwm_test.o ${FLAGS}
	${STRIP} pwm_test.o
install:
	mkdir -p $(DESTDIR)
	cp pwm_test.o $(DESTDIR)
clean:
	rm -rf *.o

CC=arm-linux-gcc
STRIP=arm-linux-strip
FLAGS=-static

all:
	${CC} app.c -o app.o ${FLAGS}
	${STRIP} app.o
	cp app.o /tftproot
install:
	mkdir -p $(DESTDIR)
	cp app.o $(DESTDIR)
clean:
	rm -rf *.o

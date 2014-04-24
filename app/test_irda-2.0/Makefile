CC=arm-linux-gcc
STRIP=arm-linux-strip
FLAGS=-static

all:
	${CC} app.c -o app.o ${FLAGS}
	${CC} read_send_file.c -o read_send_file.o ${FLAGS}
	${STRIP} app.o read_send_file.o
	cp app.o read_send_file.o /tftproot
install:
	mkdir -p $(DESTDIR)
	cp app.o $(DESTDIR)
clean:
	rm -rf *.o

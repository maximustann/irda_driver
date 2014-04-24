#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define SIZE 128
#define CHANGE_START 74
#define TEST			99

#define US12		0xa
struct pwm_buffer {
	unsigned int channel;
	unsigned int buffer[SIZE];
};


int main(void)
{
	int fd;
	int i;
	struct pwm_buffer my_buffer;
	unsigned char channel;
	unsigned int buffer[SIZE];

	my_buffer.channel = 1;
	my_buffer.buffer[0] = 71;
	my_buffer.buffer[1] = 71;
	my_buffer.buffer[2] = 71;
	my_buffer.buffer[3] = 71;
	my_buffer.buffer[4] = 71;
	my_buffer.buffer[5] = 71;
	my_buffer.buffer[6] = 71;
	my_buffer.buffer[7] = 0x0;

	fd = open("/dev/pwm", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}

//	printf("sizeof(my_buffer) = %d\n", sizeof(my_buffer));	
//	ioctl(fd, TEST);

	while(1)
	{
		
		i += 1;
		write(fd, &my_buffer, sizeof(struct pwm_buffer));	
		printf("i = %d\n", i);
		sleep(1);
	}
	return 0;
}


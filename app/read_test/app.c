#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 512
#define CHANGE_MODE_STUDY			0x101
#define CHANGE_MODE_SEND			0x100
#define TEST 99

struct ir_read {
	unsigned int buffer[SIZE];
};


int main(void)
{
	int i = 0, j;
	int fd, ret;
	struct ir_read read_buffer;	

	fd = open("/dev/pwm", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}
	
	for(j = 0; j < 10000; j++)
	{
		printf("begin to ioctl: %d\n", j);
		ioctl(fd, CHANGE_MODE_STUDY);
		ret = read(fd, &read_buffer, sizeof(struct ir_read));
		for(i = 0; read_buffer.buffer[i]; i++)
		{
			printf("read_buffer.buffer[%d] = %d\n", i, read_buffer.buffer[i]);
		}
	}

	return 0;
}


#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define SIZE 1024
#define CHANGE_START 74
#define CHANGE_MODE_STUDY		0x101
#define CHANGE_MODE_SEND		0x100


struct pwm_buffer {
	unsigned int channel;
	unsigned char buffer[SIZE];
};

struct ir_read {
	unsigned char buffer[SIZE];	
};

int main(void)
{
	int fd;
	int i, j;
	struct pwm_buffer send_buffer;
	struct ir_read read_buffer;
	int ret;
	unsigned char channel;


	fd = open("/dev/pwm", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}

	for(i = 0; i < 300000; i++)
	{
		printf("begin to study :%d\n", i);
		ioctl(fd, CHANGE_MODE_STUDY);
		ret = read(fd, &read_buffer, sizeof(struct ir_read));
		if(ret != -1)
		{
			memcpy(send_buffer.buffer, read_buffer.buffer, sizeof(struct ir_read));
			send_buffer.channel = 0;
			
			ioctl(fd, CHANGE_MODE_SEND);
	//		printf("sizeof(struct pwm_buffer) = %d\n", sizeof(struct pwm_buffer));
			write(fd, &send_buffer, sizeof(struct pwm_buffer));
		}
			memset(&send_buffer, 0, sizeof(struct pwm_buffer));
	}
	return 0;
}


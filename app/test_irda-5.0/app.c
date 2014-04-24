#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define SIZE 			1024
#define CHANGE_MODE_STUDY	0x101
#define CHANGE_MODE_SEND	0x100

struct pwm_buffer {
	unsigned int channel;
	unsigned char buffer[SIZE];
};


int main(void)
{
	int fd;
	int i, j = 5;
	struct pwm_buffer my_buffer;
	unsigned char channel;

	memset(my_buffer.buffer, 0, SIZE);
	my_buffer.channel = 0;
	for(i = 0;  i < 255; i++)
	{
		my_buffer.buffer[i] = 200;
	}



	fd = open("/dev/pwm", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}

	while(1)
	{
		write(fd, &my_buffer, sizeof(struct pwm_buffer));
		sleep(1);
	}


	return 0;
}


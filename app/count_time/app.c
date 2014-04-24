#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CHANGE_READ_REG				75
#define TEST						99

int main(void)
{
	int fd;
	fd = open("/dev/pwm", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}
	ioctl(fd, TEST);	

	return 0;
}


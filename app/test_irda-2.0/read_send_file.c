#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define SIZE 4096
struct file_data {
	unsigned int button;
	unsigned char buffer[SIZE];

};

struct ir_data {
	unsigned char buffer[SIZE];	
};


struct pwm_send {
	unsigned int channel;
	unsigned char buffer[SIZE];
};

int main(void)
{
	int i;
	int fd_file, fd_pwm;
	struct ir_data data;
	struct pwm_send send_data;
	fd_file = open("./irda_data", O_RDWR);
	if(fd_file < 0)
	{
		perror("open");
		return -1;
	}
	fd_pwm = open("/dev/pwm", O_RDWR);
	if(fd_pwm < 0)
	{
		perror("pwm");
		return -1;
	}

	read(fd_file, &data, sizeof(struct file_data));
	
	for(i = 0;; i++)
	{
		if(data.buffer[i - 1] == 0x0 && data.buffer[i] == 0x0)
		{
			break;
		}
		printf("data.buffer[%d] = %d\n", i, data.buffer[i]);
	}	
	
	send_data.channel = 0;
	memset(&send_data.buffer, 0, SIZE);
	
	memcpy(&send_data.buffer, &data.buffer, SIZE);
	write(fd_pwm, &send_data, sizeof(send_data));

	return 0;
}

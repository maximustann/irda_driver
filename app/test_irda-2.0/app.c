#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define SIZE 4096
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


struct ir_file {
	unsigned int button;
	unsigned char buffer[SIZE];
};

struct irda_item {
	int key_name[10];
	unsigned char first_key[SIZE];
	unsigned char second_key[SIZE];
};

int main(void)
{
	int fd, fd_ir;
	int i, j;
	struct pwm_buffer send_buffer;
	struct ir_read read_buffer;
	struct irda_item file_buffer;


	int ret;
	unsigned char channel;


	fd = open("/dev/pwm", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}

	fd_ir = open("./irda_data", O_RDWR | O_CREAT | O_TRUNC);
	if(fd_ir < 0)
	{
		perror("open");
		return -1;
	}
	for(i = 0; i < 300000; i++)
	{
		printf("begin to study~~~~ :%d\n", i);
		ioctl(fd, CHANGE_MODE_STUDY);
		ret = read(fd, &read_buffer, sizeof(struct ir_read));
		if(ret != -1)
		{
//			memcpy(file_buffer.first_key, read_buffer.buffer, sizeof(struct ir_read));
//			memcpy(file_buffer.second_key, read_buffer.buffer, sizeof(struct ir_read));

//			file_buffer.key_name[0] = 2;
			write(fd_ir, &read_buffer, sizeof(struct ir_read));
		}
			memset(&send_buffer, 0, sizeof(struct pwm_buffer));
	}
	return 0;
}


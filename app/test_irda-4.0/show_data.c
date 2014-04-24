#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


struct ir_data {
	char buffer[4096];
};

int main(void)
{
	int i;
	int fd;
	struct ir_data data;
	char buffer[1024];
	fd = open("./irda_data", O_RDWR);
	read(fd, &data, 4096);
	for(i = 0; ; i++)
	{
		if(data.buffer[i - 1] == 0x0 && data.buffer[i] == 0x0)
			break;
		printf("%d\n", data.buffer[i]);
	}

	return 0;
}

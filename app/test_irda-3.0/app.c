#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define SIZE 1024
struct irda_item {
    int key_name[10];
    unsigned char first_key[SIZE];
    unsigned char second_key[SIZE];
};


int main(int argc, char *argv[])
{
	FILE *fp;
	int i;
	struct irda_item buffer;

	if(argc < 1)
	{
		printf("nothing!\n");
		return -1;
	}


	fp = fopen(argv[1], "r");
	if(fp == NULL)
	{
		perror("fopen");
		return -1;
	}

	i = fread(&buffer, 4096, 1, fp);
	printf("%s\n", buffer.key_name);
	return 0;
}



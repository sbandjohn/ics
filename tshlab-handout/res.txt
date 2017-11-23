#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define DEF_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH

int main(){
	int fd = open("aha.txt", O_RDONLY, 0);
	printf("%d\n",fd);
	char c;
	read(fd, &c, 1);
	printf("%c\n",c);
	return 0;
}


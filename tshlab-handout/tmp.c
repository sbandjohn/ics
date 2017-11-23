#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define DEF_MODE S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH

int main(){
	pid_t p1 = getpgrp();
	pid_t p2 = getpid();
	pid_t p3 = getppid();
	printf("pgrp:%d  pid:%d  ppid:%d\n",p1,p2,p3);
	while (1);
	return 0;
}


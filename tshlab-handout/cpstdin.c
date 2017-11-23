#include "csapp.h"

int main(void){
	char c;
	while (Rio_readn(STDIN_FILENO, &c, 1) != 0){
		Rio_writen(STDOUT_FILENO, &c, 1);
	}

	struct stat stat;


	exit(0);
}

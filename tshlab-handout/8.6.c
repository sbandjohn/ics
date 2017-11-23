#include <stdlib.h>
#include <stdio.h>
extern char ** environ;

int main(int argc, char *argv[], char *envp[]){
	printf("Envitonment variables:\n");
	for (int i=0; envp[i]!=NULL; ++i){
		printf("  envp[%d]: %s\n", i, envp[i]);
	}

	printf("-----------------------------------------------------\n");

	for (int i=0; environ[i]!=NULL; ++i){
		printf("  fuck[%d]: %s\n", i, environ[i]);
	}

	return 0;
}



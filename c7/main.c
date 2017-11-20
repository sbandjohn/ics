//#include <stdio.h>

extern int addX;

int add(int x, int y);

char str[] = "abc";
int globalUnInit;

static int globalA = 0x123;

int f1(){
	static int x = 0x1234;
	return x;
}

int f2(){
	static int x;
	return x;
}

int main(){
	static int staticMainB = 0x456;
	int a,b;
	//scanf("%d%d",&a,&b);
	int c = add(a,b) + globalA;
	int d = c + f1() + f2() + staticMainB + addX;
	//printf("%d\n",c);
	return 0;
}


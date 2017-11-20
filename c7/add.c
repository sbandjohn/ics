int addX;

static int globalA=0x666;

int add(int x, int y){
	return x + y + globalA;
}

int junk(int x, int y){
	return 1;
}


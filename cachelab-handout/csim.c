// 林涛 1600012773
#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#define UL unsigned long

int visual;
int s, E, b, S, B, t;
int hit, miss, evic;

int cntAcc;
typedef struct{
	int lastAcc;
	int valid;
	UL tag;
} Block;

Block* cache;

FILE* fin;

void cacheInit(){
	hit = miss = evic = 0;
	cntAcc = 0;
	Block* tmp = cache;
	for (int i=0;i<S;++i)
		for (int j=0;j<E;++j){
			tmp->lastAcc = tmp->valid = 0;
			++tmp;
		}
}

int accessBlock(Block *row, UL tag){
	for (int j=0;j<E;++j)
		if (row[j].valid && row[j].tag == tag){
			++hit;
			if (visual) printf(" hit");
			return 0;
		}
	++miss;
	if (visual) printf(" miss");
	return 1;
}

void markAcc(Block *row, UL tag){
	for (int j=0;j<E;++j)
		if (row[j].tag == tag)
			row[j].lastAcc = ++cntAcc;
}

void loadFromMem(Block *row, UL tag){
	int min = 0x7fffffff;
	int choice;
	for (int j=0;j<E;++j){
		if (row[j].valid == 0){
			choice = j;
			break;
		}else if (row[j].lastAcc < min){
			choice = j;
			min = row[j].lastAcc;
		}
	}
	if (row[choice].valid){
		++evic;
		if (visual) printf(" eviction");
	}
	row[choice].valid = 1;
	row[choice].tag = tag;
}

Block* getRow(UL addr){
	unsigned setID = (addr>>b)&(S-1);
	return cache + setID*E;
}

UL getTag(UL addr){
	return addr>>(s+b);
}

void load(UL addr, int sz){
	if (visual) printf("L %lx  %d ",  addr, sz);
	Block *row = getRow(addr);
	UL tag = getTag(addr);
	int ifMiss = accessBlock(row, tag);
	if (ifMiss) loadFromMem(row, tag);
	markAcc(row, tag);
	if (visual) printf("\n");
}

void store(UL addr, int sz){
	if (visual) printf("S %lx  %d",  addr, sz);
	Block *row = getRow(addr);
	UL tag = getTag(addr);
	int ifMiss = accessBlock(row, tag);
	if (ifMiss) loadFromMem(row, tag);
	markAcc(row, tag);
	if (visual) printf("\n");
}

void modify(UL addr, int sz){
	if (visual) printf("M %lx  %d\n",  addr, sz);
	Block *row = getRow(addr);
	UL tag = getTag(addr);
	if (accessBlock(row, tag)) loadFromMem(row, tag);
	accessBlock(row, tag);
	markAcc(row, tag);
	if (visual) printf("\n");
}

void run(){
	S = 1<<s;
	B = 1<<b;
	t = 64-s-b;

	char str[200];
	char acc;
	UL addr;
	int sz;

	int C = S*E;
	cache = (Block*)malloc(C*sizeof(Block));

	cacheInit();

	while (fgets(str, 200, fin)!=NULL){
		if (str[0]!=' ') continue;
		sscanf(str, " %c %lx,%d", &acc, &addr, &sz);
		switch (acc){
			case 'M': modify(addr,sz);	break;
			case 'L': load(addr,sz);	break;
			case 'S': store(addr,sz);	break;
		}
	}
	
	free(cache);
}

void printHelp(){
	printf("代码风格分给满好不好？\n");
}

int main(int argc, char *argv[]){
	visual = 0;
	char opt;
	while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1){
		switch (opt){
			case 'v': visual = 1;		break;
			case 's': s = atoi(optarg);	break;
			case 'E': E = atoi(optarg);	break;
			case 'b': b = atoi(optarg);	break;
			case 't': fin = fopen(optarg, "r");	break;
			case 'h': printHelp();	break;
			default:
				printHelp();
				exit(EXIT_SUCCESS);
		}
	}

	run();
	fclose(fin);

	printSummary(hit,miss,evic);
	exit(EXIT_SUCCESS);
}


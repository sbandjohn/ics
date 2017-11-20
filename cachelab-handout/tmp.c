#//林涛 1600012773
/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>

void transpose_32(int M, int N, int A[N][M], int B[M][N]);
void transpose_diagonal8(int i, int M, int N, int A[N][M], int B[N][M]);

void transpose_61(int M, int N, int A[N][M], int B[M][N]){
	B[0][0]=A[0][0];
	int h = 8;
	int w = 8;
	int cnt = 0;
	for (int i=0;i<N;i+=h){
		for (int j=0;j<M;j+=w){
			for (int ii=i;ii<N && ii<i+h;++ii)
				for (int jj=j;jj<M && jj<j+h;++jj){
					B[jj][ii]=A[ii][jj];
					++cnt;
					printf("%d %d\n",ii,jj);
				}
		}
	}
	printf("%d %d\n",cnt, N*M);
}


/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{

/*	int tmp;
	for (int i=0;i<32;++i)
		for (int j=0;j<32;++j)
		tmp = A[i][j];
	B[0][0] = tmp;
*/
	//if (M==32) transpose_32(M,N,A,B);
	for (int i=0;i<32;++i)
		for (int j=0;j<32;++j)
			B[j][i] = A[i][j];

}

char transpose_32_desc[] = "Transpose 32";
void transpose_32(int M, int N, int A[N][M], int B[M][N]){

	for (int i=0; i<32; i+=8){
		for (int j=0;j<32;j+=8){
			if (i!=j){
				for (int k=0;k<8;++k){
					for (int l=0;l<8;++l){
						B[j+l][i+k] = A[i+k][j+l];
					}
				}
			}
		}
	}
	/*
	for (int i=0;i<8;++i)
		for (int j=0;j<8;++j)
			B[8+j][8+i] = A[i][j];
	for (int i=0;i<8;++i)
		for (int j=0;j<8;++j)
			B[24+j][24+i] = A[16+i][16+j];
	for (int i=0;i<8;++i)
		for (int j=0;j<8;++j)
			B[i][j] = B[8+i][8+j];
	for (int i=0;i<8;++i)
		for (int j=0;j<8;++j)
			B[16+i][16+j] = B[24+i][24+j];
	for (int i=0;i<8;++i)
		for (int j=0;j<8;++j)
			B[24+j][24+i] = A[8+i][8+j];
	for (int i=0;i<8;++i)
		for (int j=0;j<8;++j)
			B[8+j][8+i] = A[24+i][24+j];
	for (int i=0;i<8;++i)
		for (int j=0;j<8;++j){
			int tmp = B[8+i][8+j];
			B[8+i][8+j] = B[24+i][24+j];
			B[24+i][24+j] = tmp;
		}
*/
	for (int i=0;i<32;i+=8) transpose_diagonal8(i, M, N, A, B);

}

void transpose_diagonal8(int i, int M, int N, int A[N][M], int B[M][N]){
	int a = A[i][i+0];
	int b = A[i][i+1];
	int c = A[i][i+2];
	int d = A[i][i+3];
	int e = A[i][i+4];
	int f = A[i][i+5];
	int g = A[i][i+6];
	int h = A[i][i+7];
	for (int k=i+1;k<i+8;++k)
		for (int l=i+0;l<i+8;++l)
			B[k-1][l] = A[k][l];
	for (int k=i+7;k>=i+1;--k)
		for (int l=i+0;l<i+8;++l)
			B[k][l] = B[k-1][l];
	B[i][i+0] = a;
	B[i][i+1] = b;
	B[i][i+2] = c;
	B[i][i+3] = d;
	B[i][i+4] = e;
	B[i][i+5] = f;
	B[i][i+6] = g;
	B[i][i+7] = h;
	for (int k=i;k<i+8;++k){
		for (int l=i;l<i+8;++l){
			printf("%d ",A[k][l]);
		}
		printf("\n");
	}
	printf("\nB\n");
	for (int k=i;k<i+8;++k){
		for (int l=i;l<i+8;++l){
			printf("%d ",B[k][l]);
		}
		printf("\n");
	}
	printf("\n\n");

	for (int k=i;k<i+8;++k)
		for (int l=k+1;l<i+8;++l){
			a = B[k][l];
			B[k][l] = B[l][k];
			B[l][k] = a;
		}
}

void transpose_64(int M, int N, int A[N][M], int B[M][N]){

}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 
int A[61][67];
int B[67][61];

int main(){
	int N=61;
	int M=67;
	/*printf("%3d ",-1);
	for (int j=0;j<M;++j){
			printf(":%3d     ",j);
	}
	printf("\n");
	
	for (int i=0;i<N;++i){
		printf("%3d ",i);
		for (int j=0;j<M;++j){
			int id = i*M+j;
			int block = id/8;
			printf("[%3d,%3d]",block,block%32);
		}
		printf("\n");
	}
*/
	for (int i=0;i<N;++i) for (int j=0;j<M;++j) A[i][j]=i*M+j;
	transpose_61(M,N,A,B);
	for (int i=0;i<N;++i){
		for (int j=0;j<M;++j)
			printf("% 4d",A[i][j]);
		printf("\n");
	}
	printf("\n\n");
	for (int i=0;i<N;++i){
		for (int j=0;j<M;++j)
			printf("% 4d",B[i][j]);
		printf("\n");
	}
	return 0;
}


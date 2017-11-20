//林涛 1600012773
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
#include "cachelab.h"
#include "contracts.h"

void transpose_32(int M, int N, int A[N][M], int B[M][N]);

void transpose_64(int M, int N, int A[N][M], int B[M][N]);
void transpose_61(int M, int N, int A[N][M], int B[M][N]);

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

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
    REQUIRES(M > 0);
    REQUIRES(N > 0);

	if (M==32) transpose_32(M,N,A,B);
	if (M==64) transpose_64(M,N,A,B);
	if (M==61) transpose_61(M,N,A,B);
    ENSURES(is_transpose(M, N, A, B));
}

/*
   32*32
   分成8*8的块，共4*4个块。每个块都能完全存放在缓存中。
   非对角线上块的直接转置。
   对角线上的，将A的块复制到B的块，保证各加载一次，然后在B中转置。
   复制的方法是：用8个变量保存A的第一行，然后把A的其他行向上平移到B中，然后B向下平移，最后补充第一行
*/
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
	for (int k=i;k<i+8;++k)
		for (int l=k+1;l<i+8;++l){
			a = B[k][l];
			B[k][l] = B[l][k];
			B[l][k] = a;
		}
}

void transpose_32(int M, int N, int A[N][M], int B[M][N]){
	for (int i=0; i<32; i+=8)
		for (int j=0;j<32;j+=8)
			if (i!=j)
				for (int k=0;k<8;++k)
					for (int l=0;l<8;++l)
						B[j+l][i+k] = A[i+k][j+l];
	for (int i=0;i<32;i+=8) transpose_diagonal8(i, M, N, A, B);
}


/*
   64*64
   分成8*8的块，但是每一块的上下两部分（各4*8）不能同时被缓存。
   对于非对角线上的块，可以利用B中一个4*8的块，实现转置且只加载一次。
   对于对角线上的块，比如(i,i)（左上角为第i行第i列的8*8块），可以利用B中两个4*8的块实现转置，
   这两个块不能在同一个8*8块中，且不能与(i,i)在同一列，
   我选用了(0,0)和(8,8)两块（就是对角线左上角的两块）
   最后将(0,0)与(8,8)所在的8*8块转置，方法是：
   先将A的(0,0)和(8,8)复制到B的(8,8)和(0,0)，把B中的这两块再分成4*4的8块，再根据特定的顺序交换转置这些块
*/

void copy_4(int i,int j, int p,int q, int A[64][64], int B[64][64]){
	for (int x=0;x<4;++x)
		for (int y=0;y<4;++y)
			B[p+x][q+y] = A[i+x][j+y];
}

void transpose_copy_4(int i,int j, int p, int q, int A[64][64], int B[64][64]){
	for (int x=0;x<4;++x)
		for (int y=0;y<4;++y)
			B[p+y][q+x] = A[i+x][j+y];
}

void transpose_64_8_non_diagonal_by_block(int x,int y, int i,int j, int A[64][64], int B[64][64]){
	transpose_copy_4(i,j,j,i, A,B);
	copy_4(i,j+4,x,y+4,A,B);
	transpose_copy_4(i+4,j, j,i+4, A,B);
	copy_4(i+4,j+4,x,y,A,B);
	transpose_copy_4(x,y+4, j+4,i, B,B);
	transpose_copy_4(x,y, j+4,i+4, B,B);
}

void transpose_64_8_by_2_block(int x,int y, int p,int q, int i, int j, int A[64][64],int B[64][64]){
	copy_4(i,j,x,y,A,B);
	copy_4(i,j+4,x,y+4,A,B);
	copy_4(i+4,j,p,q,A,B);
	copy_4(i+4,j+4,p,q+4,A,B);
	transpose_copy_4(x,y,j,i,B,B);
	transpose_copy_4(p,q,j,i+4,B,B);
	transpose_copy_4(x,y+4,j+4,i,B,B);
	transpose_copy_4(p,q+4,j+4,i+4,B,B);
}

void transpose_swap_4(int i,int j,int p,int q,int B[64][64]){
	for (int x=0;x<4;++x)
		for (int y=0;y<4;++y){
			int tmp = B[i+x][j+y];
			B[i+x][j+y] = B[p+y][q+x];
			B[p+y][q+x] = tmp;
		}
}

void transpose_swap_64_2_block(int i,int j, int p,int q, int B[64][64]){
	transpose_swap_4(i+4,j, p,q+4, B);
	transpose_swap_4(p,q, i,j, B);
	transpose_swap_4(i,j+4, p+4,q, B);
	transpose_swap_4(p+4,q+4, i+4,j+4, B);
}

void transpose_64(int M, int N, int A[N][M], int B[M][N]){
	for (int i=8;i<N;i+=8)
		for (int j=8;j<M;j+=8)
			if (i!=j) transpose_64_8_non_diagonal_by_block(0,0, i,j, A,B);
	
	transpose_64_8_non_diagonal_by_block(2*8, 2*8, 0,8, A,B);
	transpose_64_8_non_diagonal_by_block(2*8, 2*8, 8,0, A,B);
	
	for (int i=2*8;i<N;i+=8){
		transpose_64_8_non_diagonal_by_block(8,8, 0,i, A,B);
		transpose_64_8_non_diagonal_by_block(8,8, i,0, A,B);
	}

	for (int i=2*8;i<N;i+=8) transpose_64_8_by_2_block(0,0, 8,8, i,i, A,B);

	for (int i=0;i<8;++i)
		for (int j=0;j<8;++j) B[8+i][8+j] = A[i][j];
	for (int i=0;i<8;++i)
		for (int j=0;j<8;++j) B[i][j] = A[8+i][8+j];
	transpose_swap_64_2_block(0,0, 8,8, B);
}

/*
   61*67
*/
void transpose_61(int M, int N, int A[N][M], int B[M][N]){
	int h = 12;
	int w = 1;
	for (int i=0;i<N; i+=h){
		for (int j=0;j<M; j+=w){
			int a,b,c,d,e,f,g,k;
			if (i+0<N){
				a = A[i+0][j];
				//accessA(i+0,j);
			}
			if (i+1<N){
				b = A[i+1][j];
				//accessA(i+1,j);
			}
			if (i+2<N){
				c = A[i+2][j];
				//accessA(i+2,j);
			}
			if (i+3<N){
				d = A[i+3][j];
				//accessA(i+3,j);
			}
			if (i+4<N){
				e = A[i+4][j];
				//accessA(i+4,j);
			}
			if (i+5<N){
				f = A[i+5][j];
				//accessA(i+5,j);
			}
			if (i+6<N){
				g = A[i+6][j];
				//accessA(i+6,j);
			}
			if (i+7<N){
				k = A[i+7][j];
				//accessA(i+7,j);
			}

			if (i+0<N){
				B[j][i+0] = a;
				//accessB(j,i+0);
			}
			if (i+1<N){
				B[j][i+1] = b;
				//accessB(j,i+1);
			}
			if (i+2<N){
				B[j][i+2] = c;
				//accessB(j,i+2);
			}
			if (i+3<N){
				B[j][i+3] = d;
				//accessB(j,i+3);
			}
			if (i+4<N){
				B[j][i+4] = e;
				//accessB(j,i+4);
			}
			if (i+5<N){
				B[j][i+5] = f;
				//accessB(j,i+5);
			}
			if (i+6<N){
				B[j][i+6] = g;
				//accessB(j,i+6);
			}
			if (i+7<N){
				B[j][i+7] = h;
				//accessB(j,i+7);
			}

			for (int a = i+8; a<i+h && a<N; ++a){
				B[j][a] = A[a][j];
				//accessA(a,j);
				//accessB(j,a);
			}
/*
			for (int ii=i; ii<N && ii<i+h; ++ii){
				for (int jj=j; jj<M && jj<j+1; ++jj){
					accessA(ii,jj);
					accessB(jj,ii);
	//				B[jj][ii] = A[ii][jj];
	//				cout<<'('<<ii<<','<<jj<<')'<<endl;
				}
			}
*/
		}
	}
	//return miss;
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

	//registerTransFunction(transpose_32, transpose_32_desc);
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}


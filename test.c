#include <stdio.h>
#include <math.h>
#include <stdlib.h>


int main() {


int matrix[16];
int temp_matrix[16];


for(int i=0;i<16;i++)
	temp_matrix[i] = i;


printf("\n\n");


for(int i=0;i<16;i++){
	matrix[i] = temp_matrix[16-i-1];
	//printf("%d\n", 16-i-1);
}


printf("\n\n");


for(int i=15;i>-1;i--)
	printf("%d\n", matrix[i]);

}

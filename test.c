#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define BOX 4

int main() {


int matrix[BOX][BOX];
int temp_matrix[BOX][BOX];
int ind = 1;
int vect[BOX*BOX];

for(int i=0; i<BOX; i++) {
	for(int j=0; j<BOX; j++) {
		matrix[i][j] = 0;
	}
}


for(int i=0; i<BOX; i++) {
	for(int j=0; j<BOX; j++) {
		temp_matrix[i][j] = ind;
		ind++;
	}
}


for(int i=0; i<BOX; i++) { 
	for(int j=0; j<BOX; j++)
		printf("%d\t", temp_matrix[i][j]);
	printf("\n");
}

printf("\n\n");

ind = 0;
for(int i = 0; i < BOX; i++) {
	for(int j = 0; j < BOX; j++) {
		vect[ind] = temp_matrix[j][BOX-i-1];
		ind++;
	}
}


for(int j=0; j<BOX*BOX; j++)
	printf("%d\t", vect[j]);
printf("\n");



printf("\n\n");





ind = 0;
for(int i = 0; i < BOX; i++) {
	for(int j = 0; j < BOX; j++) {
		temp_matrix[i][j] = vect[ind];
		ind++;
	}
}

for(int i=0; i<BOX; i++) { 
	for(int j=0; j<BOX; j++)
		printf("%d\t", temp_matrix[i][j]);
	printf("\n");
}



}

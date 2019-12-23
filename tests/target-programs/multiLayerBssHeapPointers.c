#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>



/*
 3 global bss pointers to heap memory which then contain various other heap pointers
A->C->F,G;
A->F->Z,X,Y;
B->P;
U->int;
*/



struct f{
	int* z;
	int* x;
	int* y;
};
struct c {
	struct f* F;
	int* g;
};

struct b{
	int* p;
};


struct c* A;
struct b* B;
int* U;

int main(){
	A = malloc(sizeof(struct c));
	B = malloc(sizeof(struct b));
	B->p=malloc(sizeof(int));
	U = malloc(sizeof(int));

	struct f* z = malloc(sizeof(struct f));
	printf("%s%ld\n","z size = ",sizeof(struct f));
	z->x=malloc(sizeof(int));
	z->z=malloc(sizeof(int));
	z->y=malloc(sizeof(int));

	A->g=malloc(sizeof(int));
	A->F=z;
	A->F->z=malloc(sizeof(int));

	printf("All allocated\n");
	sleep(3600);
}

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int* ptr1;
int* ptr2;
int* ptr3;
int* ptr4;
int* ptr5;

int main(){
	ptr1=malloc(sizeof(int));
	ptr2=malloc(sizeof(int));
	ptr3=malloc(sizeof(int));
	ptr4=malloc(sizeof(int));
	ptr5=malloc(sizeof(int));

	*ptr1=5;
	*ptr2=10;
	*ptr3=15;
	*ptr4=20;
	*ptr5=25;
	printf("5 pointers allocated and initialised\n");
	sleep(3600);
}

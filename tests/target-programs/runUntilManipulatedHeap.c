#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	printf("runUntilManipluatedHeap started\n");
	volatile u_int32_t* x=malloc(sizeof(u_int32_t));
	*x= 127127;

	while (*x == 127127){
		sleep(1);
	}

	return 0;
}

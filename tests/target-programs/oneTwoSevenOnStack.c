#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	volatile u_int32_t y= 127127;

	//Force heap to be initialised
	int* x=malloc(sizeof(int));
	*x=99999;

	while (1){
		sleep(1);
	}

	return 0;
}

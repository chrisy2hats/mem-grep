#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	volatile u_int32_t x= 127127;

	while (x == 127127){
		sleep(1);
	}

	return 0;
}

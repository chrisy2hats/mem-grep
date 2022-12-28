//Simple C program with an unsigned 32 bit integer with a constant value on the stack
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
        register void *sp asm ("sp");
        printf("%s%p\n","stackPointer=", sp);
	pid_t PID = getpid();
	printf("%s%u\n","PID=",PID);

	u_int32_t x = 127;
	printf("%s%p\n","X has been put at :",(void*) &x);
	while (1){
                //Stops compiler inlining value as it could change
		if ((u_int32_t) rand() == x){
			x++;
			printf("X increased\n");
		}else{
		printf("%d\n",x);
		}

                sleep(2);
	}
}

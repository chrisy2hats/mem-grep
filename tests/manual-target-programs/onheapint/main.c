//
// Created by cflaptop on 04/10/2019.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(){

    register void *sp asm ("sp");
    printf("%s%p\n","stackPointer=", sp);
    pid_t PID = getpid();
    printf("%s%u\n","PID=",PID);

    u_int32_t* x = malloc(sizeof(u_int32_t));
    *x=127;
    printf("%s%p\n","X has been put at :",x);
    while (1){
        //Stops compiler inlining value as it could change
        if (rand() == *x){
            (*x)++;
            printf("X increased\n");
        }else{
            printf("%d\n",*x);
        }

        sleep(2);
    }
    return 0;
}
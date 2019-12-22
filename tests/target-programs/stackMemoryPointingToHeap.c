#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int* foobar(int x){
	if (x==5){
		sleep(3600);
	}
	int* i = malloc(sizeof(int));
	*i=x;
	printf("%s%p%s%p%s%d\n","heap int allocated at:",i," which is pointed to by:",&i," with value:",*i);

	foobar(x+1);
	return i;
}
int main(){
	foobar(0);
	printf("EOM\n");
	sleep(3600);

}

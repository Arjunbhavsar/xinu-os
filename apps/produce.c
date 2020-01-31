#include <xinu.h>
#include <prodcons.h>

void producer(int count) {

    // Iterates from 0 to count, setting
    // the value of the global variable 'n'
    // each time.
    // print produced value e.g. produced : 8
   
    for(int j=0;j<=count;j++){
	//wait(consume1);
        n= j;
	printf("\n produced : %d",n);
	//signal(produce1);

    }
}




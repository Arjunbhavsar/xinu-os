#include <xinu.h>
#include <prodcons.h>
#include <string.h>
#include <prodcons_bb.h>
#include <stdlib.h>

void consumer(int count) {

  // reads the value of the global variable 'n'
  // 'count' times.
  // print consumed value e.g. consumed : 8

   for(int i=0;i<=count ; i++){
      //wait(produce1);
	printf("\n consumed :%d",n);
      //signal(consume1);
   }
}

void consumer_bb(int count) {
  // Iterate from 0 to count and for each iteration read the next available value from the global array `arr_q`
  // print consumer process name and read value as,
  // name : consumer_1, read : 8
 int temp;

 for (int i =0; i<= count;i++){
	wait(produce);
	// Consuming the value from global Array
	temp = arr_q[(consumerIndices++)%5]; 
	
	// Get the process name 
	char *processName = proctab[getpid()].prname; 

	printf("name : %s , read: %d\n",processName,temp);
	signal(consume);
	}
}

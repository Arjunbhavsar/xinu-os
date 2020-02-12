#include <xinu.h>
#include <prodcons_bb.h>
#include <string.h>
#include <prodcons.h>
#include <stdlib.h>

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

void producer_bb(int count) {
  // Iterate from 0 to count and for each iteration add iteration value to the global array `arr_q`, 
  // print producer process name and written value as,
  // name : producer_1, write : 8

  for (int j =0 ;j<=count; j++){
	 
	  wait(consume);
	  
	  // Assigning value to global variable
	  arr_q[(producerIndices++)%5]= j;
	  
	  //Read Name of the process
	  char *processName = proctab[getpid()].prname;
		
	  //printing  
	  printf("name: %s, write : %d\n",processName, j);
	 
	  signal(produce);
  }
}


#include <xinu.h>
#include <prodcons.h>

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

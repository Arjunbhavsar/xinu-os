/* xsh_run.c - xsh_run */

#include <xinu.h>
#include <string.h>
#include <stdio.h>
#include <prodcons_bb.h>
#include <ctype.h>
/*------------------------------------------------------------------------
 * xsh_run - obtain and print the current month, day, year, and time
 *------------------------------------------------------------------------
 */

int arr_q[5];
int producerIndices;
int consumerIndices;
sid32 consume,produce;

void prodcons_bb(int nargs, char *args[]) { 
  //create and initialize semaphores to necessary values 
 
   produce= semcreate(0); 
   consume= semcreate(1); 
   
  int producerProcess = atoi(args[1]); 
  int consumerProcess = atoi(args[2]); 
  int producerCount = atoi(args[3]); 
  int consumerCount = atoi(args[4]); 
 
  producerIndices = 0; 
  consumerIndices = 0; 
 
  //initialize read and write indices for the queue 
  for(int i=1; i<=producerProcess; i++){ 
        char name[10] ;
        sprintf(name, "producer_%d",i);	
        resume( create(producer_bb, 1024, 20, name, 1,producerCount)); 
  } 
  for(int j =1; j<=consumerProcess; j++ ){ 
	 char funcName[10];
	 sprintf(funcName, "consumer_%d",j);
         resume( create(consumer_bb, 1024, 20, funcName,1, consumerCount)); 
  } 
  //create producer and consumer processes and put them in ready queue 
 
  return (0); 
}

shellcmd xsh_run(int nargs, char *args[]) {


if ((nargs == 1) || (strncmp(args[1], "list", 5) == 0)) {
      printf("prodcons_bb\n");
     // printf("my_function_2\n");
      return OK;
    }
     if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tRun Command will Allow to to execute multiple commands such as Proodcons_bb \n");
		printf("Options (one per invocation):\n");
		printf("\t--help\tdisplay this help and exit\n");
		return 0;
	}

	/* Check argument count */

	if (nargs > 6) {
		fprintf(stderr, "%s: too many arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
	}
	if(nargs<6 ){
		fprintf(stderr, "%s: too less arguments\n", args[0]);
		fprintf(stderr, "Try '%s --help' for more information\n",
			args[0]);
		return 1;
	}
	
   // if (isdigit(args[2]) == isdigit(args[3])== isdigit(args[4]) == isdigit(args[5]) == 0){
	    /* This will go past "run" and pass the function/process name and its
   	 * arguments.
   	 */
   	 args++;
   	 nargs--;
    
   	 int numProducerPro = atoi(args[1]);
   	 int numConsumerPro = atoi(args[2]);
   	 int producerCount = atoi(args[3]);
   	 int consumerCount = atoi(args[4]);

    	if((numProducerPro * producerCount) == (numConsumerPro * consumerCount)){

 	   	if(strncmp(args[0], "prodcons_bb", 13) == 0) {
      			/* create a process with the function as an entry point. */
	      		resume (create((void *)prodcons_bb, 4096, 20, "prodcons_bb", 2, nargs, args));
    		}
   	 }else{
		printf("Mismatch in iteration count of producer and consumer");
   	 }
//  }else{
 //	 printf("Enter Valid Arguements.");
 // }
}

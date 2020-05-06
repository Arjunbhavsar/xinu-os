/* xsh_run.c - xsh_run */

#include <xinu.h>
#include <string.h>
#include <stdio.h>
#include <prodcons_bb.h>
#include <ctype.h>
#include <future.h>
#include <future_test.h>
#include <stream.h>
#include <fstest.h>
#include <tree.h>
/*------------------------------------------------------------------------
 * xsh_run - obtain and print the current month, day, year, and time
 *------------------------------------------------------------------------
 */

int arr_q[5];
int producerIndices;
int consumerIndices;
sid32 consume,produce;

void prodcons_bb(int nargs, char *args[]) { 
/* Check argument count */
/*
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
	}*/
//create and initialize semaphores to necessary values 
   produce= semcreate(0); 
   consume= semcreate(1); 
   
  int producerProcess = atoi(args[1]); 
  int consumerProcess = atoi(args[2]); 
  int producerCount = atoi(args[3]); 
  int consumerCount = atoi(args[4]); 
 
  producerIndices = 0; 
  consumerIndices = 0; 

   if((producerProcess * producerCount) == (consumerProcess * consumerCount)){
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
    }else{
	printf("Mismatch in iteration count of producer and consumer");
    }
  return (0); 
}


shellcmd xsh_run(int nargs, char *args[]) {
   	if ((nargs == 1) || (strncmp(args[1], "list", 5) == 0)) {
     		 printf("prodcons_bb\n");
     		 printf("futures_test\n");
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

        /* This will go past "run" and pass the function/process name and its
   	 * arguments.
   	 */
   	 args++;
   	 nargs--;

	
	if(strncmp(args[0], "rbt", 3) == 0) {
	/* create a process with the function as an entry point. */
		resume (create((void *)redBlackTreeTest, 4096, 20, "fstest", 2, nargs, args));
	}
    
	if(strncmp(args[0], "fstest", 6) == 0) {
	/* create a process with the function as an entry point. */
		resume (create((void *)fstest, 4096, 20, "fstest", 2, nargs, args));
	}
	if(strncmp(args[0], "prodcons_bb", 13) == 0) {
	/* create a process with the function as an entry point. */
		resume (create((void *)prodcons_bb, 4096, 20, "prodcons_bb", 2, nargs, args));
	}
	if(strncmp(args[0], "tscdf_fq", 8) == 0) {
		
		resume (create(stream_proc_futures, 4096, 20, "stream_proc", 2, nargs, args));
	
	}
	if(strncmp(args[0], "tscdf", 13) == 0) {
		
		resume (create(stream_proc, 4096, 20, "stream_proc", 2, nargs, args));
	
	}
	if(strncmp(args[0], "futures_test", 13) == 0) {
		/* create a process with the function as an entry point. */

		if(strncmp(args[1],"-pc",13) == 0){
			
			future_t* f_exclusive, * f_shared;
			f_exclusive = future_alloc(FUTURE_EXCLUSIVE, sizeof(int), 1);
			f_shared    = future_alloc(FUTURE_SHARED, sizeof(int), 1);
			
		// Test FUTURE_EXCLUSIVE
			resume( create(future_cons, 1024, 20, "fcons1", 1, f_exclusive) );
			resume( create(future_prod, 1024, 20, "fprod1", 2, f_exclusive, (char*) &one) );

			// Test FUTURE_SHARED
			resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) );
			resume( create(future_cons, 1024, 20, "fcons3", 1, f_shared) );
			resume( create(future_cons, 1024, 20, "fcons4", 1, f_shared) );
			resume( create(future_cons, 1024, 20, "fcons5", 1, f_shared) );
			resume( create(future_prod, 1024, 20, "fprod2", 2, f_shared, (char*) &two) );


		}
		else if ( strncmp(args[1],"-f",13)==0){
			
			int fib = -1, i;
			fib = atoi(args[2]);

			if (fib > -1) {
				int final_fib;
				int future_flags = FUTURE_SHARED; // TODO - add appropriate future mode here

			// create the array of future pointers
				if ((fibfut = (future_t **)getmem(sizeof(future_t *) * (fib + 1)))
					== (future_t **) SYSERR) {
					printf("getmem failed\n");
					return(SYSERR);
				}
			// get futures for the future array
				for (i=0; i <= fib; i++) {
					if((fibfut[i] = future_alloc(future_flags, sizeof(int), 1)) == (future_t *) SYSERR) {
					printf("future_alloc failed\n");
					return(SYSERR);
					}
				}

			// spawn fib threads and get final value
			// TODO - you need to add your code here
			// printf("\nSpawn ffib\n");
			// Spwning fib number of threads with seprate inputs 
			for(int j=0;j<=fib; j++ ){
				resume(create(ffib, 4096, 20, "ffib",     1, j));
			}
			// printf("Process Resumed value recieved \n\n");

			future_get(fibfut[fib], (char*) &final_fib);

			for (i=0; i <= fib; i++) {
				future_free(fibfut[i]);
				}

			freemem((char *)fibfut, sizeof(future_t *) * (fib + 1));
			printf("Nth Fibonacci value for N=%d is %d\n", fib, final_fib);
				return(OK);
			}
		}else if ( strncmp(args[1],"-fq1",13)==0){
			int three = 3, four = 4, five = 5, six = 6;
			future_t *f_queue;
			f_queue = future_alloc(FUTURE_QUEUE, sizeof(int), 3);

			resume(create(future_cons, 1024, 20, "fcons6", 1, f_queue));
			resume(create(future_cons, 1024, 20, "fcons7", 1, f_queue));
			resume(create(future_cons, 1024, 20, "fcons8", 1, f_queue));
			resume(create(future_cons, 1024, 20, "fcons9", 1, f_queue));
			resume(create(future_prod, 1024, 20, "fprod3", 2, f_queue, (char *)&three));
			resume(create(future_prod, 1024, 20, "fprod4", 2, f_queue, (char *)&four));
			resume(create(future_prod, 1024, 20, "fprod5", 2, f_queue, (char *)&five));
			resume(create(future_prod, 1024, 20, "fprod6", 2, f_queue, (char *)&six));
			sleep(1);

		}else if ( strncmp(args[1],"-fq2",13)==0){
			int seven = 7, eight = 8, nine=9, ten = 10, eleven = 11;
			future_t *f_queue;
			f_queue = future_alloc(FUTURE_QUEUE, sizeof(int), 3);

			resume(create(future_prod, 1024, 20, "fprod10", 2, f_queue, (char *)&seven));
			resume(create(future_prod, 1024, 20, "fprod11", 2, f_queue, (char *)&eight));
			resume(create(future_prod, 1024, 20, "fprod12", 2, f_queue, (char *)&nine));
			resume(create(future_prod, 1024, 20, "fprod13", 2, f_queue, (char *)&ten));
			resume(create(future_prod, 1024, 20, "fprod13", 2, f_queue, (char *)&eleven));

			resume(create(future_cons, 1024, 20, "fcons14", 1, f_queue));
			resume(create(future_cons, 1024, 20, "fcons15", 1, f_queue));
			resume(create(future_cons, 1024, 20, "fcons16", 1, f_queue));
			resume(create(future_cons, 1024, 20, "fcons17", 1, f_queue));
			resume(create(future_cons, 1024, 20, "fcons18", 1, f_queue));
			sleep(1);
			
		}else if ( strncmp(args[1],"-fq3",13)==0){
			
			int three = 3, four = 4, five = 5, six = 6;
			future_t *f_queue;
			f_queue = future_alloc(FUTURE_QUEUE, sizeof(int), 3);

			resume( create(future_cons, 1024, 20, "fcons6", 1, f_queue) );
			resume( create(future_prod, 1024, 20, "fprod3", 2, f_queue, (char*) &three) );
			resume( create(future_prod, 1024, 20, "fprod4", 2, f_queue, (char*) &four) );
			resume( create(future_prod, 1024, 20, "fprod5", 2, f_queue, (char*) &five) );
			resume( create(future_prod, 1024, 20, "fprod6", 2, f_queue, (char*) &six) );
			resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
			resume( create(future_cons, 1024, 20, "fcons8", 1, f_queue) );
			resume( create(future_cons, 1024, 20, "fcons9", 1, f_queue) );
			sleep(1);
		}

	}
  }

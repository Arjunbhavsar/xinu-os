#include <xinu.h>
#include <prodcons.h>

int n;                 //Definition for global variable 'n'
//sid32 produce1,consume1;
/*Now global variable n will be on Heap so it is accessible all the processes i.e. consume and produce*/

shellcmd xsh_prodcons(int nargs, char *args[])
{
  //Argument verifications and validations
  int count = 2000; //local varible to hold count
  if(nargs == 2){
     int par = atoi(args[1]);
     if(par){
  	  count = par;
     }
  }
 	/* Output info for '--help' argument */

  if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
	printf("Usage: %s\n\n", args[0]);
	printf("Description:\n");
	printf("\tDisplays the values consumed and produced in 'n' iterations from global variable.\n");
	printf("\t--help\tdisplay this help and exit\n");
	return 0;
	}
  if (nargs > 2) {
	fprintf(stderr, "%s: too many arguments\n", args[0]);
	fprintf(stderr, "Try '%s --help' for more information\n",
		args[0]);
	return 1;
	}
  //check args[1] if present assign value to count
  


  //create the process producer and consumer and put them in ready queue.
  //Look at the definations of function create and resume in the system folder for reference.      
 // produce1= semcreate(0);
 // consume1= semcreate(1);
  resume( create(producer, 1024, 20, "producer", 1, count));
  resume( create(consumer, 1024, 20, "consumer", 1, count));
  return (0);
}

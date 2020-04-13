#include <stream.h>
#include "tscdf.h"


uint pcport;
int num_streams, work_queue_depth, time_window,output_time;

void stream_consumer(int32 id, struct stream *str);

int32 stream_proc(int nargs, char *args[]) {
	ulong secs, msecs, time;
	secs = clktime;
	msecs = clkticks;
	
	char usage[] = "Usage: -s num_streams -w work_queue_depth -t time_window -o output_time\n";
	char *ch,c;
	int i;
	printf("%d",nargs); 

	if ((nargs % 2)==0) 
    {
        kprintf("%s", usage);
        return(-1);
	}else 
    {
        i = nargs - 1;
        while (i > 1) {
        ch = args[i-1];
        c = *(++ch);
        
        switch(c) {
            case 's':
            num_streams = atoi(args[i]);
            printf("%d", args[i]);
            break;

            case 'w':
            work_queue_depth = atoi(args[i]);
            break;

            case 't':
            time_window = atoi(args[i]);
            break;

            case 'o':
            output_time = atoi(args[i]);
            break;

            default:
            kprintf("%s", usage);
            return(-1);
        }

        i -= 2;
        }
	}	
	//initialise
    	ptinit(15);
	//get the pcport number
	if((pcport = ptcreate(num_streams)) == SYSERR) {
	    printf("ptcreate failed\n");
	    return(-1);
	}
	
	
	//Provinding memory for array of stream of size num_streams
	struct stream **streamArray = (struct stream **)getmem(sizeof(struct stream*) * num_streams);
	
	for (i = 0; i < num_streams; i++) {
	    	
		//providing memory to each stream
        	struct stream *stream = (struct stream *)getmem(sizeof(struct stream));
	    	
		//providing memory to work queue with deapth work_queue_depth
		stream->queue = (struct data_element **) getmem(sizeof(struct data_element*) * work_queue_depth);
	
		//providing memory to each element in work queue 
		for(int j = 0; j < work_queue_depth; j++) {
		    stream->queue[j] = (struct data_element *) getmem(sizeof(struct data_element));
		}
	
		//initialising the pointers to stream
		stream->head = 0;
		stream->tail = 0;
		
		//initialising the semaphores
		stream->items = semcreate(0);
		stream->mutex = semcreate(1);

		stream->spaces = semcreate(work_queue_depth);

		streamArray[i] = stream;
		
		//Calling consumer function and provinding initialized stream as a input.
		resume(create(stream_consumer, 1024, 20, 'consumer', 2, i, streamArray[i]));

	}

	//Parsing the input values
	int st, ts, v;
	char *a;
	for(int i=0;i<n_input;i++){
        a = (char *)stream_input[i];
        st = atoi(a);
        while (*a++ != '\t');
        ts = atoi(a);
        while (*a++ != '\t');
        v = atoi(a);
	
	//making Stream to wait to achieve synchronisation
        wait(streamArray[st]->spaces);
        wait(streamArray[st]->mutex);
	
	//Adding the parsed input values to the data element struct. 
        struct data_element *que = streamArray[st]->queue[streamArray[st]->tail % work_queue_depth];
        que->value = v;
        que->time = ts;
	
	//increasing the pointer for workqueue
        streamArray[st]->tail += 1;
	
	//providing signal to stream to allow access once work finished	
        signal(streamArray[st]->mutex);
        signal(streamArray[st]->items);

	}
	for(i=0; i < num_streams; i++) {

		uint32 pm;
		pm = ptrecv(pcport);
		kprintf("process %d exited\n", pm);

	}

	ptdelete(pcport, 0);
	time = (((clktime * 1000) + clkticks) - ((secs * 1000) + msecs));
	kprintf("time in ms: %u\n", time);
	return(0);
}



void stream_consumer(int32 id, struct stream *str) {

	int count =0;
	char *output = ""; 
	int32 *qarray;
	

	kprintf("stream_consumer id:%d (pid:%d)\n",id,getpid());
	
	struct tscdf *tc = tscdf_init(time_window);

	while(1) {
	
        wait(str->items);
        wait(str->mutex);
        
	// fetching the value of element from the work queue
        struct data_element *queueElem = str->queue[str->head % work_queue_depth];
		
 	// Appending the work queue pointer
        str->head +=1;
 
        if (queueElem->time == 0 && queueElem->value==0) {
 
		kprintf("stream_consumer exiting\n");
		ptsend(pcport, getpid());

		return;
    	}
	
	tscdf_update(tc,queueElem->time, queueElem->value);
	count+=1;
	
	if(count==output_time){
	
        qarray = tscdf_quartiles(tc);
        
        if(qarray == NULL) {
            kprintf("tscdf_quartiles returned NULL\n");
            continue;
        }
        sprintf(output, "s%d: %d %d %d %d %d", id, qarray[0], qarray[1], qarray[2], qarray[3], qarray[4]);
        kprintf("%s\n", output);
        freemem((char *)qarray, (6*sizeof(int32)));
        count=0;
	}
	
	
	signal(str->mutex);
	signal(str->spaces);

	}
}

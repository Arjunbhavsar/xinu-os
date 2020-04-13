#include<xinu.h>
#include<future.h>
#include<stream.h>
#include "tscdf.h"

future_t **farray;
int g_out_time,g_queue_depth, g_tWindow;
uint pcport;


int stream_proc_futures(int nargs, char* args[]) {
  // Parse arguments
  ulong secs, msecs, time;
  secs = clktime;
  msecs = clkticks;
  char usage[] = "Usage: -s num_streams -w work_queue_depth -t time_window -o output_time\n";
  int i, num_streams, work_queue_depth, time_window, output_time; 
  int st, ts ,v, head;
  char *a, *ch, c;
  if (!(nargs % 2)) {
    printf("%s", usage);
    return(-1);
  }
  else {
    int i = nargs - 1;
    while (i > 0) {
      ch = args[i-1];
      c = *(++ch);
      
      switch(c) {
      case 's':
        num_streams = atoi(args[i]);
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
        printf("%s", usage);
        return(-1);
      }

      i -= 2;
    }
  }
    // initialize all ports 
    ptinit(10);

    //create pc ports 
    if((pcport = ptcreate(num_streams)) == SYSERR) {
        printf("ptcreate failed\n");
        return(-1);
    }

    g_queue_depth = work_queue_depth;
    g_tWindow = time_window;
    g_out_time = output_time;

    // Create array to hold `num_streams` number of futures
    if ((farray = (future_t **)getmem(sizeof(future_t *) * (num_streams))) == (future_t **)SYSERR)
    {
        printf("getmem failed\n");
        return (SYSERR);
    }

    //Allocated memory to farray and spawned to methond stream_consumer_future with new memory
    for (i = 0; i < num_streams; i++)
    {
        if ((farray[i] = future_alloc(FUTURE_QUEUE, sizeof(de), work_queue_depth)) == (future_t *)SYSERR)
        {
            return (SYSERR);
        }
        resume(create((void *)stream_consumer_future, 4096, 20, "stream_consumer_future", 2, i, farray[i]));
    }

    // Create consumer processes and allocate futures
    // Use `i` as the stream id.
    // Future mode = FUTURE_QUEUE
    // Size of element = sizeof(struct data_element)
    // Number of elements = work_queue_depth
    
    // Parse input header file data and set future values
    for (i = 0; i < n_input; i++)
    {
        a = (char *)stream_input[i];
        st = atoi(a);
        while (*a++ != '\t')
            ;
        ts = atoi(a);
        while (*a++ != '\t')
            ;
        v = atoi(a);
        de *tempdata = (de *)getmem(sizeof(de));
        tempdata->time = ts;
        tempdata->value = v;
        future_set(farray[st], tempdata);
    }

    for (i = 0; i < num_streams; i++)
    {
        uint32 pm;
        pm = ptrecv(pcport);
        printf("process %d exited\n", pm);
    }

    //delete the used pc port 
    ptdelete(pcport, 0);

    //time calculated for completion of all processes 
    time = (((clktime * 1000) + clkticks) - ((secs * 1000) + msecs));
    printf("time in ms: %u\n", time);
    
    // free futures array
    for (i = 0; i < num_streams; i++)
    {
        future_free(farray[i]);
    }
  return 0;
}

void stream_consumer_future(int32 id, future_t *f)
{
    kprintf("stream_consumer_future id:%d ", id);
    kprintf("(pid:%d)\n", getpid());
    struct tscdf *tc;
    int count = 0;
    tc = tscdf_init(g_tWindow);
    while (TRUE)
    {
        de *temp;

        //Consume the value from future
        future_get(f, temp);

        if (temp->time == 0 && temp->value == 0)
        {
            break;
        }

        tscdf_update(tc, temp->time, temp->value);

        if (count++ == (g_out_time - 1))
        {
            char output[10];
            int *qarray;
            qarray = tscdf_quartiles(tc);
            if (qarray == NULL)
            {
                kprintf("tscdf_quartiles returned NULL\n");
                continue;
            }
            sprintf(output, "s%d: %d %d %d %d %d \n", id, qarray[0], qarray[1], qarray[2], qarray[3], qarray[4]);
            kprintf("%s", output);
            freemem((char *)qarray, (6 * sizeof(int32)));
            count = 0;
        }
    }
    tscdf_free(tc);
    ptsend(pcport, getpid());
}
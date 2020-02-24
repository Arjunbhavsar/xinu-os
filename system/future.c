#include <xinu.h>
#include <stdio.h>
#include <future.h>

// Allocating required memory to the future
// Also initializing the values 
future_t *future_alloc(future_mode_t mode, uint size, uint nelems)
{
    future_t *f;
    if ((f = (future_t *)getmem(sizeof(future_t) + (size))) == (future_t *)SYSERR)
    {
        return (future_t *)(SYSERR);
    }

    f->data = sizeof(future_t) + (char *)f;
    f->state = FUTURE_EMPTY;

    f->size = size;
    f->mode = mode;

    f->set_queue = newqueue();
    f->get_queue = newqueue();
    return f;
}

//freeing the memory of the future
syscall future_free(future_t* f )
{
        if( freemem( f, sizeof(f) + f -> size) == (future_t*) SYSERR)
        {
                  return (char *)SYSERR;
        }
        return (OK);
}

syscall future_set(
                     future_t* f,
                     char* in
                     )
{
    intmask mask;
    mask= disable();
    
    // Required Operations for mode FUTURE_EXCLUSIVE
    if(f->mode == FUTURE_EXCLUSIVE){
            
            // Here input value is assigned to future->data
            if (f->state ==FUTURE_EMPTY){
                    //f->data = *in;
                    memcpy(f->data,in ,sizeof(in));
                    f->state = FUTURE_READY;

            //Future set is allowed only one time hence will return SYSERR if called again
            }else if (f->state == FUTURE_READY){

                    restore(mask);
                    return (char*)SYSERR;

            // Here once the value is produced 
            //the only process which is in suspended state will be resumed 
            }else if(f->state == FUTURE_WAITING){
                    memcpy(f->data,in, sizeof(in)+1);
                    //f->data = in;
                    f->state = FUTURE_READY;
                    resume( f -> pid );

            }
    }else if( f -> mode == FUTURE_SHARED ){
            if( f -> state == FUTURE_EMPTY ){
                    //f = future_alloc(FUTURE_SHARED, sizeof(int), 1);
                    //f->data = *in;
                    memcpy(f->data,in,sizeof(in));
                    f->state = FUTURE_READY;
            
            //Future set is allowed only one time hence will return SYSERR if called again
            }else if( f -> mode == FUTURE_READY ){

                    restore(mask);
                    return (char*)SYSERR;

            }
            // Here once the value is produced 
            // All processes which are in get_queue in suspended state will be resumed 
            else if( f -> mode == FUTURE_WAITING ){

                    //f->data = in;
                    memcpy(f->data,in , sizeof(in));
                    f->state = FUTURE_READY;
                    pid32 proc ;
                    //while(!isempty(f->get_queue)){
                    while( ( proc = dequeue(f->get_queue)) != EMPTY){
                            //p = dequeue(f->get_queue);
                            resume(proc);
                    }
            }
    }
    restore(mask);
    return (OK);
}


syscall future_get(future_t *f, char *out)
{
    intmask mask;
    mask = disable();
    
    //future operations for FUTURE_WAITING state.
    if (f->state == FUTURE_WAITING )
    {   
        // Need to maintain the process id with futures running in waiting state
        // ans these procss are suspended till the value is getting generated 
        if(f->mode == FUTURE_SHARED){
            enqueue(getpid(), f->get_queue);
            suspend(getpid());
        }     
    }

    // future operations for empty state
    else if (f->state == FUTURE_EMPTY)
    {
        // There is no need add pid of the process in the queue and
        // the process is suspended till the time value is produced.
        // Once the value is produced the process is resumed.
        if (f->mode == FUTURE_EXCLUSIVE)
        {
            f->state = FUTURE_WAITING;
            f->pid = getpid();
            suspend(getpid());
            memcpy(out, f->data, sizeof(f->data));
            restore(mask);
            return (OK);
        }

        // Here get calls are added in the queue and the process is suspended till the time 
        // value is produced.
        // Once the value is produced the process is resumed.
        else if (f->mode == FUTURE_SHARED)
        {
            f->state = FUTURE_WAITING;
            enqueue(getpid(), f->get_queue);
            suspend(getpid());
            memcpy(out, f->data, sizeof(f->data));
            restore(mask);
            return (OK);
        }
    }
    
    //future Actions for ready state 
    //As mode is ready which means the data is produced and so get call will return some value 
    if (f->state == FUTURE_READY)
    {
        // Only one get call possible hence -> state is changed to Empty after consuming the value
        if (f->mode == FUTURE_EXCLUSIVE)
        {
            f->state = FUTURE_EMPTY;
            memcpy(out, f->data, sizeof(f->data));
            restore(mask);
            return (OK);
        }

        // Multiple get calls possible hence -> state is not changed to Empty after consuming the value
        if(f->mode == FUTURE_SHARED){ 
            memcpy(out, f->data, sizeof(f->data));
            restore(mask);
            return (OK);
        }
    }
}


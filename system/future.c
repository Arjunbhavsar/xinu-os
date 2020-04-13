#include<xinu.h>
#include<future.h>

future_t* future_alloc(future_mode_t mode, uint size, uint nelems){
	future_t* f ;
    
    //changed based on value of nelems 
    f = (future_t *)getmem((size*nelems) + sizeof(future_t));
    
    f->state = FUTURE_EMPTY;
    f->size = size;
    f->data = sizeof(future_t) + (char *)f;
    f->mode = mode;
    
    f->get_queue = newqueue();
    f->set_queue = newqueue();

    // new Fields 
    f->max_elems = nelems;
    f->count = 0;
    f->head = 0;
    f->tail = 0; 
    
    return f;
}

syscall future_free(future_t* f){
	return freemem(f, sizeof(f)+f->size);
}


syscall future_set(future_t* f, char* in){
	intmask mask;
    mask = disable();
    if (f->mode == FUTURE_QUEUE){

        // Check to ensure queue is full if yes we add the set call in the set_queue 
        //and once the values from the queue are consumed and the set call from the set queue 
        //will be resumed        
        if(f->count == f->max_elems - 1){
            enqueue(getpid(), f->set_queue);
            suspend(getpid());
          }
        
        //tail pointer for setting the value in the queue 
        char* tailelemptr = f->data + (f->tail * f->size);
        
        //value to be set is copied into location of the tailpointer         
        memcpy(tailelemptr, in, f->size);

        // head value is incremented yet ensured that it doen't cros max_elems        
        f->tail = (f->tail + 1)%f->max_elems;

        //once the value is produced by future_set, count is incremented by one 
        f->count = f->count + 1;
        
        //As the value is produced the state of the future is changed to FUTURE_READY
        //So that the produced value can be consumed 
        f->state = FUTURE_READY;
        //resume one of the request from the setqueue once get is complete
        pid32 proc ;
        if( (proc = dequeue(f->get_queue)) != EMPTY){
            resume(proc);
        }
        restore(mask);
        return OK;
    }
    //future operations for FUTURE_SHARED mode.
    if (f->mode == FUTURE_SHARED){
        memcpy(f->data, in, sizeof(in));
        f->state = FUTURE_READY;
        pid32 p;
        while((p = dequeue(f->get_queue)) != EMPTY){
            resume(p);
        }
    }

    //future operations for FUTURE_EXCLUSIVE mode.
    if(f->mode == FUTURE_EXCLUSIVE){
        if (f->state == FUTURE_EMPTY){
            
            memcpy(f->data, in, sizeof(in));
            f->state = FUTURE_READY;
        }
        // Here once the value is produced 
        //the only process which is in suspended state will be resumed 
        if (f->state == FUTURE_WAITING){
            
            memcpy(f->data, in, sizeof(in));
            f->state = FUTURE_READY;
            resume(f->pid);
        }
    }
    
    restore(mask);
    return OK;
}


syscall future_get(future_t* f, char* out){
	intmask mask;
    mask = disable();

    if (f->mode == FUTURE_SHARED){

        // Need to maintain the process id with futures running in waiting state
        // ans these procss are suspended till the value is getting generated 
        if(f->state == FUTURE_WAITING){
            enqueue(getpid(), f->get_queue);
            suspend(getpid());
        }

        if (f->state == FUTURE_EMPTY){
            f->state = FUTURE_WAITING;
            enqueue(getpid(), f->get_queue);
            suspend(getpid());
            memcpy(out, f->data, sizeof(f->data));
        }
        if (f->state == FUTURE_READY){
            memcpy(out, f->data, sizeof(f->data));
        }
    }

    else if (f->mode == FUTURE_EXCLUSIVE){

        // There is no need add pid of the process in the queue and
        // the process is suspended till the time value is produced.
        // Once the value is produced the process is resumed.
        if (f->state == FUTURE_EMPTY){
            f->state = FUTURE_WAITING;
            f->pid = getpid();
            suspend(f->pid);
        }

        if (f->state == FUTURE_READY){
            memcpy(out, f->data, sizeof(f->data));
            f->state = FUTURE_EMPTY;
        }
    }
    else if(f->mode == FUTURE_QUEUE){
        
        if(f->state == FUTURE_WAITING){
            //for waiting fot the value to be produced first 
            //the get process is suspended and added in a queue
            enqueue(getpid(), f->get_queue);
            suspend(getpid());
        }


        if (f->state == FUTURE_EMPTY){
            
            f->state = FUTURE_WAITING;
        
            // as future state is empty there no value to consume Hence we suspend the 
            // process till value is produced.
            enqueue(getpid(), f->get_queue);
            suspend(getpid());
            
            //head pointer of the queue  
            char* headelemptr = f->data + (f->head * f->size);
            //value is consumed from head pointer 
            memcpy(out, headelemptr, f->size);
            
            // head value is incremented yet ensured that it doen't cros max_elems
            f->head = (f->head + 1)%f->max_elems;
            
            //once the value is used by future_get count is minimized by one 
            f->count = f->count - 1;
            
            //resume one of the request from the setqueue once get is complete
            pid32 proc ;
            if( (proc = dequeue(f->set_queue)) != EMPTY){
                resume(proc);
            }
            
            restore(mask);
            return OK;
        }
        
        if(f->state == FUTURE_READY){
            
            //Here check is added as f->count can have value 0 even if being in FUTURE_READY state 
            if(f->count == 0){
                enqueue(getpid(), f->get_queue);
                suspend(getpid());
            }
            
            //head pointer of the queue  
            char* headelemptr = f->data + (f->head * f->size);
            //value is consumed from head pointer 
            memcpy(out, headelemptr, f->size);
            
            // head value is incremented yet ensured that it doen't cros max_elems
            f->head = (f->head + 1)%f->max_elems;
            
            //once the value is used by future_get count is minimized by one 
            f->count = f->count - 1;
            
            //resume one of the request from the setqueue once get is complete
            pid32 proc ;
            if( (proc = dequeue(f->set_queue)) != EMPTY){
                resume(proc);
            }

            restore(mask);
            return OK;   
        }
    }
    restore(mask);
    return OK;
}

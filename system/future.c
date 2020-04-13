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
        
        if(f->count == f->max_elems - 1){
            enqueue(getpid(), f->set_queue);
            suspend(getpid());
          }
        char* tailelemptr = f->data + (f->tail * f->size);
        memcpy(tailelemptr, in, f->size);
        f->state = FUTURE_READY;
        f->tail = (f->tail + 1)%f->max_elems;
        f->count = f->count + 1;
        //resume(dequeue(f->get_queue));
        pid32 proc ;
        if( (proc = dequeue(f->get_queue)) != EMPTY){
            resume(proc);
        }
    }
    
    if (f->mode == FUTURE_SHARED){
        memcpy(f->data, in, sizeof(in));
        f->state = FUTURE_READY;
        pid32 p;
        while((p = dequeue(f->get_queue)) != EMPTY){
            resume(p);
        }
    }
    if(f->mode == FUTURE_EXCLUSIVE){
        if (f->state == FUTURE_EMPTY){
            
            memcpy(f->data, in, sizeof(in));
            f->state = FUTURE_READY;
        }
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
            
            //
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
            
            char* headelemptr = f->data + (f->head * f->size);
            
            memcpy(out, headelemptr, f->size);
            
            //
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

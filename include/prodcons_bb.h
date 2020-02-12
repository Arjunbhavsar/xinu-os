// declare globally shared array
extern int arr_q[5];
// declare globally shared semaphores
extern sid32 consume,produce;
// declare globally shared read and write indices
extern int producerIndices;
extern int consumerIndices;

// function prototypes
void consumer_bb(int count);
void producer_bb(int count);

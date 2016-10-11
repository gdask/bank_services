#include "DB.h"
#include "Lists.h"
#include <pthread.h>
#include <iostream>

class System{
public:
    int thr; //# Of Working Threads
    HashTable *db;
    Fd_Queue *q; //FileDescriptor Queue
    pthread_mutex_t mtx;
    pthread_cond_t nonempty,nonfull;
    pthread_t *thread_IDs;
    System(int threads,int q_size,int buckets);
    ~System();
    void Feed(int fd);
};

void * Worker(void *ptr);

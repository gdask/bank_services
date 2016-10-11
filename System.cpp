#include "System.h"
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 512

using namespace std;

System::System(int threads,int q_size,int buckets){
    //Init shared stractures
    thr=threads;
    db = new HashTable(buckets);
    q = new Fd_Queue(q_size);

    //Init mutexes and cond variables
    pthread_mutex_init(&mtx,NULL);
    pthread_cond_init(&nonempty,NULL);
    pthread_cond_init(&nonfull,NULL);

    //Create Worker Threads
    thread_IDs = new pthread_t[threads];
    int i;
    for(i=0;i<threads;i++) pthread_create(&thread_IDs[i],0,Worker,this);
}

System::~System(){

    int i;
    for(i=0;i<thr;i++) this->Feed(-1); //Terminate Worker Threads
    for(i=0;i<thr;i++) pthread_join(thread_IDs[i],NULL);

    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&nonempty);
    pthread_cond_destroy(&nonfull);

    delete db;
    delete q;

    delete[] thread_IDs;
}


void System::Feed(int fd){
    //Synchronized add FileDescriptord to queue.
    pthread_mutex_lock(&mtx); //CS starts
    while(q->Full()){
        pthread_cond_wait(&nonfull,&mtx);
    }
    q->Add(fd);
    pthread_mutex_unlock(&mtx); //End of CS
    pthread_cond_broadcast(&nonempty); //Signal at workers

    return;
}


void * Worker(void * ptr){
    System *sys = (System*) ptr;
    while(1){
        //Synchronized pop fd from queue
        pthread_mutex_lock(&(sys->mtx)); //CS starts
        while(sys->q->Empty()){
            pthread_cond_wait(&(sys->nonempty),&(sys->mtx));
        }
        int fd=sys->q->Pop();
        pthread_mutex_unlock(&(sys->mtx)); //End of CS
        pthread_cond_broadcast(&(sys->nonfull)); //Signal at Feed
        if(fd==-1) pthread_exit(0);
        //Read from fd,call db->Service,write response at fd,until connection is closed.

        while(1){ //Provide service at client
            char msg_size[sizeof(uint32_t)];
            uint32_t *size = (uint32_t*) msg_size;

            int readed = read(fd,msg_size,sizeof(uint32_t));
            if(readed==-1) break; //Something Went wrong
            if(readed==0) break; //Client Disconected

            //Now read #size chars from fd
            int bytes_to_read = ntohl(*size);
            stringstream inp;
            char buf[BUF_SIZE];
            while(bytes_to_read > 0){
                if(bytes_to_read < BUF_SIZE-1){
                    readed = read(fd,buf,bytes_to_read);
                }
                else{
                    readed = read(fd,buf,BUF_SIZE-1);
                }
                if(readed==-1) break; //Something Went Wrong
                if(readed== 0) break; //Client Disconected
                bytes_to_read = bytes_to_read - readed;
                buf[readed] = 0; //Force Null Terminated string
                inp << buf ;
            }

            //Use Service from DB
            stringstream res;
            sys->db->Service(inp,res);
            string out(res.str());

            //Count characters to sent
            *size=htonl(out.length());
            //Sent that info at fd
            int writed = write(fd,msg_size,sizeof(uint32_t));
            if( writed==-1) break; //Client Disconected Unexpectedly
            //Now write #size chars at client

            while(out.length()!=0){
                if(out.length() < BUF_SIZE){
                    strcpy(buf,out.c_str());
                    writed=write(fd,buf,out.length());
                }
                else{
                    memcpy(buf,out.c_str(),BUF_SIZE);
                    writed=write(fd,buf,BUF_SIZE);
                }
                if(writed==-1) break; //Something Went wrong
                out.erase(out.begin(),out.begin()+writed);
            }

        }
        close(fd);
    }
}

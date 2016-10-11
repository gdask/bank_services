#ifndef DB_H
#define	DB_H

#include "Lists.h"
#include <pthread.h>
#include <time.h>
#include <cstring>
#include <sstream>


class HashTable{ //A thread safe DB. Provides at system a thread safe service for query anwsering.
    int buckets;
    Account_List **account_bucket;
    pthread_mutex_t **lock_arr; //Array of mutexes
    //Secondary functions
    void Lock(Int_List &arg);
    void Unlock(Int_List &arg);
    int StringToBucket(std::string name);
    Int_List* Convert(String_List& arg); //Converts a list of names to an distinct ordered list of buckets to get locked/unlocked.
    void Sleep(clock_t start,clock_t stop,int delay);
    //Primary functions
    voidaccount_bucketd_Account(std::string name,int start,int delay,std::stringstream &res);
    void Transfer(std::string source,int amount,int delay,String_List &targets,std::stringstream &res); //Multi & Atomic Transfer
    void Balance(String_List &targets,std::stringstream &res); //Multi & Atomic Balance check
public:
    HashTable(int n); //N=Number of buckets
    ~HashTable();
    void Service(std::stringstream &in,std::stringstream &out);
};



#endif

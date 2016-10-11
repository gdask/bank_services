#include <iostream>
#include <string>
#include <sstream>
#include <string>

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "System.h"

using namespace std;

int NetSet(short port,int queue){
    int sock_fd;
    struct sockaddr_in server;

    server.sin_family = AF_INET;       /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    //Create socket for tcp internet connection
    sock_fd=socket(AF_INET,SOCK_STREAM,0);
    if(sock_fd<0){
        perror("Socket Error");
        exit(1);
    }
    //Bind socket to port
    if(bind(sock_fd,(struct sockaddr*)&server,sizeof(server))<0){
        perror("Bind Error");
        exit(1);
    }
    //Listen
    if(listen(sock_fd,queue)<0){
        perror("Listen Failed");
        exit(1);
    }
    return sock_fd;
}

System *global;

void ShutDown(int arg){
    cout << "Server Shutting down" << endl;
    global->~System();
    exit(0);
}

int main(int argc,char **argv){
    //Arguments check
    int i,port,tpool_size,q_size;
    if(argc!=7){
        cout << "Wrong Arguments" << endl;
        cout << "-p <port> -s <thread pool size> -q <queue size>" << endl;
        exit(1);
    }
    for(i=1;i<7;i=i+2){
        if(strcmp(argv[i],"-p")==0){
            port=atoi(argv[i+1]);
        }
        else if(strcmp(argv[i],"-s")==0){
            tpool_size=atoi(argv[i+1]);
        }
        else if(strcmp(argv[i],"-q")==0){
            q_size=atoi(argv[i+1]);
        }
        else{
            cout << "Wrong Arguments" << endl;
            cout << "-p <port> -s <thread pool size> -q <queue size>" << endl;
            exit(1);
        }
    }
    if(port<=0 || tpool_size<=0 || q_size<=0){
        cout<<"Wrong Arguments" << endl;
        cout<<"Arguments should be positive integers" << endl;
    }

    //Init subsystems
    int socket=NetSet(port,10); //Last argument is socket queue
    System sys(tpool_size,q_size,10); //Last argument is the # buckets of HashTable
    global = &sys;

    signal(SIGPIPE,SIG_IGN);
    signal(SIGINT,ShutDown);

    cout << "Server Running on port: " << port << endl;

    while(1){
        int new_fd = accept(socket,NULL,NULL);
        if(new_fd<0){
            perror("Accept Failed");
        }
        else{
            sys.Feed(new_fd);
        }
    }

    return 0;
}

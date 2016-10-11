#include <netdb.h>	         /* gethostbyaddr */
#include <stdio.h>
#include <sys/types.h>	     /* sockets */
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <unistd.h>          /* read, write, close */
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#define BUF_SIZE 512

using namespace std;

int main(int argc,char **argv){
    //Arguments Check
    int i,port;
    char *name,*file;
    if(argc!=7){
        cout << "Wrong Arguments" << endl;
        cout << "-h <server host> -p <server port> -i <command file>" << endl;
        exit(1);
    }

    for(i=1;i<7;i=i+2){
        if(strcmp(argv[i],"-h")==0){
            name=argv[i+1];
        }
        else if(strcmp(argv[i],"-p")==0){
            port=atoi(argv[i+1]);
        }
        else if(strcmp(argv[i],"-i")==0){
            file=argv[i+1];
        }
        else{
            cout << "Wrong Arguments" << endl;
            cout << "-h <server host> -p <server port> -i <command file>" << endl;
            exit(1);
        }
    }
    ifstream instructions;
    instructions.open(file);


    int sock;
    struct sockaddr_in server;
    struct hostent *rem;

    //Resolve name at IP address
    rem = gethostbyname(name);
    if(rem==NULL){
        herror("GetHostIp Failed");
        exit(1);
    }
    //Create a socket
    sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock <0){
        perror("Socket Failed");
        exit(2);
    }
    //Connect to server
    server.sin_family = AF_INET;       /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port);         /* Server port */

    if(connect(sock,(struct sockaddr*)&server,sizeof(server)) <0){
        perror("Connect failed");
        exit(3);
    }
    while(1){//Server-Client Comunication through fd sock
        char msize[sizeof(uint32_t)];
        uint32_t *siz = (uint32_t*) msize;
        char buf[BUF_SIZE];

        //Get Input
        string inp;
        if(instructions.good()) getline(instructions,inp); //Input from instructions file
        else getline(cin,inp); //Input from standart Input
        //else break;

        //Parse Whatever
        if(inp.compare("exit")==0) break;
        if(inp.length()==0) continue;

        stringstream temp;
        temp << inp;
        string tempstr;
        temp >> tempstr;
        if(tempstr.compare("sleep")==0){
            int tempint;
            temp >> tempint;
            if(tempint!=0){
                usleep(tempint*1000);
                continue;
            }
        }
        //Write request lenght at server (network byte order)
        *siz=htonl(inp.length());
        int writed = write(sock,siz,sizeof(uint32_t));
        if( writed==-1){
            perror("Write Fail");
            break;
        }
        //Write actual Request
        while(inp.length()!=0){
          if(inp.length() < BUF_SIZE){
            strcpy(buf,inp.c_str());
            writed=write(sock,buf,inp.length());
          }
          else{
            memcpy(buf,inp.c_str(),BUF_SIZE-1);
            writed=write(sock,buf,BUF_SIZE-1);
          }
          if(writed==-1){
            perror("Write Failed");
            close(sock);
            exit(4);
          }
          inp.erase(inp.begin(),inp.begin()+writed);
        }


        //Read Response lenght from server (network byte order)
        int readed = read(sock,siz,sizeof(uint32_t));
        if(readed==-1){
            perror("Read Fail");
            break;
        }

        //Read Response
        int bytes_to_read = ntohl(*siz);

        while(bytes_to_read > 0){
            if(bytes_to_read < BUF_SIZE -1){
                readed = read(sock,buf,bytes_to_read);
            }
            else{
                readed = read(sock,buf,BUF_SIZE-1);
            }
            if(readed==0){
                cout << "Server Disconected" << endl;
                break;
            }
            if(readed==-1){
                perror("Read Failed");
                close(sock);
                exit(5);
            }
            buf[readed]=0;
            bytes_to_read = bytes_to_read - readed;
            cout << buf ;
        }
    }
    close(sock);
    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define Buffersize 1024

int main(int argc,char*argv[]){

    int sockfd,n;
    struct sockaddr_in remote_addr,local_addr;
    socklen_t len=sizeof(struct sockaddr_in);
    char buffer[Buffersize];

    if(argc<2){
        perror("inserire porta\n");
        return 0;
    }

    memset(&local_addr,0,len);
    memset(&remote_addr,0,len);

    local_addr.sin_family=AF_INET;
    local_addr.sin_port=htons(atoi(argv[1]));
    local_addr.sin_addr.s_addr=INADDR_ANY;

    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    if(sockfd<0){
        perror("errore apertura socket\n");
        return -1;
    }

    bind(sockfd,(struct sockaddr*)&local_addr,len);

    printf("server is ON....\n");

    while(1){

        n=recvfrom(sockfd,buffer,sizeof(buffer),0,(struct sockaddr*)&remote_addr,&len);

        if(strcmp(buffer,"exit")==0){
            break;
        }

        printf("message:%s, from: %s\n",buffer,inet_ntoa(remote_addr.sin_addr));
        
    }

    close(sockfd);
    return 0;

}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define Buffersize 1024

int main(int argc,char*argv[]){

    int sockfd,newsockfd,n;
    struct sockaddr_in remote_addr,local_addr;
    socklen_t len=sizeof(struct sockaddr_in);
    char buffer[Buffersize];

    if(argc<2){
        perror("inserire porta\n");
        return 0;
    }

    sockfd=socket(AF_INET,SOCK_STREAM,0);

    if(sockfd<0){
        perror("errore apertura socket\n");
        return -1;
    }

    memset(&local_addr,0,len);
    memset(&remote_addr,0,len);

    local_addr.sin_family=AF_INET;
    local_addr.sin_port=htons(atoi(argv[1]));
    local_addr.sin_addr.s_addr=INADDR_ANY;

    bind(sockfd,(struct sockaddr*)&local_addr,len);

    printf("server is ON....\n");

    listen(sockfd,5);


    newsockfd=accept(sockfd,(struct sockaddr*)&remote_addr,&len);

    while(1){

        n=recv(newsockfd,buffer,sizeof(buffer),0);

        if(strcmp(buffer,"exit")==0){
            break;
        }

        //printf("message:%s, from: %s\n",buffer,inet_ntoa(remote_addr.sin_addr));
        int lunghezza=strlen(buffer);
        char lunghezzaSTR[1024];
        sprintf(lunghezzaSTR,"%d",lunghezza);
        send(newsockfd,lunghezzaSTR,sizeof(lunghezzaSTR),0);
    }

    close(newsockfd);
    close(sockfd);

    return 0;

}
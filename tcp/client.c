#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define Buffersize 1024

int main(int argc,char*argv[]){

    int sockfd,n;
    struct sockaddr_in remote_addr;
    socklen_t len=sizeof(struct sockaddr_in);
    char buffer[Buffersize];

    if(argc<3){
        perror("inserire ip e porta\n");
        return 0;
    }

    sockfd=socket(AF_INET,SOCK_STREAM,0);

    if(sockfd<0){
        perror("errore nell'apertura socket\n");
        exit(EXIT_FAILURE);
    }

    memset(&remote_addr,0,len);
    remote_addr.sin_family=AF_INET;
    remote_addr.sin_port=htons(atoi(argv[2]));
    inet_pton(AF_INET,argv[1],&remote_addr.sin_addr);

    connect(sockfd,(struct sockaddr*)&remote_addr,len);

    while(1){
        printf("inserire messaggio: ");
        fgets(buffer,Buffersize,stdin);
        buffer[strcspn(buffer,"\n")]=0; // ellimina lo /n che mette a capo la stringa

        n=send(sockfd,buffer,sizeof(buffer),0);

        if(strcmp(buffer,"exit")==0){
            break;
        }

        if(n<0){
            perror("error in send\n");
            return 0;
        }
        
        recv(sockfd,buffer,sizeof(buffer),0);

        printf("server msg: %s\n",buffer);

    }

    close(sockfd);
    return 0;
}
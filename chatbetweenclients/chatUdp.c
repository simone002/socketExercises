#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#define buffersize 1024

int main(int argc,char*argv[]){

    int sockfd,n;
    char buffer[buffersize];
    struct sockaddr_in remote_addr,local_addr;

    socklen_t len=sizeof(struct sockaddr_in);

    if(argc!=4){
        printf("ERRORE! inserisci IP, P1, P2\n");
        return 0;
    }

    if (!fork()) // figlio
    {
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            printf("ERRORE! apertura socket\n");
            return -1;
        }

        memset(&local_addr, 0, len);
        local_addr.sin_family = AF_INET;
        local_addr.sin_port = htons(atoi(argv[3]));

        if (bind(sockfd, (struct sockaddr *)&local_addr, len) < 0)
        {
            printf("errore nella bind\n");
        }

        for (;;)
        {
            n = recvfrom(sockfd, buffer, buffersize, 0, (struct sockaddr *)&remote_addr, &len);

            buffer[n] = 0; // ultimo elemento a 0 per evitare di leggere altro

            printf("Messaggio %s,IP %s, Port %d\n", buffer, inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
        }
    }
    else{

        if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0){
            printf("ERRORE nellapertura socket");
            return 0;
        }

    //bisogna mettere cpme rete 
    /*enable  broadcasting*/
        //int broadcastEnable=1;
        //int ret=setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&broadcastEnable,sizeof(broadcastEnable));
    /*                      */

        memset(&remote_addr,0,len);
        remote_addr.sin_family=AF_INET;
        inet_pton(AF_INET,argv[1],(&remote_addr.sin_addr));
        remote_addr.sin_port=htons(atoi(argv[2]));

        while(fgets(buffer,buffersize,stdin)!=NULL){

            sendto(sockfd,buffer,buffersize,0,(struct sockaddr*)&remote_addr,len);
        }
    }
    close(sockfd);
    return 0;
}
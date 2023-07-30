#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>


#define buffersize 1024
#define filenamelenght 100

FILE *file;
char buffer[buffersize],filname[filenamelenght];
int n;

void send_file(int newsockfd,char*nome)
{

    if ((file = fopen(nome, "r")) == NULL) {
        perror("file\n");
        exit(EXIT_FAILURE);
    }

    // Send the file to the server
    while ((n = fread(buffer, 1, buffersize, file)) > 0) {
        if (send(newsockfd, buffer, n, 0) < 0) {
            perror("send\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("File sent successfully\n");

    // Close the file and the connection
    fclose(file);
    close(newsockfd);
}

int main(int argc, char**argv){

    int sockfd,newsocket;
    struct sockaddr_in server_addr,client_addr;
    socklen_t len=sizeof(server_addr);

    if(argc!=2){
        perror("inserire ip e porta\n");
        exit(EXIT_FAILURE);
    }

    sockfd=socket(AF_INET,SOCK_STREAM,0);

    if(sockfd<0){
        perror("socket\n");
        exit(EXIT_FAILURE);
    }


    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr=INADDR_ANY;

    bind(sockfd,(struct sockaddr*)&server_addr,len);

    listen(sockfd,5);

    newsocket=accept(sockfd,(struct sockaddr*)&client_addr,&len);

    recv(newsocket,filname,sizeof(filname),0);

    send_file(newsocket,filname);

    close(sockfd);
    return 0;
        

    return 0;
}
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define namefilelength 100

int main(int argc, char**argv){

    int sockfd,n;
    struct sockaddr_in server_addr;
    socklen_t len=sizeof(server_addr);

    FILE*f;

    if(argc!=3){
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
    server_addr.sin_port=htons(atoi(argv[2]));
    inet_pton(AF_INET,argv[1],&server_addr.sin_addr);

    connect(sockfd,(struct sockaddr*)&server_addr,len);

    char buffer[1024];
    char namefile[namefilelength];

    printf("quale file vuoi ricevere?\n");

    fgets(namefile,sizeof(namefile),stdin);
    namefile[strcspn(namefile, "\n")]=0;

    send(sockfd,namefile,sizeof(namefile),0);

    // Open the file to save the received data
    if ((f = fopen("received_file.txt", "w")) == NULL) {
        perror("file\n");
        exit(EXIT_FAILURE);
    }

    // Receive data from the server and save it to the file
    while ((n = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
        if (fwrite(buffer, 1, n, f) != n) {
            perror("file\n");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

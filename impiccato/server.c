#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#define numTotRegistrazioni 2

struct giocatore{

    char nome[30];
    int port;
};

struct players{
    char nome[30];
    int port;
    int vite;
    char ip[INET_ADDRSTRLEN];
};

struct players p[2];

int NumRegistrazioni=0;
int running=1;


struct giocatore g[2];
int sockfd,n;
struct sockaddr_in server_addr,client_addr;
socklen_t len=sizeof(server_addr);
char ans[1024];


void Registrazione(){

    while(NumRegistrazioni<numTotRegistrazioni){
        recvfrom(sockfd,&g[NumRegistrazioni],sizeof(g[NumRegistrazioni]),0,(struct sockaddr*)&client_addr,&len);
        strcpy(p[NumRegistrazioni].nome,g[NumRegistrazioni].nome);
        p[NumRegistrazioni].port=g[NumRegistrazioni].port;
        strcpy(p[NumRegistrazioni].ip,inet_ntoa(client_addr.sin_addr));
        p[NumRegistrazioni].vite=3;
        NumRegistrazioni++;
    }
   

}

void Game(){



}

int main(int argc, char**argv){

    if(argc<2){
        perror("inserire porta\n");
        exit(EXIT_FAILURE);
    }


    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    if(sockfd<0){
        perror("errore apertura socket\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr,0,len);
    memset(&client_addr,0,len);

    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr=INADDR_ANY;
    

    bind(sockfd,(struct sockaddr*)&server_addr,len);

    printf("server listening\n");

    Registrazione();

    memset(ans,0,sizeof(ans));
    strcpy(ans,"benvenuti player");

    

    //sendto(sockfd,ans,sizeof(ans),0,(struct sockaddr*)&client_addr,len);


    //while(running){
  //      Game();
    //}


    close(sockfd);
}
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

struct giocatore{
    char nome[30];
    int port;

};

int sockfd,n;
struct sockaddr_in server_addr;
socklen_t len=sizeof(server_addr);
char ans[1024];

struct giocatore g;

void Registrazione(){


    printf("inserire nome: ");
    fgets(g.nome,sizeof(g.nome),stdin);
    g.nome[strcspn(g.nome,"\n")]=0;

    printf("inserire port: ");
    scanf("%d",&g.port);
    getchar();


    sendto(sockfd,&g,sizeof(g),0,(struct sockaddr*)&server_addr,len);


}

int main(int argc,char**argv){

    if(argc<3){
        perror("error insert ip and port\n");
        exit(EXIT_FAILURE);
    }

    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    memset(&server_addr,0,len);

    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(atoi(argv[2]));
    inet_pton(AF_INET,argv[1],&server_addr.sin_addr);

    Registrazione();
    memset(ans,0,sizeof(ans));

  //  recvfrom(sockfd,ans,sizeof(ans),0,(struct sockaddr*)&server_addr,&len);
  //  printf("ans: %s\n",ans);

    close(sockfd);

}
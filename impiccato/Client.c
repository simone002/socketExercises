#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

struct giocatore {
    char nome[30];
    int port;
};

int running=1;

int sockfd, n;
struct sockaddr_in server_addr;
socklen_t len = sizeof(server_addr);
char ans[1024];

struct giocatore g;

void Registrazione() {
    printf("inserire nome: ");
    fgets(g.nome, sizeof(g.nome), stdin);
    g.nome[strcspn(g.nome, "\n")] = 0;

    printf("inserire port: ");
    scanf("%d", &g.port);
    getchar();

    sendto(sockfd, &g.nome, sizeof(g.nome), 0, (struct sockaddr*)&server_addr, len);
    sendto(sockfd, &g.port, sizeof(g.port), 0, (struct sockaddr*)&server_addr, len);
}

void game(){

    printf("indovina la parola\n");

    printf("vuoi dire una lettera o una parola? 1. lettera 2. parola\n");

    scanf("%s",ans);
    getchar();

    char lettera;
    char parola[10];

    sendto(sockfd,ans,sizeof(ans),0,(struct sockaddr*)&server_addr,len);
    if(strcmp(ans,"1")==0){
        printf("inserisci una lettera\n");
        scanf("%c",&lettera);
        sendto(sockfd,&lettera,sizeof(lettera),0,(struct sockaddr*)&server_addr,len);
    }
    if(strcmp(ans,"2")==0){
        printf("inserisci una parola\n");
        scanf("%s",parola);
        getchar();
        sendto(sockfd,parola,sizeof(parola),0,(struct sockaddr*)&server_addr,len);
    }
    memset(ans,0,sizeof(ans));
    recvfrom(sockfd,ans,sizeof(ans),0,(struct sockaddr*)&server_addr,&len);
    if(strcmp(ans,"hai vinto")==0){
        running=0;
    }
}


int main(int argc, char** argv) {
    if (argc < 3) {
        perror("error insert ip and port\n");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    Registrazione();
    memset(ans, 0, sizeof(ans));

    // Receive message from server
    recvfrom(sockfd, ans, sizeof(ans), 0, (struct sockaddr*)&server_addr, &len);
    printf("Received message from server: %s\n", ans);
    while(running){
        game();
    }

    close(sockfd);
}

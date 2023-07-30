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

void game() {
    char client[10];

    printf("Inserire la tua mossa (rock/paper/scissor): ");
    fgets(client, sizeof(client), stdin);
    client[strcspn(client, "\n")] = 0;

    sendto(sockfd, client, sizeof(client), 0, (struct sockaddr*)&server_addr, len);
    recvfrom(sockfd, ans, sizeof(ans), 0, (struct sockaddr*)&server_addr, &len);
    recvfrom(sockfd, ans, sizeof(ans), 0, (struct sockaddr*)&server_addr, &len);
    
    printf("%s\n",ans);

    if (strcmp(ans, "hai perso") == 0) {
        running = 0;
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
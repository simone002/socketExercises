#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define numTotRegistrazioni 2


struct players {
    char nome[30];
    int port;
    int vite;
    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in client_addr;
};

struct players p[numTotRegistrazioni];
int NumRegistrazioni = 0;
int running = 1;

int sockfd, n;
struct sockaddr_in server_addr, client_addr;
socklen_t len = sizeof(server_addr);
char ans[1024];

void Registrazione() {
    while (NumRegistrazioni < numTotRegistrazioni) {
        recvfrom(sockfd, &p[NumRegistrazioni].nome, sizeof(p[NumRegistrazioni].nome), 0, (struct sockaddr*)&client_addr, &len);

        recvfrom(sockfd, &p[NumRegistrazioni].port, sizeof(p[NumRegistrazioni].port), 0, (struct sockaddr*)&client_addr, &len);

        strcpy(p[NumRegistrazioni].ip, inet_ntoa(client_addr.sin_addr));
        p[NumRegistrazioni].vite = 3;
        p[NumRegistrazioni].client_addr = client_addr;
        NumRegistrazioni++;
    }
}

void Game()
{

    char client1[10];
    char client2[10];
    memset(ans, 0, sizeof(ans));

    strcpy(ans, "fai la tua mossa");
    sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&p[0].client_addr, len);
    recvfrom(sockfd, client1, sizeof(client1), 0, (struct sockaddr *)&p[0].client_addr, &len);
    sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&p[1].client_addr, len);
    recvfrom(sockfd, client2, sizeof(client2), 0, (struct sockaddr *)&p[1].client_addr, &len);
    if (p[0].vite == 0)
    {
        strcpy(ans, "hai perso");
        sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&p[0].client_addr, len);
        running = 0;
    }
    if (p[1].vite == 0)
    {
        strcpy(ans, "hai perso");
        sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&p[1].client_addr, len);
        running = 0;
    }

    if (strcmp(client1, "rock") == 0 && strcmp(client2, "scissor") == 0)
    {
        memset(ans, 0, sizeof(ans));
        strcpy(ans, "vince roccia");
        for (int i = 0; i < numTotRegistrazioni; i++)
            sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&p[i].client_addr, len);
        p[1].vite--;
    }
    if (strcmp(client1, "scissor") == 0 && strcmp(client2, "rock") == 0)
    {
        memset(ans, 0, sizeof(ans));
        strcpy(ans, "vince roccia");
        for (int i = 0; i < numTotRegistrazioni; i++)
            sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&p[i].client_addr, len);
        p[0].vite--;
    }
    if (strcmp(client1, "scissor") == 0 && strcmp(client2, "paper") == 0)
    {
        memset(ans, 0, sizeof(ans));
        strcpy(ans, "vince scissor");
        for (int i = 0; i < numTotRegistrazioni; i++)
            sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&p[i].client_addr, len);
        p[1].vite--;
    }
    if (strcmp(client1, "paper") == 0 && strcmp(client2, "scissor") == 0)
    {
        memset(ans, 0, sizeof(ans));
        strcpy(ans, "vince scissor");
        for (int i = 0; i < numTotRegistrazioni; i++)
            sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&p[i].client_addr, len);
        p[0].vite--;
    }
    if (strcmp(client1, "paper") == 0 && strcmp(client2, "rock") == 0)
    {
        memset(ans, 0, sizeof(ans));
        strcpy(ans, "vince paper");
        for (int i = 0; i < numTotRegistrazioni; i++)
            sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&p[i].client_addr, len);
        p[1].vite--;
    }
    if (strcmp(client1, "rock") == 0 && strcmp(client2, "paper") == 0)
    {
        memset(ans, 0, sizeof(ans));
        strcpy(ans, "vince paper");
        for (int i = 0; i < numTotRegistrazioni; i++)
            sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&p[i].client_addr, len);
        p[0].vite--;
    }
}

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        perror("inserire porta\n");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("errore apertura socket\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, len);
    memset(&client_addr, 0, len);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr *)&server_addr, len);

    printf("server listening\n");

    Registrazione();

    memset(ans, 0, sizeof(ans));
    strcpy(ans, "benvenuti player");
    for (int i = 0; i < numTotRegistrazioni; i++) {
        sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr*)&p[i].client_addr, len);
    }

    while (running)
    {
        Game();
    }

    close(sockfd);
}

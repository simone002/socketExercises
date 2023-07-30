#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define numTotRegistrazioni 2

struct giocatore {
    char nome[30];
    int port;
};

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

void Game() {
    for (int i = 0; i < numTotRegistrazioni; i++) {
        sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr*)&p[i].client_addr, len);
    }
    int parolecorrette=0;
    while (parolecorrette < 10)
    {
        char parola[10] = "imieicogli";
        memset(ans, 0, sizeof(ans));
        char lettera;
        char frase[10];
        int corretto = 0;
        recvfrom(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&client_addr, &len);
        if (strcmp(ans, "1") == 0)
        {
            recvfrom(sockfd, &lettera, sizeof(lettera), 0, (struct sockaddr *)&client_addr, &len);
            for (int i = 0; i < 10; i++)
            {
                if (parola[i] == lettera)
                {
                    corretto = 1;
                    parolecorrette++;
                }
            }
            memset(ans, 0, sizeof(ans));
            if (corretto)
                strcpy(ans, "corretto");
            else if (!corretto)
            {
                strcpy(ans, "non corretto");
                for (int i = 0; i < 2; i++)
                {
                    if (p[i].ip == inet_ntoa(client_addr.sin_addr))
                        p[i].vite--;
                }
            }
            sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&client_addr, len);
        }
        else if (strcmp(ans, "2") == 0)
        {
            recvfrom(sockfd, frase, sizeof(frase), 0, (struct sockaddr *)&client_addr, &len);
            memset(ans, 0, sizeof(ans));
            if (strcmp(parola, frase) == 0)
            {
                strcpy(ans, "hai vinto");
                sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&client_addr, len);
                running = 0;
                break;
            }
            else{
                strcpy(ans,"hai perso");
                sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&client_addr, len);
                running = 0;
                break;
            }
        }
        if(parolecorrette==10){
            strcpy(ans, "hai vinto");
                sendto(sockfd, ans, sizeof(ans), 0, (struct sockaddr *)&client_addr, len);
                running = 0;
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        perror("inserire porta\n");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("errore apertura socket\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, len);
    memset(&client_addr, 0, len);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&server_addr, len);

    printf("server listening\n");

    Registrazione();

    memset(ans, 0, sizeof(ans));
    strcpy(ans, "benvenuti player");

    while(running){
        Game();


    }
    

    close(sockfd);
}

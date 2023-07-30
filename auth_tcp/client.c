#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define buffersize 1024
#define namefilelength 100

int sockfd, newsockfd, n;
struct sockaddr_in client_addr, server_addr;
socklen_t len = sizeof(server_addr);
char ans[buffersize];
char namefile[namefilelength];
FILE*f;

pid_t child;

struct Reg
{
    char name[30];
    char passw[30];
};

struct Reg reg;

void registrazione()
{
    system("clear");
    printf("inserisci il nome: ");
    scanf("%s", reg.name);
    getchar();
    printf("inserisci password: ");
    scanf("%s", reg.passw);
    getchar();
    send(sockfd, &reg, sizeof(reg), 0);
    memset(ans, 0, sizeof(ans));
    n = recv(sockfd, ans, sizeof(ans), 0);
    ans[n] = 0;
    printf("%s", ans);
}

void loggedSeleziona()
{
    system("clear");
    printf("Benvenuto %s, cosa vuoi fare?\n", reg.name);
    printf("1. recv file  2. Logout\n");

    scanf("%s", ans);
    if (strcmp(ans, "1") == 0)
    {
        send(sockfd, ans, strlen(ans) + 1, 0);
        memset(&ans,sizeof(ans),0);

        recv(sockfd,ans,sizeof(ans),0);
        printf("%s\n",ans);

        fgets(namefile, sizeof(namefile), stdin);
        namefile[strcspn(namefile, "\n")] = 0;

        send(sockfd, namefile, sizeof(namefile), 0);

        // Open the file to save the received data
        if ((f = fopen("received_file.txt", "w")) == NULL)
        {
            perror("file\n");
            exit(EXIT_FAILURE);
        }

        // Receive data from the client and save it to the file
        memset(&ans,0,sizeof(ans));
        while ((n = recv(sockfd, ans, sizeof(ans), 0)) > 0)
        {
            if (fwrite(ans, 1, n, f) != n)
            {
                perror("file\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    else if (strcmp(ans, "2") == 0)
    {
        send(sockfd, ans, strlen(ans) + 1, 0);
        return;
    }
    else
    {
        printf("Scelta non valida\n");
        sleep(3);
        loggedSeleziona();
        return;
    }

    sleep(3);
    loggedSeleziona();
}

void login()
{
    system("clear");
    printf("Inserisci il nome utente: ");
    scanf("%s", reg.name);
    printf("Inserisci la password: ");
    scanf("%s", reg.passw);

    send(sockfd, &reg, sizeof(reg), 0);
    memset(ans, 0, sizeof(ans));
    n = recv(sockfd, ans, sizeof(ans) - 1, 0);
    ans[n] = '\0';

    if (n < 0)
    {
        perror("Errore nella ricezione\n");
        return;
    }

    if (strcmp(ans, "login correct") == 0)
    {
        printf("Login effettuato con successo\n");
        sleep(1);
        loggedSeleziona();
        return;
    }
    else
    {
        printf("Errore: %s\n", ans);
        return;
    }
}

void Del()
{

    system("clear");
    printf("ciao! chi vuoi elliminare?\n");
    printf("inserisci nome utente: ");
    scanf("%s", reg.name);
    printf("inserisci password: ");
    scanf("%s", reg.passw);

    send(sockfd, &reg, sizeof(reg), 0);
}



int main(int argc, char **argv)
{

    if (argc != 3)
    {
        perror("errore\n");
        return 0;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("errore apertura socket\n");
        return -1;
    }

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    inet_pton(AF_INET, argv[2], &server_addr.sin_addr);

    connect(sockfd, (struct sockaddr *)&server_addr, len);

    printf("connessione effettuata\n");

    while (1)
    {
        system("clear");
        printf("1. to register 2. to login 3. to DELETE 4. to logout \n");
        fgets(ans, sizeof(ans), stdin);
        ans[strcspn(ans, "\n")] = 0;
        if (strcmp(ans, "1") == 0)
        {
            send(sockfd, ans, strlen(ans) + 1, 0);
            registrazione();
        }
        else if (strcmp(ans, "2") == 0)
        {
            send(sockfd, ans, strlen(ans) + 1, 0);
            login();
        }
        else if (strcmp(ans, "3") == 0)
        {
            send(sockfd, ans, strlen(ans) + 1, 0);
            Del();
        }
        else if (strcmp(ans, "4") == 0)
        {
            send(sockfd, ans, strlen(ans) + 1, 0);
            break;
        }
    }

    close(sockfd);

    return 0;
}

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h> // per SIGTERM

#include <sys/types.h>
#include <signal.h>

#define Buffersize 1024

char ans[Buffersize];

int sockfd, n;
struct sockaddr_in remote_addr, local_addr;
socklen_t len = sizeof(struct sockaddr_in);

pid_t child;
void terminator(){
    kill(child,SIGTERM);
    kill(getpid(),SIGTERM);
}

struct User
{
    char name[30];
    char password[30];
};

struct User user;

void registrazione()
{
    system("clear");
    printf("inserisci nome utente: ");
    scanf("%s", user.name);
    getchar();
    printf("inserisci la password: ");
    scanf("%s", user.password);
    getchar();
    sendto(sockfd, &user, sizeof(user), 0, (struct sockaddr *)&remote_addr, len);
    memset(ans, 0, sizeof(ans));
    n = recvfrom(sockfd, ans, sizeof(ans) - 1, 0, (struct sockaddr *)&remote_addr, &len);
    ans[n] = '\0';
    if (n < 0)
    {
        perror("errore nella ricezione della risposta\n");
        return;
    }
    else
        printf("%s\n", ans);
}

void loggedSeleziona()
{

    system("clear");
    printf("Benvenuto %s, cosa vuoi fare?\n", user.name);
    printf("1. say hello 2. logout\n");
    scanf("%s", ans);
    if (strcmp(ans, "1") == 0)
    {
        sendto(sockfd, ans, strlen(ans) + 1, 0, (struct sockaddr *)&remote_addr, len);
    }
    else if (strcmp(ans, "2") == 0)
    {
        sendto(sockfd, ans, strlen(ans) + 1, 0, (struct sockaddr *)&remote_addr, len);
        return;
    }
    else{
        printf("scelta non valida\n");
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
    printf("inserisci il nome utente: ");
    scanf("%s", user.name);
    printf("inserisci la password: ");
    scanf("%s", user.password);
    
    sendto(sockfd, &user, sizeof(user), 0, (struct sockaddr *)&remote_addr, len);
    memset(ans, 0, sizeof(ans));
    n = recvfrom(sockfd, ans, sizeof(ans) - 1, 0, (struct sockaddr *)&remote_addr, &len);
    ans[n] = '\0';

    if (n < 0)
    {
        perror("errore nella ricezione\n");
        return;
    }

    if (strcmp(ans, "login correct") == 0)
    {
        printf("login effettuato con successo\n");
        sleep(1);
        loggedSeleziona();
        return;
    }
}



void Del(){

    system("clear");
    printf("ciao! chi vuoi elliminare?\n");
    printf("inserisci nome utente: ");
    scanf("%s",user.name);
    printf("inserisci password: ");
    scanf("%s",user.password);

    sendto(sockfd,&user,sizeof(user),0,(struct sockaddr*)&remote_addr,len);

}

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        perror("errore nell'inserimento dei parametri\n");
        return 0;
    }

    child = fork();
    if (child == 0)
    {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            perror("errore nell'apertura della socket\n");
            return -1;
        }

        memset(&local_addr, 0, len);
        local_addr.sin_family = AF_INET;
        local_addr.sin_port = htons(atoi(argv[1]));
        if (bind(sockfd, (struct sockaddr *)&local_addr, len) < 0)
        {
            perror("error binding\n");
            return -1;
        }

        recvfrom(sockfd, ans, sizeof(ans) - 1, 0, (struct sockaddr *)&local_addr, &len);
        if (strcmp(ans, "exit") == 0)
        {
            close(sockfd);
            exit(0);
        }
    }

    else
    {

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);

        if (sockfd < 0)
        {
            perror("errore nella creazione del socket\n");
            return -1;
        }

        memset(&remote_addr, 0, len);
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = htons(atoi(argv[2]));
        inet_pton(AF_INET, argv[3], &remote_addr.sin_addr);

        while (1)
        {
            system("clear");
            printf("Cosa vuoi fare?\n");
            printf("1. Registrazione. 2. Login. 3. Delete 4.exit\n");
            scanf("%s", ans);

            if (strcmp(ans, "1") == 0)
            {
                sendto(sockfd, ans, strlen(ans) + 1, 0, (struct sockaddr *)&remote_addr, len);
                registrazione();
                sleep(5);
            }
            else if (strcmp(ans, "2") == 0)
            {
                sendto(sockfd, ans, strlen(ans) + 1, 0, (struct sockaddr *)&remote_addr, len);
                login();
                sleep(5);
            }
            else if (strcmp(ans, "3") == 0)
            {
                sendto(sockfd, ans, strlen(ans) + 1, 0, (struct sockaddr *)&remote_addr, len);
                Del();
                sleep(5);
            }
            else if(strcmp(ans,"5")==0){
                close(sockfd);
                terminator();
            }
            else if(strcmp(ans,"4")==0){
                sendto(sockfd,ans,strlen(ans)+1,0,(struct sockaddr*)&remote_addr,len);
                break;
            }
        }
    }

    close(sockfd);

    return 0;
}
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>

#define buffersize 1024

int sockfd, n;
struct sockaddr_in server_addr, client_addr;
socklen_t len = sizeof(struct sockaddr_in);
FILE* file;

char ans[buffersize];

struct User {
    char name[30];
    char password[30];
};

struct read {
    struct User copia;
};

struct User user;
struct read reader;

void registrazione() {
    printf("Server ON: registrazione in corso\n");
    fflush(stdout);
    n = recvfrom(sockfd, &user, sizeof(user), 0, (struct sockaddr*)&client_addr, &len);
    if (n < 0) {
        perror("errore nella ricezione\n");
        return;
    }

    if ((file = fopen("user.txt", "a+")) == NULL) {
        perror("errore nell'apertura del file\n");
        return;
    }

    char line[200];
    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, "_");
        if (token != NULL)
            strcpy(line, token);
        if (strcmp(line, user.name) == 0) {
            strcpy(ans, "Errore: utente già registrato");
            sendto(sockfd, ans, sizeof(ans) + 1, 0, (struct sockaddr*)&client_addr, len);
            fclose(file);
            return;
        }
    }

    fprintf(file, "%s_%s\n", user.name, user.password);
    fclose(file);
    strcpy(ans, "registrazione avvenuta con successo");
    sendto(sockfd, ans, strlen(ans) + 1, 0, (struct sockaddr*)&client_addr, len);
}

void login() {
    printf("server on: login in corso\n");
    fflush(stdout);
    n = recvfrom(sockfd, &user, sizeof(user), 0, (struct sockaddr*)&client_addr, &len);
    if (n < 0) {
        perror("errore nella ricezione\n");
        return;
    }

    if ((file = fopen("user.txt", "r")) == NULL) {
        perror("file non esistente\n");
        return;
    }
    fseek(file, 0, SEEK_SET);

    char line[200];

    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, "_");
        if (token != NULL)
            strcpy(reader.copia.name, token);
        token = strtok(NULL, "\n");
        if (token != NULL)
            strcpy(reader.copia.password, token);
        if (strcmp(reader.copia.name, user.name) == 0 && strcmp(reader.copia.password, user.password) == 0) {
            strcpy(ans, "login correct");
            sendto(sockfd, ans, strlen(ans) + 1, 0, (struct sockaddr*)&client_addr, len);
            fclose(file);
            while (1) {
                n = recvfrom(sockfd, ans, sizeof(ans) - 1, 0, (struct sockaddr*)&client_addr, &len);
                ans[n] = '\0';
                if (n < 0) {
                    perror("errore ricezione\n");
                    return;
                }
                if (strcmp(ans, "1") == 0) {
                    printf("hello\n");
                }
                if (strcmp(ans, "2") == 0) {
                    break;
                }
            }
        }
        else {
            strcpy(ans, "password o nome utente errato");
            sendto(sockfd, ans, strlen(ans) + 1, 0, (struct sockaddr*)&client_addr, len);
            fclose(file);
            return;
        }
    }
}

void Del() {
    printf("Server ON: delete\n");
    fflush(stdout);
    recvfrom(sockfd, &user, sizeof(user), 0, (struct sockaddr*)&client_addr, &len);

    FILE* file;
    if ((file = fopen("user.txt", "r")) == NULL) {
        perror("Errore nell'apertura del file\n");
        return;
    }

    FILE* file_tmp;
    if ((file_tmp = fopen("usertmp.txt", "w+")) == NULL) {
        perror("Errore nell'apertura del file temporaneo\n");
        fclose(file);
        return;
    }

    while (fscanf(file, "%[^_]%*c%[^\n]%*c", reader.copia.name, reader.copia.password) == 2) {
        if (strcmp(user.name, reader.copia.name) != 0 || strcmp(user.password, reader.copia.password) != 0)
            fprintf(file_tmp, "%s_%s\n", reader.copia.name, reader.copia.password);
    }

    fclose(file);
    fclose(file_tmp);

    if (remove("user.txt") != 0) {
        perror("Errore nella rimozione del file\n");
        return;
    }
    if (rename("usertmp.txt", "user.txt") != 0) {
        perror("Errore nella rinomina del file temporaneo\n");
        return;
    }

    printf("Utente eliminato con successo\n");
}

int main(int argc, char** argv) {
    if (argc != 2) {
        perror("errore\n");
        return 0;
    }

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("errore apertura socket\n");
        return -1;
    }

    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&server_addr, len) < 0) {
        perror("errore nella bind\n");
        return -1;
    }
    int isRunning = 1;
    while (isRunning) {
        printf("server in attesa..\n");
        fflush(stdout);
        memset(ans, 0, sizeof(ans));
        n = recvfrom(sockfd, ans, sizeof(ans) - 1, 0, (struct sockaddr*)&client_addr, &len);
        ans[n] = '\0';
        if (strcmp(ans, "1") == 0)
            registrazione();
        else if (strcmp(ans, "4") == 0) {
            isRunning = 0;
        }
        else if (strcmp(ans, "2") == 0) {
            login();
        }
        else if(strcmp(ans,"3")==0){
            Del();
        }
    }

    close(sockfd);

    return 0;
}

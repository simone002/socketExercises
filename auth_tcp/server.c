#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define buffersize 1024
#define filnamelength 100

int sockfd, newsockfd, n;
struct sockaddr_in client_addr, server_addr;
socklen_t len = sizeof(server_addr);
char ans[buffersize];
char filename[filnamelength];
char buffer[buffersize];

struct Reg
{
    char name[30];
    char passw[30];
};


struct read{
    struct Reg copia;
};

struct Reg reg;
struct read reader;

FILE *f;

void send_file(char*nome){

    FILE*file;
    if ((file = fopen(nome, "r")) == NULL) {
        perror("file\n");
        exit(EXIT_FAILURE);
    }

    //send file to server

    while((n=fread(buffer,1,buffersize,file))>0){
        send(newsockfd,buffer,sizeof(buffer),0);
    }

    printf("file sent successfully\n");

    fclose(file);
}

void registrazione()
{
    printf("Server: registrazione\n");
    fflush(stdout);
    n = recv(newsockfd, &reg, sizeof(reg), 0);
    if ((f = fopen("users.txt", "a+")) == NULL)
    {
        perror("FILE NON ESISTENTE\n");
        return;
    }

    char line[200];
    while (fgets(line, sizeof(line), f))
    {
        char *token = strtok(line, "_");
        if (token != NULL)
            strcpy(line, token);
        if (strcmp(line, reg.name) == 0)
        {
            strcpy(ans, "utente già presente\n");
            send(newsockfd, ans, strlen(ans) + 1, 0);
            fclose(f);
            return;
        }
    }
    fprintf(f, "%s_%s\n", reg.name, reg.passw);
    fclose(f);
    strcpy(ans, "utente registrato\n");
    send(newsockfd, ans, strlen(ans) + 1, 0);
}

void login()
{
    printf("Server: Login in corso\n");
    fflush(stdout);

    n = recv(newsockfd, &reg, sizeof(reg), 0);
    if (n < 0)
    {
        perror("Errore nella ricezione\n");
        return;
    }

    FILE *f = fopen("users.txt", "r");
    if (f == NULL)
    {
        perror("File non esistente\n");
        return;
    }

    char line[200];

    while (fgets(line, sizeof(line), f))
    {
        char *token = strtok(line, "_");
        if (token != NULL)
            strcpy(reader.copia.name, token);
        token = strtok(NULL, "\n");
        if (token != NULL)
            strcpy(reader.copia.passw, token);

        if (strcmp(reader.copia.name, reg.name) == 0 && strcmp(reader.copia.passw, reg.passw) == 0)
        {
            strcpy(ans, "login correct");
            send(newsockfd, ans, strlen(ans) + 1, 0);
            fclose(f);

            while (1)
            {
                n = recv(newsockfd, ans, sizeof(ans) - 1, 0);
                ans[n] = '\0';
                if (n < 0)
                {
                    perror("Errore nella ricezione\n");
                    return;
                }
                if (strcmp(ans, "1") == 0)
                {
                    memset(&ans, 0, sizeof(ans));
                    strcpy(ans, "quale file vuoi ricevere?");
                    send(newsockfd, ans, sizeof(ans), 0);
                    n = recv(newsockfd, filename, sizeof(filename), 0);
                    if (n > 0)
                    {
                        send_file(filename);
                    }
                    else
                    {
                        printf("Errore nella ricezione del nome del file\n");
                    }
                }
                else if (strcmp(ans, "2") == 0)
                {
                    break;
                }
            }
            return;
        }
    }

    strcpy(ans, "password o nome utente errato");
    send(newsockfd, ans, strlen(ans) + 1, 0);
    fclose(f);
}

void Del(){
    printf("server ON: deleting");
    fflush(stdout);
    recv(newsockfd,&reg,sizeof(reg),0);

    FILE*file=fopen("users.txt","r");
    if(file==NULL){
        perror("file\n");
        exit(1);
    }
    
    FILE*file_tmp=fopen("users_tmp.txt","w+");
    if(file_tmp==NULL){
        perror("file\n");
        fclose(file);
        exit(1);
    }

    while(fscanf(file,"%[^_]%*c%[^\n]%*c",reader.copia.name,reader.copia.passw)==2){
        if((strcmp(reader.copia.name,reg.name)!=0)|| strcmp(reader.copia.passw,reg.passw)!=0 ){
            fprintf(file_tmp,"%s_%s\n",reader.copia.name,reader.copia.passw);
        }
    }

    fclose(file);
    fclose(file_tmp);

    if(remove("users.txt")!=0){
        perror("remove\n");
        exit(1);
    }

    if(rename("users_tmp.txt","users.txt")!=0){
        perror("rename\n");
        exit(1);
    }

    printf("utente elliminato con successo\n");
}

int main(int argc, char **argv)
{
    if (argc != 2)
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
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if ((bind(sockfd, (struct sockaddr *)&server_addr, len)) < 0)
    {
        perror("error binding\n");
        return -1;
    }

    if (listen(sockfd, 10) < 0)
    {
        perror("error listening\n");
        return -1;
    }

    printf("Server listening\n");

    newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &len);
    if (newsockfd < 0)
    {
        perror("Errore nell'accettazione della connessione");
        exit(1);
    }

    while (newsockfd > 0)
    {
        n = recv(newsockfd, ans, sizeof(ans), 0);
        ans[n] = '\0';

        if (strcmp(ans, "1") == 0)
        {
            registrazione();
        }
        else if (strcmp(ans, "2") == 0)
        {
            login();
        }
        else if (strcmp(ans, "3") == 0)
        {
            Del();
        }
        else if(strcmp(ans,"4")==0)
        {
            printf("Server: Logout\n");
            break;
        }
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}

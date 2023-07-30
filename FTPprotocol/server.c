#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>


int sockfd,n,newsocket;
struct sockaddr_in remote_addr, local_addr;
socklen_t len=sizeof(struct sockaddr_in);
char ans[1024];

struct user{

    char name[30];
    char listaFile[1000];
    int port;


};

struct Login{
    struct user copia;
};
struct Logged{
    char filename[1000];
};

struct user u;
struct Login c;
struct Logged l;

FILE*f;

void sendfile(char*nome){

    if ((f= fopen(nome, "r")) == NULL) {
        perror("file\n");
        exit(EXIT_FAILURE);
    }

    // Send the file to the server
    while ((n = fread(ans, 1, sizeof(ans), f)) > 0) {
        if (send(newsocket, ans, n, 0) < 0) {
            perror("send\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("File sent successfully\n");
    strcpy(ans,"FileTransferComplete");
    send(newsocket,ans,sizeof(ans),0);

    // Close the file and the connection
    fclose(f);
    
}

void reg(){

    fflush(stdout);
    printf("server ON: registrazione\n");
    recv(newsocket,&u,sizeof(u),0);


    if((f=fopen("users.txt","a+"))==NULL){
        perror("file not exists\n");
        exit(EXIT_FAILURE);
    }

    char line[200];
    while(fgets(line,sizeof(line),f)){
        char *totk=strtok(line," ");
        if(totk!=NULL)strcpy(line,totk);
        if(strcmp(line,u.name)==0){
            strcpy(ans,"utente già registrato");
            send(newsocket,ans,sizeof(ans),0);
            fclose(f);
            return;
        }
    }
    fprintf(f,"%s %d %s\n",u.name,u.port,u.listaFile);
    fclose(f);
    strcpy(ans,"utente registrato");
    send(newsocket,ans,sizeof(ans),0);
}

void login() {
    fflush(stdout);
    printf("server ON: login\n");

    recv(newsocket, &c, sizeof(c), 0);

    if ((f = fopen("users.txt", "r+")) == NULL) {
        perror("file non esistente\n");
        exit(EXIT_FAILURE);
    }

    char line[200];
    rewind(f);

    while (fgets(line, sizeof(line), f)) {
        char *tok = strtok(line, " ");
        if (tok != NULL) strcpy(line, tok);
        tok = strtok(NULL, " ");
        if (tok != NULL) c.copia.port = atoi(tok);
        tok = strtok(NULL, "\n");
        if (tok != NULL) strcpy(c.copia.listaFile, tok);
        if (strcmp(line, c.copia.name) == 0) {
            strcpy(ans, "login correct");
            send(newsocket, ans, sizeof(ans), 0);
            fclose(f);
            while(1){
                recv(newsocket,ans,sizeof(ans),0);
                if(strcmp(ans,"1")==0){
                    memset(ans,0,sizeof(ans));
                    strcpy(ans,"quale file vuoi ricevere?");
                    send(newsocket,ans,sizeof(ans),0);
                    char filename[30];
                    memset(filename,0,sizeof(filename));
                    recv(newsocket,filename,sizeof(filename),0);
                    sendfile(filename);
                }
                if(strcmp(ans,"2")==0){
                    break;
                }
            }
            return;
        }
    }

    // If the loop completes without finding a matching username, send login error.
    strcpy(ans, "login error");
    send(newsocket, ans, sizeof(ans), 0);
    fclose(f);
}

int main(int argc,char**argv){

    if(argc<2){
        perror("errore inserire indirizzo ip server e porta");
        exit(EXIT_FAILURE);
    }

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    
    if(sockfd<0){
        perror("error socket");
        exit(EXIT_FAILURE);
    }

    memset(&local_addr,0,len);
    memset(&remote_addr,0,len);

    local_addr.sin_family=AF_INET;
    local_addr.sin_port=htons(atoi(argv[1]));
    local_addr.sin_addr.s_addr=INADDR_ANY;

    bind(sockfd,(struct sockaddr*)&local_addr,len);

    listen(sockfd,5);

     newsocket=accept(sockfd,(struct sockaddr*)&remote_addr,&len);    

    
    while(1){
        memset(ans,0,sizeof(ans));
        recv(newsocket,ans,sizeof(ans),0);
        if(strcmp(ans,"1")==0){
            reg();
        }
        if(strcmp(ans,"2")==0){
            login();
        }
        if(strcmp(ans,"3")==0){
            break;
        }

    }
    close(newsocket);
    close(sockfd);
}
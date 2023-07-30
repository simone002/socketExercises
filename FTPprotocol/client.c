#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

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

int sockfd,n;
struct sockaddr_in client_addr, server_addr;
socklen_t len=sizeof(struct sockaddr_in);
char ans[1024];
FILE*f;


void recvfile() {
    // Open the file to save the received data
    if ((f = fopen("received_file.txt", "w")) == NULL) {
        perror("file\n");
        exit(EXIT_FAILURE);
    }
    memset(ans, 0, sizeof(ans));
    // Receive data from the server and save it to the file
    while ((n = recv(sockfd, ans, sizeof(ans), 0)) > 0) {
        if (strcmp(ans, "FileTransferComplete") == 0) {
            // File transfer is complete, break out of the loop
            break;
        }
        if (fwrite(ans, 1, n, f) != n) {
            perror("file\n");
            exit(EXIT_FAILURE);
        }
    }
    fclose(f);
}

void reg(){

    system("clear");
    printf("inserire nome utente: ");
    scanf("%s",u.name);
    getchar();
    printf("inserire lista file: separati da spazio _ ");
    fgets(u.listaFile,sizeof(u.listaFile),stdin);
    u.listaFile[strcspn(u.listaFile,"\n")]=0;
    printf("inserire porta: ");
    scanf("%d",&u.port);

    send(sockfd,&u,sizeof(u),0);
    recv(sockfd,ans,sizeof(ans),0);
    printf("%s",ans);
    recv(sockfd,&c,sizeof(c),0);
    recv(sockfd,&c,sizeof(c),0);
    fflush(stdout);
    printf("%s",c.copia.listaFile);
    sleep(5);

}

void loggedSeleziona(){
    system("clear");
    while(1){
        printf("inserire 1 per chiedere un file 2 per uscire\n");
        fgets(ans,sizeof(ans),stdin);
        ans[strcspn(ans,"\n")]=0;

        if(strcmp(ans,"1")==0){
            send(sockfd,ans,sizeof(ans),0);
            memset(ans,0,sizeof(ans));
            recv(sockfd,ans,sizeof(ans),0);
            printf("server:%s\n",ans);
            char filename[30];
            memset(filename,0,sizeof(filename));
            scanf("%s",filename);
            getchar();
            send(sockfd,filename,sizeof(filename),0);
            recvfile();

        }
        if(strcmp(ans,"2")==0){
            send(sockfd,ans,sizeof(ans),0);
            break;
        }
    }
}


void login(){

    system("clear");
    printf("inserire nome utente: ");
    scanf("%s",c.copia.name);

    send(sockfd,&c,sizeof(c),0);
    recv(sockfd,ans,sizeof(ans),0);
    printf("%s\n",ans);
    if(strcmp(ans,"login correct")==0){
        loggedSeleziona();
    }
    sleep(5);

}

void seleziona(){

    while(1){
        printf("inserire 1 per registrarsi 2 login 3 exit\n");
        fgets(ans,sizeof(ans),stdin);
        ans[strcspn(ans,"\n")]=0;

        if(strcmp(ans,"1")==0){
            send(sockfd,ans,sizeof(ans),0);
            reg();
        }
        if(strcmp(ans,"2")==0){
            send(sockfd,ans,sizeof(ans),0);
            login();
        }
        if(strcmp(ans,"3")==0){
            send(sockfd,ans,sizeof(ans),0);
            break;
        }
    }

}

int main(int argc,char**argv){

    if(argc<3){
        perror("errore inserire indirizzo ip server e porta");
        exit(EXIT_FAILURE);
    }

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    
    if(sockfd<0){
        perror("error socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr,0,len);

    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(atoi(argv[2]));
    inet_pton(AF_INET,argv[1],&server_addr.sin_addr);

    connect(sockfd,(struct sockaddr*)&server_addr,len);

    seleziona();
   
    close(sockfd);
}
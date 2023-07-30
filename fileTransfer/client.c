#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define buffersize 1024

void handle(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        handle("Insert an IP ADDRESS, PORT, and FILENAME\n");
    }

    FILE *file;
    char buffer[buffersize];
    int n;

    // Open the file to be sent
    if ((file = fopen(argv[3], "r")) == NULL) {
        handle("File not found\n");
    }

    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    // Opening socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        handle("Socket not opened correctly\n");
    }
    printf("Socket created\n");

    // Server address
    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &(server_addr.sin_addr)) <= 0) {
        handle("Invalid server address\n");
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&server_addr, len) < 0) {
        handle("Connection failed\n");
    }
    printf("Connected to the server\n");

    // Send the file to the server
    while ((n = fread(buffer, 1, buffersize, file)) > 0) {
        if (send(sockfd, buffer, n, 0) < 0) {
            handle("Error in send\n");
        }
    }

    printf("File sent successfully\n");

    // Close the file and the connection
    fclose(file);
    close(sockfd);

    return 0;
}
//il client invia un file al server

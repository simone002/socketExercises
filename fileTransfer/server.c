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
    if (argc < 2) {
        handle("Insert a PORT\n");
    }

    FILE *file;
    char buffer[buffersize];
    int n;

    int sockfd, client_sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    // Opening socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        handle("Socket not opened correctly\n");
    }
    printf("Socket created\n");

    // Server address
    memset(&server_addr, 0, len);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    // Bind
    if (bind(sockfd, (struct sockaddr*)&server_addr, len) < 0) {
        handle("Binding failed\n");
    }
    printf("Socket bound to port %s\n", argv[1]);

        // Listen
    if (listen(sockfd, 5) < 0) {
        handle("Listening failed\n");
    }
    printf("Server listening on port %s\n", argv[1]);

    // Accept incoming connections
    if ((client_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &len)) < 0) {
        handle("Error in accepting connection\n");
    }
    printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

    // Open the file to save the received data
    if ((file = fopen("received_file.txt", "w")) == NULL) {
        handle("Error in creating file\n");
    }

    // Receive data from the client and save it to the file
    while ((n = recv(client_sockfd, buffer, buffersize, 0)) > 0) {
        if (fwrite(buffer, 1, n, file) != n) {
            handle("Error in writing to file\n");
        }
    }

    printf("File received and saved\n");

    // Close the file and the connection
    fclose(file);
    close(client_sockfd);
    close(sockfd);

    return 0;
}

// il server riceve un file dal client
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(){
    int server_socket;
    struct sockaddr_in server_addr;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully\n");

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to server address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket bound successfully\n");

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening...\n");

    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t client_size = sizeof(client_addr);

    // Accept incoming connections
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_size)) >= 0) {
        printf("New connection, socket fd is %d, ip is : %s, port : %d\n", client_socket, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Send message to client
        char *message = "Hello from Server!\n";
        send(client_socket, message, strlen(message), 0);

        // Close the connection
        close(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
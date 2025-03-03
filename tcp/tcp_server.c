#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

int main(){
    struct addrinfo hints, *res;
    struct sockaddr_storage client_addr;
    int server_socket, client_socket;

    // load up address structs with getaddrinfo()

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;  


    if (getaddrinfo(NULL, "8080", &hints, &res) != 0) {
        perror("getaddrinfo failed");
        exit(EXIT_FAILURE);
    }

    // create socket
    server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (server_socket == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully\n");

    // bind socket
    if (bind(server_socket, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket bound successfully\n");

    freeaddrinfo(res);

    // Listen for incoming connections
    if (listen(server_socket, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening...\n");

    socklen_t client_size = sizeof(client_addr);

    // Accept incoming connections
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_size)) >= 0) {
        struct sockaddr_in *client_addr_in = (struct sockaddr_in *)&client_addr;
        printf("New connection, socket fd is %d, ip is : %s, port : %d\n", client_socket, 
        inet_ntoa(client_addr_in->sin_addr), 
        ntohs(client_addr_in->sin_port));

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
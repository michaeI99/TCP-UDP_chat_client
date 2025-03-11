#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX_BUFFER 1024
#define PORT_START 8080
#define PORT_END 8090

// Function to accept a client connection
int accept_client(int server_socket, struct sockaddr_storage *client_addr, socklen_t *client_size) {
    int client_socket = accept(server_socket, (struct sockaddr *)client_addr, client_size);
    if (client_socket < 0) {
        perror("accept failed");
        return -1;
    }

    struct sockaddr_in *client_addr_in = (struct sockaddr_in *)client_addr;
    printf("New connection, socket fd is %d, ip is : %s, port : %d\n", client_socket,
           inet_ntoa(client_addr_in->sin_addr),
           ntohs(client_addr_in->sin_port));

    return client_socket;
}

int main(){
    struct addrinfo hints, *res;
    struct sockaddr_storage client_addr1, client_addr2;
    int server_socket, client_socket1, client_socket2;

    // load up address structs
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;  

// automatically choose port from in a range of ports
int port_found = 0;

for(int port = PORT_START; port <= PORT_END; port++){

    // convert port number to string
    char port_str[6];
    snprintf(port_str, sizeof(port_str), '%d', port);
    if (getaddrinfo(NULL, port_str, &hints, &res) != 0) {
        perror("getaddrinfo failed");
        exit(EXIT_FAILURE);
    }
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

    socklen_t client_size1 = sizeof(client_addr1);
    socklen_t client_size2= sizeof(client_addr2); 

    // Accept incoming connections
   client_socket1 = accept_client(server_socket, &client_addr1, &client_size1);
    if (client_socket1 < 0) {
        exit(EXIT_FAILURE);
    }
    client_socket2 = accept_client(server_socket, &client_addr2, &client_size2);
    if (client_socket2 < 0) {
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_BUFFER];
    ssize_t bytes_received;

    while(1){
         // Receive data from client 1
        bytes_received = recv(client_socket1, buffer, MAX_BUFFER, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0)
            printf("Client 1 disconnected\n");
        else
            perror("recv failed from client 1");
            break;
        }
        // forward message to client2
        send(client_socket2, buffer, bytes_received, 0);

        // Receive data from client 2
        bytes_received = recv(client_socket2, buffer, MAX_BUFFER, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0)
            printf("Client 2 disconnected\n");
        else
            perror("recv failed from client 2");
            break;
        }
        // forward message to client1
        send(client_socket1, buffer, bytes_received, 0);

    }

    // Close the server sockets
    close(client_socket1);
    close(client_socket2);
    close(server_socket);

    return 0;
}
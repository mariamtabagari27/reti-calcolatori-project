#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

void cleanup_socket(int sock) {
#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup error.\n");
        return 1;
    }
#endif

    int sock, PORT;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char server_name[BUFFER_SIZE];

    printf("Enter the host name: ");
    if (fgets(server_name, sizeof(server_name), stdin) == NULL) {
        fprintf(stderr, "Error reading the host name.\n");
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }
    server_name[strcspn(server_name, "\n")] = 0;

    printf("Enter the port: ");
    if (scanf("%d", &PORT) != 1) {
        fprintf(stderr, "Error reading the port.\n");
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }
    getchar(); // Consume the newline character remaining in the buffer

    // Host resolution using getaddrinfo (instead of gethostbyname)
    struct addrinfo hints = {0}, *res;
    hints.ai_family = AF_INET; // IPv4 only
    hints.ai_socktype = SOCK_STREAM;

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", PORT);

    if (getaddrinfo(server_name, port_str, &hints, &res) != 0) {
        fprintf(stderr, "Error: unable to resolve host %s\n", server_name);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    // Socket creation
    if ((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        perror("Socket creation error");
        freeaddrinfo(res);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    // Connect to server
    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Connection error");
        cleanup_socket(sock);
        freeaddrinfo(res);
        return 1;
    }

    freeaddrinfo(res);

    // Receive welcome message
    if (recv(sock, buffer, BUFFER_SIZE - 1, 0) > 0) {
        buffer[BUFFER_SIZE - 1] = '\0'; // Ensure null-termination
        printf("Server: %s\n", buffer);
    } else {
        fprintf(stderr, "Error receiving the welcome message.\n");
        cleanup_socket(sock);
        return 1;
    }

    // Data sending loop
    while (1) {
        printf("Enter an alphanumeric string: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            fprintf(stderr, "Error reading input.\n");
            break;
        }
        buffer[strcspn(buffer, "\n")] = 0; // Remove the newline

        if (send(sock, buffer, strlen(buffer), 0) < 0) {
            perror("Error sending data");
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int valread = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (valread > 0) {
            buffer[valread] = '\0'; // Ensure null-termination
            printf("Server: %s\n", buffer);
        } else {
            fprintf(stderr, "Error receiving data from server.\n");
            break;
        }

        if (strstr(buffer, "Execution complete")) {
            printf("Termination requested by server.\n");
            break;
        }
    }

    // Cleanup
    cleanup_socket(sock);
    return 0;
}

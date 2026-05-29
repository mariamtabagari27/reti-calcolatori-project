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

    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    char server_name[BUFFER_SIZE];
    int PORT;

    printf("Enter the host name: ");
    if (fgets(server_name, sizeof(server_name), stdin) == NULL) {
        fprintf(stderr, "Error reading host name.\n");
        return 1;
    }
    server_name[strcspn(server_name, "\n")] = 0;

    printf("Enter the port: ");
    if (scanf("%d", &PORT) != 1) {
        fprintf(stderr, "Error reading port.\n");
        return 1;
    }
    getchar(); // Consume the newline character remaining in the buffer

    // Host resolution using getaddrinfo
    struct addrinfo hints = {0}, *res;
    hints.ai_family = AF_INET; // IPv4 only
    hints.ai_socktype = SOCK_DGRAM;

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

    // Send and receive data
    printf("Enter an alphanumeric string: ");
    if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
        fprintf(stderr, "Error reading input.\n");
        cleanup_socket(sock);
        freeaddrinfo(res);
        return 1;
    }
    buffer[strcspn(buffer, "\n")] = 0;

    if (sendto(sock, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen) < 0) {
        perror("Error sending data");
        cleanup_socket(sock);
        freeaddrinfo(res);
        return 1;
    }

    memset(buffer, 0, BUFFER_SIZE);
    socklen_t addr_len = res->ai_addrlen;

    if (recvfrom(sock, buffer, BUFFER_SIZE, 0, res->ai_addr, &addr_len) < 0) {
        perror("Error receiving data");
        cleanup_socket(sock);
        freeaddrinfo(res);
        return 1;
    }

    int numero = atoi(buffer);
    printf("String N:%d received from server address: %s\n", numero,
           inet_ntoa(((struct sockaddr_in *)res->ai_addr)->sin_addr));

    // Cleanup
    cleanup_socket(sock);
    freeaddrinfo(res);

    return 0;
}

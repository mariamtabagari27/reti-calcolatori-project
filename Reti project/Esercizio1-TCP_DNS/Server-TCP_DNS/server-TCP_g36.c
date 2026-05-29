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

#define PORT 53477
#define BUFFER_SIZE 1024

// Function to count the number of vowels in a string
int count_vowels(const char *str) {
    int count = 0;
    for (int i = 0; str[i]; i++) {
        if (strchr("aeiouAEIOU", str[i])) {
            count++;
        }
    }
    printf("Number of vowels: %d\n", count);
    return count;
}

// Function for socket closing and cleanup
void cleanup_socket(int sock) {
#ifdef _WIN32
    closesocket(sock);
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

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Socket creation
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation error");
#ifdef _WIN32
        WSACleanup();
#endif
        exit(EXIT_FAILURE);
    }

    // Server address configuration
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Listen on all addresses
    address.sin_port = htons(PORT);

    // Socket bind
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind error");
        cleanup_socket(server_fd);
#ifdef _WIN32
        WSACleanup();
#endif
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_fd, 3) < 0) {
        perror("Listen error");
        cleanup_socket(server_fd);
#ifdef _WIN32
        WSACleanup();
#endif
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
            perror("Accept error");
            cleanup_socket(server_fd);
#ifdef _WIN32
            WSACleanup();
#endif
            exit(EXIT_FAILURE);
        }

        // Hostname resolution using gethostbyaddr
        struct hostent *client_host = gethostbyaddr((const char *)&address.sin_addr, sizeof(address.sin_addr), AF_INET);
        if (client_host) {
            printf("Connection from: %s (%s)\n", client_host->h_name, inet_ntoa(address.sin_addr));
        } else {
            printf("Connection from: %s\n", inet_ntoa(address.sin_addr));
        }

        // Welcome message
        snprintf(buffer, BUFFER_SIZE, "Initial message from server: Welcome client %s\n",
                 client_host ? client_host->h_name : "anonymous");
        send(new_socket, buffer, strlen(buffer), 0);

        // Data reception and handling loop
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int valread = recv(new_socket, buffer, BUFFER_SIZE - 1, 0);
            if (valread <= 0) {
                printf("Connection closed by client.\n");
                break;
            }

            buffer[valread] = '\0'; // Ensure string null-termination
            printf("Message received: %s\n", buffer);

            int num_vowels = count_vowels(buffer);
            char response[BUFFER_SIZE];

            if (num_vowels % 2 == 0) {
                snprintf(response, BUFFER_SIZE, "N: %d, even number of vowels. Execution complete.\n", num_vowels);
                send(new_socket, response, strlen(response), 0);
                break;
            } else {
                snprintf(response, BUFFER_SIZE, "N: %d, odd number of vowels. Continue entering strings.\n", num_vowels);
                send(new_socket, response, strlen(response), 0);
            }
        }

        cleanup_socket(new_socket); // Close the client socket
    }

    // General cleanup
    cleanup_socket(server_fd);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}

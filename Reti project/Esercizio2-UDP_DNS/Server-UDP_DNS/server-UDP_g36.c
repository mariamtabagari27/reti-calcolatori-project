#ifdef _WIN32
#include <winsock2.h>
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
int count_vowels(const char *str)
{
    int count = 0;
    for (int i = 0; str[i]; i++)
    {
        if (strchr("aeiouAEIOU", str[i]))
            count++;
    }
    printf("Number of vowels: %d\n", count);
    return count;
}

int main()
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup error.\n");
        return 1;
    }
#endif

    int sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Socket creation
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation error");
#ifdef _WIN32
        WSACleanup();
#endif
        exit(EXIT_FAILURE);
    }

    // Server address configuration
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all local addresses
    server_addr.sin_port = htons(PORT);

    // Socket bind
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind error");
#ifdef _WIN32
        closesocket(sock);
        WSACleanup();
#else
        close(sock);
#endif
        exit(EXIT_FAILURE);
    }

    printf("UDP server listening on port %d...\n", PORT);

    while (1)
    {
        socklen_t addr_len = sizeof(client_addr); // addr_len type corrected
        memset(buffer, 0, BUFFER_SIZE);

        // Receive message from client
        int valread = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (valread > 0)
        {
            buffer[valread] = '\0';
            printf("Message received: %s\n", buffer);

            // IP address resolution using gethostbyaddr
            struct hostent *client_host = gethostbyaddr((const char *)&client_addr.sin_addr, sizeof(client_addr.sin_addr), AF_INET);
            if (client_host)
            {
                printf("Connection from: %s (%s)\n", client_host->h_name, inet_ntoa(client_addr.sin_addr));
            }
            else
            {
                printf("Connection from: %s\n", inet_ntoa(client_addr.sin_addr));
            }

            int num_vowels = count_vowels(buffer);
            char response[BUFFER_SIZE];

            snprintf(response, BUFFER_SIZE, "%d", num_vowels);
            printf("Response sent: %s\n", response);
            sendto(sock, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len);
        }
    }

#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif

    return 0;
}

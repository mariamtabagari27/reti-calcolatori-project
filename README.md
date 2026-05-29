# Reti di Calcolatori – Socket Programming Project

## Project Description

This project implements client-server communication using UDP and TCP sockets in C programming language.

The server receives an alphanumeric string from the client, counts the number of vowels contained in the string, and sends the result back to the client.

The purpose of the project is to demonstrate the use of:

* UDP communication
* TCP communication
* socket programming
* hostname resolution
* client-server architecture

## Files Included

### server-UDP_g36.c

UDP server that:

* waits for incoming client messages
* counts the vowels in the received string
* sends the number of vowels back to the client

### client-UDP_g36.c

UDP client that:

* connects to the server
* sends a string to the server
* receives the vowel count result

### server-TCP_g36.c

TCP server that:

* accepts client connections
* continuously processes incoming strings
* terminates communication when the number of vowels is even


## Technologies Used

* C Programming Language
* Socket API
* IPv4 Networking
* UDP Protocol
* TCP Protocol


## Main Networking Functions Used

* `socket()`
* `bind()`
* `listen()`
* `accept()`
* `send()`
* `recv()`
* `sendto()`
* `recvfrom()`
* `getaddrinfo()`
* `gethostbyaddr()`


## Compilation

### Linux / macOS

```bash
gcc server-UDP_g36.c -o serverUDP
gcc client-UDP_g36.c -o clientUDP
gcc server-TCP_g36.c -o serverTCP
```

### Windows

```bash
gcc server-UDP_g36.c -o serverUDP.exe -lws2_32
gcc client-UDP_g36.c -o clientUDP.exe -lws2_32
gcc server-TCP_g36.c -o serverTCP.exe -lws2_32
```


## Execution

Run UDP server:

```bash
./serverUDP
```

Run UDP client:

```bash
./clientUDP
```

Run TCP server:

```bash
./serverTCP
```


## Networking Concepts Demonstrated

* Client-server communication
* TCP sockets
* UDP sockets
* Hostname and IP address resolution
* Data transmission through sockets
* String processing in C


## 

Mariam Tabagari
For code development used Visual Studio Code and not Eclipse.
Project developed for the course - Reti di Calcolatori MZ

#pragma once
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 23456

class TcpServer
{
public:
    TcpServer();
    ~TcpServer();

    void run();
    void setup();
    void acceptConnections();

private:
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    const char *hello = "Hello from server";

};

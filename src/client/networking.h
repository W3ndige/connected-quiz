#ifndef NETWORKING_H
#define NETWORKING_H

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

extern const int PORT;
extern const int MAX_RECEIVE_BUFFER;

int receiveAndVerify(int client_fd, char *buffer);
void sendData(int client_fd, char *message);

#endif

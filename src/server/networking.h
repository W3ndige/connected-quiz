#ifndef NETWORKING_H
#define NETWORKING_H

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

extern const size_t PORT;
extern const size_t MAX_RECEIVE_BUFFER;

int receiveData(int client_fd, struct sockaddr_in destination, char *buffer);
int sendAndValidate(int client_fd, struct sockaddr_in destination, char *message);
int handleClientAnswers(int client_fd, struct sockaddr_in destination, char *correct_answer);

#endif

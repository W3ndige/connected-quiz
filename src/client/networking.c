#include "networking.h"

int receiveAndVerify(int client_fd, char *buffer) {
  int len = recv(client_fd, buffer, MAX_RECEIVE_BUFFER,0);
  if (len == -1) {
    perror("Error receiving data");
    return 0;
  }
  buffer[len] = '\0';
  send(client_fd, "OK", strlen("OK"), 0);
  return 1;
}

void sendData(int client_fd, char *message) {
  send(client_fd, message, strlen(message), 0);
  printf("Sending data: %s\nWaiting for confirmation...\n", message);
}

void getScore(int socket_fd, char *score) {
  receiveAndVerify(socket_fd, score);
}

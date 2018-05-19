#include "networking.h"

/*
 * Function: receiveAndVerify
 * ----------------------------
 *   Receive the data from the client and resend confirmation `OK`.
 *
 *   socket_fd: file descriptor for the open socket.
 *   buffer: pointer to buffer variable.
 *
 */

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

/*
 * Function: sendData
 * ----------------------------
 *   Sends the data.
 *
 *   client_fd: file descriptor of client that got accepted.
 *   message: pointer to a message to be send.
 *
 */

void sendData(int client_fd, char *message) {
  send(client_fd, message, strlen(message), 0);
  printf("Sending data: %s\nWaiting for confirmation...\n", message);
}

#include "networking.h"

/*
 * Function: receiveData
 * ----------------------------
 *   Receives data from the client and puts it in buffer. Returns
 *   0 on failure and 1 on success.
 *
 *   client_fd: file descriptor of client that got accepted.
 *   destination: structure that contains information about client.
 *   buffer: pointer to buffer variable.
 *
 */

int receiveData(int client_fd, struct sockaddr_in destination, char *buffer) {
  int len = recv(client_fd, buffer, MAX_RECEIVE_BUFFER,0);
  if (len == -1) {
    perror("Error receiving data");
    return 0;
  }
  else if (len == 0) {
    printf("Dropping connection from: %s\n", inet_ntoa(destination.sin_addr));
    return 0;
  }
  else {
    buffer[len] = '\0';
    return 1;
  }
}

/*
 * Function: sendAndValidate
 * ----------------------------
 *   Sends the data and waits for verification from the connected client.
 *   Returns 0 on failure and 1 on success.
 *
 *   client_fd: file descriptor of client that got accepted.
 *   destination: structure that contains information about client.
 *   message: pointer to a message to be send.
 *
 */

int sendAndValidate(int client_fd, struct sockaddr_in destination, char *message) {
  char confirmation_buffer[3];
  send(client_fd, message, strlen(message), 0);
  printf("Sending data: %s\nWaiting for confirmation...\n", message);
  if (receiveData(client_fd, destination, confirmation_buffer)) {
    printf("Got confirmation: %s\n", confirmation_buffer);
    if (!strncmp(confirmation_buffer, "OK", 2)) {
      return 1;
    }
  }
  return 0;
}

/*
 * Function: handleClientAnswers
 * ----------------------------
 *   Checks the answer that client sends and assign points.
 *   Returns 0 when the answer is incorrect, 1 when it's correct,
 *   and -1 on error.
 *
 *   client_fd: file descriptor of client that got accepted.
 *   destination: structure that contains information about client.
 *   correct_answer: pointer to correct answer.
 *
 */

int handleClientAnswers(int client_fd, struct sockaddr_in destination, char *correct_answer) {
  char buffer[MAX_RECEIVE_BUFFER];
  printf("Correct answer: %s\n", correct_answer);
  if (receiveData(client_fd, destination, buffer)) {
    printf("Got: %s\n", buffer);
    if (buffer[0] == correct_answer[0]) {
      sendAndValidate(client_fd, destination, "Correct answer!");
      return 1;
    }
    else {
      sendAndValidate(client_fd, destination, "Sorry, this time you were wrong!");
      return 0;
    }
  }
  else {
    return -1;
  }
}

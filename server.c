#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

// TODO(W3ndige): Client verification.
// TODO(W3ndige): Change buffer into dynamic data type.

const int PORT = 1337;
const int MAX_RECEIVE_BUFFER = 500;

int receiveData(int client_fd, struct sockaddr_in destination, char *buffer) {
  int len = recv(client_fd, buffer, MAX_RECEIVE_BUFFER,0);
  if (len == -1) {
    perror("Error receiving data");
    return 0;
  }
  if (len == 1 || len == 0) {
    printf("Dropping connection from: %s\n", inet_ntoa(destination.sin_addr));
    return 0;
  }
  else {
    buffer[len] = '\0';
    return 1;
  }
}

int sendAndValidate(int client_fd, struct sockaddr_in destination, char *message) {
  char confirmation_buffer[10];
  send(client_fd, message, strlen(message), 0);
  printf("Sending data: %s\nWaiting for confirmation...\n", message);
  if (receiveData(client_fd, destination, confirmation_buffer)) {
    printf("Got confirmation: %s\n", confirmation_buffer);
    if (!strcmp(confirmation_buffer, "OK")) {
      return 1;
    }
    else {
      return 0;
    }
  }
  return 0;
}

void handleClient(int socket_fd, socklen_t socket_size, struct sockaddr_in destination, char *questions, char **answers, int correct_answer) {
  char buffer[MAX_RECEIVE_BUFFER];
  int client_fd = accept(socket_fd, (struct sockaddr *)&destination, &socket_size);
  printf("New connection from: %s\n", inet_ntoa(destination.sin_addr));
  while (1) {
    if (sendAndValidate(client_fd, destination, questions)) {
      for (size_t i = 0; i < sizeof(answers) / 2; i++) {
        if (!sendAndValidate(client_fd, destination, answers[i])) {
          break;
        }
      }

      if (receiveData(client_fd, destination, buffer)) {
        printf("Got: %s\n", buffer);
        int answer = strtol(buffer, NULL, 10);
        if (answer == correct_answer + 1) {
          sendAndValidate(client_fd, destination, "Correct answer!");
        }
        else {
          sendAndValidate(client_fd, destination, "Sorry, this time you were wrong!");
        }
      }
      else {
        break;
      }
    }
    else {
      break;
    }
  }
  close(client_fd);
}

int main(int argc, char *argv[]) {
  struct sockaddr_in destination;
  struct sockaddr_in server;

  socklen_t socket_size = sizeof(struct sockaddr_in);

  memset(&destination, 0, sizeof(struct sockaddr_in));
  memset(&server, 0, sizeof(struct sockaddr_in));

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(PORT);

  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    perror("Couldn't create socket");
    return 1;
  }
  bind(socket_fd, (struct sockaddr *)&server, sizeof(struct sockaddr));
  listen(socket_fd, 1);

  // Temporary variables
  char *questions = "Where would you find the Sea of Tranquility?";
  char *answers[4] = {"Mars", "Titan", "Venus", "Moon"};
  int correct_answer = 3;

  while (1) {
    handleClient(socket_fd, socket_size, destination, questions, answers, correct_answer);
  }

  close(socket_fd);
  return 1;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

// TODO(W3ndige): Add error handling.

const int PORT = 1337;
const int MAX_RECEIVE_BUFFER = 500;

int main(int argc, char *argv[]) {
  char buffer[MAX_RECEIVE_BUFFER];
  char *message = "Hello there my slave.";

  struct sockaddr_in destination;
  struct sockaddr_in server;

  socklen_t socket_size = sizeof(struct sockaddr_in);

  memset(&destination, 0, sizeof(struct sockaddr_in));
  memset(&server, 0, sizeof(struct sockaddr_in));

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(PORT);

  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  bind(socket_fd, (struct sockaddr *)&server, sizeof(struct sockaddr));

  listen(socket_fd, 1);

  while (1) {
    int client_fd = accept(socket_fd, (struct sockaddr *)&destination, &socket_size);
    printf("New connection from: %s\n", inet_ntoa(destination.sin_addr));

    while (1) {
      send(client_fd, message, strlen(message), 0);
      printf("Sending: %s\n", message);

      int len = recv(client_fd, buffer, MAX_RECEIVE_BUFFER,0);

      if (len == 1) {
        printf("Dropping connection from: %s\n", inet_ntoa(destination.sin_addr));
        break;
      }

      buffer[len] = '\0';
      printf("Got: %s\n", buffer);
    }
    close(client_fd);
  }

  close(socket_fd);
  return EXIT_SUCCESS;
}

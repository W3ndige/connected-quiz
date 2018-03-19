#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

// TODO(W3ndige): Error handling.

const int PORT = 1337;
const int MAX_RECEIVE_BUFFER = 500;

int main(int argc, char *argv[]) {
   char buffer[MAX_RECEIVE_BUFFER];
   struct sockaddr_in destination;

   int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

   memset(&destination, 0, sizeof(struct sockaddr_in));
   destination.sin_family = AF_INET;
   destination.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
   destination.sin_port = htons(PORT);

   connect(socket_fd, (struct sockaddr *)&destination, sizeof(struct sockaddr_in));

   while (1) {
     int len = recv(socket_fd, buffer, sizeof(buffer),0);
     buffer[len] = '\0';
     printf("New message: %s\n", buffer);
     printf("Enter your response:\n");
     fgets(buffer, MAX_RECEIVE_BUFFER, stdin);
     send(socket_fd, buffer, strlen(buffer), 0);
   }


   close(socket_fd);
   return EXIT_SUCCESS;
}

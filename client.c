#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

// TODO(W3ndige): Sending end of connection message.
// TODO(W3ndige): Maximum buffer protection

const int PORT = 1337;
const int MAX_RECEIVE_BUFFER = 500;

void receiveAndVerify(int socket_fd, char *buffer) {
  int len = recv(socket_fd, buffer, MAX_RECEIVE_BUFFER, 0);
  buffer[len] = '\0';
  send(socket_fd, "OK", strlen("OK"), 0);
}

int main(int argc, char *argv[]) {
   char buffer[MAX_RECEIVE_BUFFER];
   struct sockaddr_in destination;
   int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
   memset(&destination, 0, sizeof(struct sockaddr_in));

   destination.sin_family = AF_INET;
   destination.sin_addr.s_addr = inet_addr("127.0.0.1");
   destination.sin_port = htons(PORT);

   if (connect(socket_fd, (struct sockaddr *)&destination, sizeof(struct sockaddr_in)) == -1) {
     perror("Error while connecting to server");
     return 1;
   }

   while (1) {
     receiveAndVerify(socket_fd, buffer);
     printf("New question: %s\n", buffer);
     for (int i  = 0; i < 4; i++) {
       receiveAndVerify(socket_fd, buffer);
       printf("%c. %s\n", 'A' + i, buffer);
     }
     printf("Enter your response:\n");
     fgets(buffer, MAX_RECEIVE_BUFFER, stdin);
     send(socket_fd, buffer, strlen(buffer), 0);

     receiveAndVerify(socket_fd, buffer);
     printf("%s\n", buffer);
   }

   close(socket_fd);
   return 1;
}

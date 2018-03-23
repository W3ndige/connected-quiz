#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

// TODO(W3ndige): Error handling.
// TODO(W3ndige): Sending end of connection message.
// TODO(W3ndige): Maximum buffer protection
// TODO(W3ndige): Client have to confirm that he got the question, only then the server will send another message.

const int PORT = 1337;
const int MAX_RECEIVE_BUFFER = 500;

int main(int argc, char *argv[]) {
   char buffer[MAX_RECEIVE_BUFFER];
   struct sockaddr_in destination;
   int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
   memset(&destination, 0, sizeof(struct sockaddr_in));

   destination.sin_family = AF_INET;
   destination.sin_addr.s_addr = inet_addr("192.168.0.248");
   destination.sin_port = htons(PORT);

   if (connect(socket_fd, (struct sockaddr *)&destination, sizeof(struct sockaddr_in)) == -1) {
     perror("Error while connecting to server");
     return 1;
   }

   while (1) {
     int len = recv(socket_fd, buffer, sizeof(buffer),0);
     buffer[len] = '\0';
     printf("New question: %s\n", buffer);
     for (int i = 0; i < 4; i++) {
       memset(buffer, 0, sizeof(buffer) / sizeof(buffer[0]));
       len = recv(socket_fd, buffer, sizeof(buffer),0);
       buffer[len] = '\0';
       printf("%i. %s\n", i + 1, buffer);
     }
     printf("Enter your response:\n");
     fgets(buffer, MAX_RECEIVE_BUFFER, stdin);
     send(socket_fd, buffer, strlen(buffer), 0);
   }

   close(socket_fd);
   return 1;
}

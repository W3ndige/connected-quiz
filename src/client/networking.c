#include "networking.h"

int loadTheConfig(char *ip_address, size_t ip_address_size) {
  FILE *config_file = fopen("client_assets/config.cfg", "r");
  if (config_file == NULL) {
    perror("Couldn't locate config.cfg file.");
    return 0;
  }
  char line[100];
  fgets(line, 100, config_file);
  if (!strncmp("IP", line, 2)) {
    fgets(ip_address, ip_address_size, config_file);
    return 1;
  }
  perror("No IP marker. Please place word IP at the top line, and actuall IP address at the line under the word.");
  return 0;
}

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

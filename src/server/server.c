/**
 * @file
 * @date 29 May 2018
 * @brief File contains main functions used behind the server logic. 
 *
 */

#include "processes.h"

const size_t PORT = 1337;
const size_t NUM_OF_ANSWERS = 4;
const size_t MAX_RECEIVE_BUFFER = 500;
const size_t MAX_ANSWER_LINE_SIZE = 100;
const size_t MAX_QUESTION_LINE_SIZE = 300;
const size_t MAX_CATEGORY_LINE_SIZE = 100;
const size_t MAX_NUMBER_OF_CONNECTIONS = 20;
const char *CATEGORIES_FILENAME = "server_assets/categories.txt";

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused))) {

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

  int num_of_categories = calculateNumberOfCategories(CATEGORIES_FILENAME);
  int total_number_of_questions = calculateTotalNumberOfQuestions();
  printf("Total: %d questions in %d categories\n", total_number_of_questions, num_of_categories);

  struct question_info *questions = malloc(sizeof(struct question_info) * total_number_of_questions);
  if (questions == NULL) {
    perror("Couldn't create questions array");
    return 1;
  }
  populateQuestions(questions, num_of_categories);

  while (1) {
    handleClientConnection(socket_fd, socket_size, destination, num_of_categories,
                           questions, total_number_of_questions);
  }

  free(questions);
  close(socket_fd);
  return 0;
}

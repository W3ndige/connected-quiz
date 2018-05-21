#include "processes.h"

const size_t PORT = 1337;
const size_t NUM_OF_ANSWERS = 4;
const size_t MAX_RECEIVE_BUFFER = 500;
const size_t MAX_ANSWER_LINE_SIZE = 100;
const size_t MAX_QUESTION_LINE_SIZE = 300;
const size_t MAX_CATEGORY_LINE_SIZE = 100;
const size_t MAX_NUMBER_OF_CONNECTIONS = 20;
const char *CATEGORIES_FILENAME = "server_assets/categories.txt";

/*
 * Function: handleClientConnection
 * ----------------------------
 *   Create the pipe, and forks according to global variable
 *   MAX_NUMBER_OF_CONNECTIONS. Then handleChildProcess and
 *   handleParentProcess are being called.
 *
 *   socket_fd: file descriptor for the open socket.
 *   socket_size: size of the socket.
 *   destination: structure that contains information about client.
 *   num_of_categories: number of categories.
 *
 */

void handleClientConnection(int socket_fd, socklen_t socket_size, struct sockaddr_in destination, int num_of_categories, struct question_info questions[], int total_number_of_questions) {
  int pipefd_to_child[2];
  int pipefd_to_parent[2];

  // We're going to create a pipeline between parent and child processes.
  if (pipe(pipefd_to_child) == -1) {
    fprintf(stderr, "Pipe to child failed\n");
  }
  if (pipe(pipefd_to_parent) == -1) {
    fprintf(stderr, "Pipe to parent failed\n");
  }

  struct user_score score_table[MAX_NUMBER_OF_CONNECTIONS];
  resetScoreTable(score_table);

  // Handle maximum number of processes connected simultaneously.
  pid_t children[MAX_NUMBER_OF_CONNECTIONS];
  for (size_t i = 0; i < MAX_NUMBER_OF_CONNECTIONS; i++) {
    if ((children[i] = fork()) == 0) {
      if (children[i] < 0) {
        fprintf(stderr, "Error in process creation.\n");
        break;
      }
      handleChildProcess(socket_fd, socket_size, destination, num_of_categories, pipefd_to_child, pipefd_to_parent, questions, total_number_of_questions);
    }
  }

  // Parent is taking care of this part of the process.
  handleParentProcess(score_table, pipefd_to_child, pipefd_to_parent);
}

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
  int total_number_of_questions = calculateTotalNumberOfQuestions(CATEGORIES_FILENAME);
  printf("Total: %d questions in %d categories\n", total_number_of_questions, num_of_categories);

  struct question_info questions[total_number_of_questions];
  populateQuestions(questions, num_of_categories);

  while (1) {
    handleClientConnection(socket_fd, socket_size, destination, num_of_categories, questions, total_number_of_questions);
  }

  close(socket_fd);
  return 0;
}

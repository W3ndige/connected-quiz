#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

const size_t PORT = 1337;
const size_t NUM_OF_ANSWERS = 4;
const size_t MAX_RECEIVE_BUFFER = 500;
const size_t MAX_ANSWER_LINE_SIZE = 100;
const size_t MAX_QUESTION_LINE_SIZE = 300;
const size_t MAX_CATEGORY_LINE_SIZE = 100;
const size_t MAX_NUMBER_OF_CONNECTIONS = 20;
const char *CATEGORIES_FILENAME = "server_assets/categories.txt";

struct user_score {
  int pid;
  int score;
};

/*
 * Function: calculateNumberOfCategories
 * ----------------------------
 *   Returns the number of categories in categories file.
 *   or -1 on error.
 *
 *   filename: filename of categories file.
 *
 */

int calculateNumberOfCategories(const char *filename) {
  FILE *categories_file = fopen(filename, "r");
  if (!categories_file) {
    perror("Could not open category file");
    return -1;
  }
  else {
    int num_of_categories = 0;
    while (!feof(categories_file)) {
      char line[MAX_CATEGORY_LINE_SIZE];
      fgets(line, MAX_CATEGORY_LINE_SIZE, categories_file);
      if (strncmp(line,"END_OF_LIST",11) == 0) {
        break;
      }
      else {
        num_of_categories++;
      }
    }
    fclose(categories_file);
    return num_of_categories - 1;
  }
}

/*
 * Function: calculateNumberOfQuestions
 * ----------------------------
 *   Returns the number of questions in question file,
 *   or -1 on error.
 *
 *   filename: filename of questions file.
 *
 */

int calculateNumberOfQuestions(char *filename) {
  FILE *questions_file = fopen(filename, "r");
  if (!questions_file) {
    perror("Could not open category file");
    puts(filename);
    return -1;
  }
  else {
    int num_of_questions = 0;
    while (!feof(questions_file)) {
      char line[MAX_QUESTION_LINE_SIZE];
      fgets(line, MAX_QUESTION_LINE_SIZE, questions_file);
      if (strncmp(line,"BEGIN_OF_QUESTION",17) == 0) {
        num_of_questions++;
      }
    }
    fclose(questions_file);
    return num_of_questions;
  }
}

/*
 * Function: properlyTerminateString
 * ----------------------------
 *   Replaces the newline character with end of string character.
 *
 *   string: pointer to a char array
 *
 */

void properlyTerminateString(char *string) {
  for (size_t i = 0; i <  strlen(string); i++) {
    if (string[i] == '\n') {
      string[i] = '\0';
    }
  }
}

int calculateTotalNumberOfQuestions() {
  int total_number_of_questions = 0;
  FILE *categories_file = fopen(CATEGORIES_FILENAME, "r");
  if (!categories_file) {
    fprintf(stderr, "Could not open %s: %s\n", CATEGORIES_FILENAME, strerror(errno));
    return -1;
  }
  char category_source[MAX_CATEGORY_LINE_SIZE];
  fgets(category_source, sizeof(category_source) / sizeof(category_source[0]), categories_file);
  while (!feof(categories_file) ) {
    fgets(category_source, sizeof(category_source) / sizeof(category_source[0]), categories_file);
    if (strncmp(category_source,"END_OF_LIST",11) == 0) {
      break;
    }
    properlyTerminateString(category_source);
    int number_of_questions = calculateNumberOfQuestions(category_source);
    if (number_of_questions > 0) {
      total_number_of_questions += number_of_questions;
    }
  }
  fclose(categories_file);
  return total_number_of_questions;
}

/*
 * Function: getRandomCategory
 * ----------------------------
 *   Chooses the random category and puts it's filename
 *   into the category_source. Returns -1 on error and
 *   line of choosen category.
 *
 *   category_source: pointer to which a choosen category will be saved.
 *   size: size of the buffer that fget will get from the line.
 *   num_of_categories: number of categories.
 *
 */

 int getRandomCategory(char *category_source, size_t size, size_t num_of_categories) {

   int rand_category = rand() % num_of_categories;
   FILE *categories_file = fopen(CATEGORIES_FILENAME, "r");
   if (!categories_file) {
     fprintf(stderr, "Could not open %s: %s\n", CATEGORIES_FILENAME, strerror(errno));
     return -1;
   }
   int counted_line = 0;
   while (!feof(categories_file)) {
     fgets(category_source, size, categories_file);
     if (counted_line == rand_category) {
       fgets(category_source, size, categories_file);
       break;
     }
      counted_line++;
    }
   fclose(categories_file);
   return counted_line;
 }

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
    else {
      return 0;
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

/*
 * Function: askRandomQuestion
 * ----------------------------
 *   Gets the random question from the random category, sens it to the client
 *   following with the sending of possible answers. Correct answer is being
 *   saved in a buffer and compared with client answer.
 *   Returns -1 on error, 0 if anwer is incorrect and 1 if correct.
 *
 *   client_fd: file descriptor of client that got accepted.
 *   destination: structure that contains information about client.
 *   num_of_categories: number of categories in categories file.
 *
 */

int askRandomQuestion(int client_fd, struct sockaddr_in destination, int num_of_categories) {
  char category_source[50];

  // Firstly check if getRandomCategory() function will run correctly
  int category_number = getRandomCategory(category_source, sizeof(category_source) / sizeof(category_source[0]), num_of_categories);
  if (category_number == -1) {
    return -1;
  }

  properlyTerminateString(category_source);
  printf("Choosen category: %s\n", category_source);
  FILE *questions_file = fopen(category_source, "r");

  if (!questions_file) {
    perror("Could not open category file");
    return -1;
  }

  int counted_line = 0;
  int rand_question = rand() % calculateNumberOfQuestions(category_source) + 1;
  char correct_answer[2];
  char line[MAX_QUESTION_LINE_SIZE];

  while (!feof(questions_file)) {
    fgets(line, MAX_QUESTION_LINE_SIZE, questions_file);

    if (!strncmp(line, "BEGIN_OF_QUESTION", 17)) {
      counted_line++;
    }

    if (counted_line == rand_question) {
      memset(line, 0, MAX_QUESTION_LINE_SIZE);
      fgets(line, MAX_QUESTION_LINE_SIZE, questions_file);
      properlyTerminateString(line);
      if (!sendAndValidate(client_fd, destination, line)) {
        fclose(questions_file);
        return -1;
      }

      fgets(line, MAX_QUESTION_LINE_SIZE, questions_file);
      for (size_t i = 0; i < NUM_OF_ANSWERS; i++) {
        char answer[MAX_ANSWER_LINE_SIZE];
        fgets(answer, MAX_ANSWER_LINE_SIZE, questions_file);
        properlyTerminateString(answer);
        if (!sendAndValidate(client_fd, destination, answer)) {
          fclose(questions_file);
          return -1;
        }
      }
      fgets(correct_answer, 2, questions_file);
      properlyTerminateString(correct_answer);
      break;
    }
  }
  fclose(questions_file);

  return handleClientAnswers(client_fd, destination, correct_answer);
}

/*
 * Function: resetScoreTable
 * ----------------------------
 *   Zeroes every element in score table.
 *
 *   score_table: array of user_score structures containing pid of user and score.
 *
 */

void resetScoreTable(struct user_score score_table[]) {
  for (size_t i = 0; i < MAX_NUMBER_OF_CONNECTIONS; i++) {
    score_table[i].pid = 0;
    score_table[i].score = 0;
  }
}

/*
 * Function: resetScoreTable
 * ----------------------------
 *   Places the user score in the proper place of the score_table.
 *   After the process, prints the current scoreboard.
 *
 *   score_table: array of user_score structures containing pid of user and score.
 *   client_pid: PID of the client that score is about to be placed.
 *   points: number of points to be put.
 *
 */

void updateScoreTable(struct user_score score_table[], int client_pid, int points) {
  // If user already in table, change the score.
  int in_table = 0;
  for (size_t i = 0; i < MAX_NUMBER_OF_CONNECTIONS; i++) {
    if (score_table[i].pid == client_pid) {
      score_table[i].score += points;
      in_table = 1;
      break;
    }
  }
  // If not, add it to the first empty place.
  if (!in_table) {
    for (size_t i = 0; i < MAX_NUMBER_OF_CONNECTIONS; i++) {
      if (score_table[i].pid == 0) {
        score_table[i].pid = client_pid;
        score_table[i].score += points;
        break;
      }
    }
  }
  puts("--------------SCOREBOARD--------------");
  for (size_t i = 0; i < MAX_NUMBER_OF_CONNECTIONS; i++) {
    if (score_table[i].pid != 0) {
      printf("PID: %d SCORE: %d\n", score_table[i].pid, score_table[i].score);
    }
  }
  puts("--------------------------------------");
}

/*
 * Function: handleChildProcess
 * ----------------------------
 *   Accept a new connection, ask question using askRandomQuestion,
 *   points are then sent together with PID to the parent process using
 *   pipe.
 *
 *   socket_fd: file descriptor for the open socket.
 *   socket_size: size of the socket.
 *   destination: structure that contains information about client.
 *   num_of_categories: number of categories.
 *   pipefd: array containing pipe file descriptors.
 *
 */

void handleChildProcess(int socket_fd, socklen_t socket_size, struct sockaddr_in destination, int num_of_categories, int pipefd[], int total_number_of_questions) {

  pid_t child_pid = getpid();
  srand(time(0) + (int)child_pid);
  int client_fd = accept(socket_fd, (struct sockaddr *)&destination, &socket_size);
  printf("New connection from: %s at PID: %d\n", inet_ntoa(destination.sin_addr), child_pid);
  close(pipefd[0]); // Close the read end of pipe, child is only going to write.

  int possible_questions[total_number_of_questions];
  memset(possible_questions, 0, total_number_of_questions * sizeof(int));

  while (1) {
    int points = askRandomQuestion(client_fd, destination, num_of_categories);
    if (points == -1) {
      fprintf(stderr, "Error while asking question.\n");
      break;
    }
    else if (points >= 0) {
      char message_to_parent[20];
      snprintf(message_to_parent, 20, "%d %d", (int)child_pid, points);
      write(pipefd[1], message_to_parent, strlen(message_to_parent));
    }
  }
  close(pipefd[1]); // Before function end close the write end of the pipe.
  close(client_fd);
}

/*
 * Function: handleParentProcess
 * ----------------------------
 *   Reads message from child sent over the pipe,
 *   updates the score table according to it.
 *
 *   score_table: array of user_score structures containing pid of user and score.
 *   pipefd: array containing pipe file descriptors.
 *
 */

void handleParentProcess(struct user_score score_table[], int pipefd[]) {
  close(pipefd[1]); // Close the write end of pipe, parent is only going to read.
  char message_from_child[20];
  while (1) {
    int client_pid;
    int points;
    read(pipefd[0], message_from_child, 20);
    if (message_from_child[0] == '\0') {
      break;
    }
    sscanf(message_from_child, "%d %d", &client_pid, &points);
    updateScoreTable(score_table, client_pid, points);
    message_from_child[0] = '\0';
  }

  close(pipefd[0]);
  signal(SIGCHLD, SIG_IGN);
}

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

void handleClientConnection(int socket_fd, socklen_t socket_size, struct sockaddr_in destination, int num_of_categories, int total_number_of_questions) {
  int pipefd[2];

  // We're going to create a pipeline between parent and child processes.
  if (pipe(pipefd) == -1) {
    fprintf(stderr, "Pipe failed\n");
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
      handleChildProcess(socket_fd, socket_size, destination, num_of_categories, pipefd, total_number_of_questions);
    }
  }

  // Parent is taking care of this part of the process.
  handleParentProcess(score_table, pipefd);
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

  int num_of_categories = calculateNumberOfCategories(CATEGORIES_FILENAME);
  int total_number_of_questions = calculateTotalNumberOfQuestions(CATEGORIES_FILENAME);
  printf("%d questions %d categories\n", total_number_of_questions, num_of_categories);

  while (1) {
    handleClientConnection(socket_fd, socket_size, destination, num_of_categories, total_number_of_questions);
  }

  close(socket_fd);
  return 0;
}

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

const int PORT = 1337;
const int NUM_OF_ANSWERS = 4;
const int MAX_RECEIVE_BUFFER = 500;
const int MAX_ANSWER_LINE_SIZE = 100;
const int MAX_QUESTION_LINE_SIZE = 300;
const int MAX_CATEGORY_LINE_SIZE = 100;
const char *CATEGORIES_FILENAME = "server_assets/categories.txt";

struct user_score {
  int pid;
  int score;
};

int calculateNumberOfCategories(const char *filename) {
  FILE *categories_file = fopen(filename, "r");
  if (!CATEGORIES_FILENAME) {
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

int calculateNumberOfQuestions(char *filename) {
  FILE *questions_file = fopen(filename, "r");
  if (!questions_file) {
    perror("Could not open category file");
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

void properlyTerminateString(char *string) {
  for (size_t i = 0; i <  strlen(string); i++) {
    if (string[i] == '\n') {
      string[i] = '\0';
    }
  }
}

int getRandomCategory(char *category_source, size_t size, size_t num_of_categories) {
  int rand_category = rand() % num_of_categories;
  FILE *categories_file = fopen(CATEGORIES_FILENAME, "r");
  if (!categories_file) {
    fprintf(stderr, "Could not open %s: %s\n", CATEGORIES_FILENAME, strerror(errno));
    return 0;
  }
  else {
    int counted_line = 0;
    while (!feof(categories_file)) {
      fgets(category_source, size, categories_file);
      if (counted_line == rand_category) {
        fgets(category_source, size, categories_file);
        break;
      }
      counted_line++;
    }
  }
  fclose(categories_file);
  return 1;
}

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

int askRandomQuestion(int client_fd, struct sockaddr_in destination, int num_of_categories) {
  char category_source[50];

  // Firstly check if getRandomCategory() function will run correctly
  if (!getRandomCategory(category_source, sizeof(category_source) / sizeof(category_source[0]), num_of_categories)) {
    return 0;
  }

  properlyTerminateString(category_source);
  printf("Choosen category: %s\n", category_source);
  FILE *questions_file = fopen(category_source, "r");

  if (!questions_file) {
    perror("Could not open cateogory file");
    return 0;
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
      for (int i = 0; i < NUM_OF_ANSWERS; i++) {
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

void handleClientConnection(int socket_fd, socklen_t socket_size,struct sockaddr_in destination, int num_of_categories) {
  int pipefd[2];

  int client_fd = accept(socket_fd, (struct sockaddr *)&destination, &socket_size);
  if (pipe(pipefd) == -1) {
    fprintf(stderr, "Pipe failed\n");
  }

  pid_t pid = fork();
  if (pid < 0) {
    fprintf(stderr, "Error in process creation.\n" );
    return;
  }

  struct user_score score_table[20];
  for (int i = 0; i < 20; i++) {
    score_table[i].pid = 0;
    score_table[i].score = 0;
  }

  if (pid == 0) { // Child
    pid_t pid_num = getpid();
    srand(time(0) + (int)pid_num);
    printf("New connection from: %s at PID: %d\n", inet_ntoa(destination.sin_addr), pid_num);
    close(pipefd[0]);

    while (1) {
      int points = askRandomQuestion(client_fd, destination, num_of_categories);
      if (points == -1) {
        puts("An error occured.");
        break;
      }
      else {
        char message_to_parent[20];
        snprintf(message_to_parent, 20, "%d %d", (int)pid_num, points);
        write(pipefd[1], message_to_parent, strlen(message_to_parent));
      }
    }
    close(pipefd[1]);
  }

  else { // Parent
    int client_pid;
    int points;
    char message_from_child[20];
    close(pipefd[1]);

    while (1) {
      read(pipefd[0], message_from_child, 20);
      if (message_from_child[0] == '\0') {
        break;
      }

      sscanf(message_from_child, "%d %d", &client_pid, &points);
      // If user already in table, change the score
      for (int i = 0; i < 20; i++) {
        if (score_table[i].pid == client_pid) {
          score_table[i].score += points;
          break;
        }
      }
      // If not, add it to the first empty place.
      for (int i = 0; i < 20; i++) {
        if (score_table[i].pid == 0) {
          score_table[i].pid = client_pid;
          score_table[i].score += points;
          break;
        }
      }
      puts("--------------SCOREBOARD--------------");
      for (int i = 0; i < 20; i++) {
        if (score_table[i].pid != 0) {
          printf("PID: %d SCORE: %d\n", score_table[i].pid, score_table[i].score);
        }
      }
      puts("--------------------------------------");
      message_from_child[0] = '\0';
    }
    close(pipefd[0]);
  }

  close(pipefd[0]);
  close(pipefd[1]);
  close(client_fd);
  signal(SIGCHLD, SIG_IGN);
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

  while (1) {
    handleClientConnection(socket_fd, socket_size, destination, num_of_categories);
  }

  close(socket_fd);
  return 1;
}

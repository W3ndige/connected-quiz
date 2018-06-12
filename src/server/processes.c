#include "processes.h"

static volatile int running = 1;

void interruptHandler(int sig __attribute__ ((unused))) {
  running = 0;
}

void resetScoreTable(struct user_score score_table[]) {
  for (size_t i = 0; i < MAX_NUMBER_OF_CONNECTIONS; i++) {
    score_table[i].pid = 0;
    score_table[i].score = 0;
  }
}

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

int sendToProcessAndVerify(int *pipefd_to_child, int *pipefd_to_parent, char *message) {
  char verification[2];
  write(pipefd_to_parent[1], message, strlen(message));
  read(pipefd_to_child[0], verification, 2);
  if (strncmp(verification, "OK", 2)) {
    fprintf(stderr, "Error in interprocess verification.\n");
    return 0;
  }
  return 1;
}

void readFromProcessAndVerify(int *pipefd_to_child, int *pipefd_to_parent, char *message, int message_size) {
  read(pipefd_to_parent[0], message, message_size);
  write(pipefd_to_child[1], "OK", strlen("OK"));
}

void handleChildProcess(int socket_fd, socklen_t socket_size, struct sockaddr_in destination,
                        int num_of_categories, int *pipefd_to_child, int *pipefd_to_parent,
                        struct question_info *questions, int total_number_of_questions) {

  close(pipefd_to_parent[0]); // Close the read end of pipe, child is only going to write.
  close(pipefd_to_child[1]);
  pid_t child_pid = getpid();
  srand(time(0) + (int)child_pid);
  int client_fd = accept(socket_fd, (struct sockaddr *)&destination, &socket_size);
  printf("New connection from: %s at PID: %d\n", inet_ntoa(destination.sin_addr), child_pid);

  int mode;
  int questions_asked = 0;
  char game_mode[2];
  int num_of_questions = 0;
  char message_to_parent[20];
  struct question_info *tmp_questions;


  if (receiveData(client_fd, destination, game_mode)) {
    printf("Game mode: %s\n", game_mode);
    if (!strncmp(game_mode, "1", 1)) {
      shuffleQuestions(questions, total_number_of_questions);
    }
    else if (!strncmp(game_mode, "2", 1)) {

      mode = 2;
      int category = rand() % num_of_categories;
      int question_offset = 0;
      for (int i = 0; i < total_number_of_questions; i++) {
        if (questions[i].category_number == category) {
          num_of_questions++;
        }
      }
      tmp_questions = malloc(num_of_questions * sizeof(struct question_info));
      if (tmp_questions == NULL) {
        fprintf(stderr, "Error during memory allocation.\n");
        return;
      }

      if (category == 0) {
        memcpy(tmp_questions, questions, num_of_questions * sizeof(struct question_info));
      }
      else {
        for (int i = 0; i < total_number_of_questions; i++) {
          if (questions[i].category_number == category) {
            question_offset = i;
            break;
          }
        }
        memcpy(tmp_questions, questions + question_offset,
              num_of_questions * sizeof(struct question_info));
      }
      shuffleQuestions(tmp_questions, num_of_questions);
    }
    else {
      shuffleQuestions(questions, total_number_of_questions);
      mode = 3;
    }
  }

  signal(SIGINT, interruptHandler);
  while (running) {
    if (questions_asked == total_number_of_questions ||
       (mode == 2 && questions_asked == num_of_questions)) {
      if (sendAndValidate(client_fd, destination, "END_OF_QUESTIONS")) {
        printf("Questions ended.\n");
        break;
      }
    }
    int points;
    if (mode == 2) {
      points = askQuestion(client_fd, destination, tmp_questions, questions_asked);
    }
    else {
      points = askQuestion(client_fd, destination, questions, questions_asked);
    }
    if (points == -1) {
      fprintf(stderr, "Error while asking question.\n");
      break;
    }
    questions_asked++;

    if (mode == 3) {
      if (points == 1) {
        if (sendToProcessAndVerify(pipefd_to_child, pipefd_to_parent, "PTSCORE")) {
          snprintf(message_to_parent, 20, "%d %d", (int)child_pid, points);
          if (!sendToProcessAndVerify(pipefd_to_child, pipefd_to_parent, message_to_parent)) {
            fprintf(stderr, "Error in interprocess communication.\n");
            break;
          }
        }
        else {
          fprintf(stderr, "Error in interprocess communication.\n");
          break;
        }
      }
      else if (points == 0) {
        printf("Player %d failed in hotshot game mode!\n", child_pid);
        break;
      }
    }
    else {
      if (points >= 0) {
        if (sendToProcessAndVerify(pipefd_to_child, pipefd_to_parent, "PTSCORE")) {
          snprintf(message_to_parent, 20, "%d %d", (int)child_pid, points);
          if (!sendToProcessAndVerify(pipefd_to_child, pipefd_to_parent, message_to_parent)) {
            fprintf(stderr, "Error in interprocess communication 1.\n");
            break;
          }
        }
        else {
          fprintf(stderr, "Error in interprocess communication 1.\n");
          break;
        }
      }
    }
    // Now we are able to send the score to the user.
    if (sendToProcessAndVerify(pipefd_to_child, pipefd_to_parent, "GTSCORE")) {
      snprintf(message_to_parent, 20, "%d", (int)child_pid);
      if (!sendToProcessAndVerify(pipefd_to_child, pipefd_to_parent, message_to_parent)) {
        fprintf(stderr, "Error in interprocess communication 1.\n");
      }
      char score[20];
      read(pipefd_to_child[0], score, 20);
      sendAndValidate(client_fd, destination, score);
    }
  }

  free(tmp_questions);
  close(pipefd_to_parent[1]); // Before function end close the write end of the pipe.
  close(pipefd_to_child[0]);
  close(client_fd);
}

void handleParentProcess(struct user_score score_table[], int *pipefd_to_child, int *pipefd_to_parent) {
  close(pipefd_to_parent[1]);
  close(pipefd_to_child[0]);
  char message_from_child[20];
  signal(SIGINT, interruptHandler);
  while (running) {
    int client_pid, points;
    readFromProcessAndVerify(pipefd_to_child, pipefd_to_parent, message_from_child, 7);
    if (!strncmp(message_from_child, "PTSCORE", 7)) {
      readFromProcessAndVerify(pipefd_to_child, pipefd_to_parent, message_from_child, 20);
      if (message_from_child[0] == '\0') {
        break;
      }
      sscanf(message_from_child, "%d %d", &client_pid, &points);
      updateScoreTable(score_table, client_pid, points);
      message_from_child[0] = '\0';
    }
    if (!strncmp(message_from_child, "GTSCORE", 7)) {
      readFromProcessAndVerify(pipefd_to_child, pipefd_to_parent, message_from_child, 20);
      sscanf(message_from_child, "%d", &client_pid);
      for (size_t i = 0; i < MAX_NUMBER_OF_CONNECTIONS; i++) {
        if (score_table[i].pid == client_pid) {
          points = score_table[i].score;
        }
      }
      char score[20];
      snprintf(score, 20, "%d", (int)points);
      write(pipefd_to_child[1], score, strlen(score));
    }
  }
  close(pipefd_to_child[1]);
  close(pipefd_to_parent[0]);
  signal(SIGCHLD, SIG_IGN);
}

void handleClientConnection(int socket_fd, socklen_t socket_size, struct sockaddr_in destination,
                            int num_of_categories, struct question_info *questions,
                            int total_number_of_questions) {

 int pipefd_to_child[2];
 int pipefd_to_parent[2];

 // We're going to create a pipeline between parent and child processes.
 if (pipe(pipefd_to_child) == -1) {
   fprintf(stderr, "Pipe to child failed\n");
   return;
 }
 if (pipe(pipefd_to_parent) == -1) {
   fprintf(stderr, "Pipe to parent failed\n");
   return;
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
     handleChildProcess(socket_fd, socket_size, destination, num_of_categories,
                        pipefd_to_child, pipefd_to_parent, questions, total_number_of_questions);
   }
 }

 // Parent is taking care of this part of the process.
 handleParentProcess(score_table, pipefd_to_child, pipefd_to_parent);
}

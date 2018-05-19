#ifndef PROCESSES_H
#define PROCESSES_H

#include <unistd.h>
#include <signal.h>
#include "game.h"

struct user_score {
  int pid;
  int score;
};

void resetScoreTable(struct user_score score_table[]);
void updateScoreTable(struct user_score score_table[], int client_pid, int points);
int sendToProcessAndVerify(int *pipefd_to_child, int *pipefd_to_parent, char *message);
void readFromProcessAndVerify(int *pipefd_to_child, int *pipefd_to_parent, char *message, int message_size);
void handleChildProcess(int socket_fd, socklen_t socket_size, struct sockaddr_in destination, int num_of_categories, int *pipefd_to_child, int *pipefd_to_parent, int total_number_of_questions);
void handleParentProcess(struct user_score score_table[], int *pipefd_to_child, int *pipefd_to_parent);

#endif

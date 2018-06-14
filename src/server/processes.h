/**
 * @file
 * @date 29 May 2018
 * @brief File contains functions in interaction between processes.
 *
 */

#ifndef PROCESSES_H
#define PROCESSES_H

#include <unistd.h>
#include <signal.h>
#include "game.h"

struct user_score { /**< Structure containing information client score. */
  int pid;
  int score;
};

/**
* @brief Places the user score in the proper place of the score_table and prints scoreboard.
*
* @param[out] struct user_score score_table: array of user_score structures containing pid of user and score.
*/

void resetScoreTable(struct user_score score_table[]);

/**
* @brief Places the user score in the proper place of the score_table and prints scoreboard.
*
* @param[out] struct user_score score_table: array of user_score structures containing pid of user and score.
* @param[in] int client_pid: PID of the client that score is about to be placed.
* @param[in] int points: number of points to be put.
*/

void updateScoreTable(struct user_score score_table[], int client_pid, int points);

/**
* @brief Send to process and verify if the information was received.
*
* @param[in] int *pipefd_to_child: array of user_score structures containing pid of user and score.
* @param[in] int *pipefd_to_parent: PID of the client that score is about to be placed.
* @param[in] char *message: number of points to be put.
* @return Returns 0 on error or 1 on success.
*/

int sendToProcessAndVerify(int *pipefd_to_child, int *pipefd_to_parent, char *message);

/**
* @brief Read data sent from another process and send confirmation.
*
* @param[in] int *pipefd_to_child: array of user_score structures containing pid of user and score.
* @param[in] int *pipefd_to_parent: PID of the client that score is about to be placed.
* @param[out] char *message: number of points to be put.
* @param[in] int message_size Size of the message.
*/

void readFromProcessAndVerify(int *pipefd_to_child, int *pipefd_to_parent,
                              char *message, int message_size);

 /**
 * @brief Accept a new connection, ask question using askRandomQuestion,
 * points are then sent together with PID to the parent process using pipe.
 *
 * @param[in] int socket_fd: file descriptor for the open socket.
 * @param[in] int socket_size: size of the socket.
 * @param[in] struct sockaddr_in destination: structure that contains information about client.
 * @param[in] struct question_info num_of_categories: number of categories.
 * @param[in] int pipefd: array containing pipe file descriptors.
 */

void handleChildProcess(int socket_fd, socklen_t socket_size, struct sockaddr_in destination,
                        int num_of_categories, int *pipefd_to_child, int *pipefd_to_parent,
                        struct question_info *questions, int total_number_of_questions);

 /**
 * @brief Reads message from child sent over the pipe,  updates the score table according to it.
 *
 * @param[in] struct user_score score_table: array of user_score structures containing pid of user and score.
 * @param[in] int pipefd: array containing pipe file descriptors.
 */


void handleParentProcess(struct user_score score_table[], int *pipefd_to_child,
                         int *pipefd_to_parent);

 /**
 * @brief Create the pipe, and forks according to global variable
 * MAX_NUMBER_OF_CONNECTIONS. Then handleChildProcess and
 * handleParentProcess are being called.
 *
 * @param[in] int socket_fd: file descriptor for the open socket.
 * @param[in] int socket_size: size of the socket.
 * @param[in] struct user_score destination: structure that contains information about client.
 * @param[in] int num_of_categories: number of categories.
 */

 void handleClientConnection(int socket_fd, socklen_t socket_size, struct sockaddr_in destination,
                             int num_of_categories, struct question_info *questions,
                             int total_number_of_questions);

#endif

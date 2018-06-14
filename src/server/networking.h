/**
 * @file
 * @date 29 May 2018
 * @brief File contains functions in sending data between client and server.
 *
 */

#ifndef NETWORKING_H
#define NETWORKING_H

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

extern const size_t PORT;
extern const size_t MAX_RECEIVE_BUFFER;

 /**
 * @brief Receives data from the client and puts it in buffer.
 *
 * @param[in] client_fd File descriptor of client that got accepted.
 * @param[in] destination Structure that contains information about client.
 * @param[out] buffer: pointer to buffer variable.
 * @return Returns 1 on success or 0 on error.
 */

int receiveData(int client_fd, struct sockaddr_in destination, char *buffer);


/**
* @brief Sends the data and waits for verification from the connected client.
*
* @param[in] client_fd File descriptor of client that got accepted.
* @param[in] destination Structure that contains information about client.
* @param[out] message: pointer to message that is meant to be sent.
* @return Returns 1 on success or 0 on error.
*/

int sendAndValidate(int client_fd, struct sockaddr_in destination, char *message);

/**
* @brief Checks the answer that client sends and assign points.
*
* @param[in] client_fd File descriptor of client that got accepted.
* @param[in] destination Structure that contains information about client.
* @param[out] correct_answer: pointer to correct answer.
* @return Returns 1 when answer is correct, otherwise 0 or -1 on error.
*/

int handleClientAnswers(int client_fd, struct sockaddr_in destination, char *correct_answer);

#endif

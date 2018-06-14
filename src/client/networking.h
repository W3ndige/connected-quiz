/**
 * @file
 * @date 29 May 2018
 * @brief File contains functions used in interactions between server and client
 *        sending and receiving data over the network.
 *
 */

#ifndef NETWORKING_H
#define NETWORKING_H

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

extern const int PORT;  /**< Port used in server - client communication. */
extern const int MAX_RECEIVE_BUFFER;  /**< Maximum buffer that the client can receive. */

/**
* @brief Get the IP address of the server from the config file.
*
* @param ip_address Pointer to buffer that will get the IP address.
* @param ip_address_size Maximum size of the ip address buffer.
*/

int loadTheConfig(char *ip_address, size_t ip_address_size);

/**
* @brief Receive the data from the client and resend confirmation `OK`.
*
* @param client_fd File descriptor for the open socket.
* @param buffer Pointer to buffer variable.
*/

int receiveAndVerify(int client_fd, char *buffer);

/**
* @brief Send the data to the server.
*
* @param client_fd File descriptor for the open socket.
* @param message Pointer to message variable.
*/

void sendData(int client_fd, char *message);

/**
* @brief Send the data to the server.
*
* @param client_fd File descriptor for the open socket.
* @param message Pointer to score variable.
*/

void getScore(int client_fd, char *score);

#endif

/**
 * @file
 * @author W3ndige
 * @date 29 May 2018
 * @brief File contains functions used in order to get the question data from
 * files.
 *
 */

#ifndef GAME_H
#define GAME_H

#include <time.h>
#include "files.h"
#include "networking.h"

extern const size_t NUM_OF_ANSWERS;
extern const size_t MAX_RECEIVE_BUFFER;
extern const size_t MAX_ANSWER_LINE_SIZE;


/**
* @brief Sends the question data to the client.
*
* @param client_fd File descriptor of client that got accepted.
* @param destination Structure that contains information about client.
* @param questions Array containing questions.
* @param question_number Number of current question.
* @return User score or -1 on error.
*/

int askQuestion(int client_fd, struct sockaddr_in destination,
                      struct question_info *questions,
                      int question_number);

/**
* @brief Shuffle the questions in order to make order random.
*
* @param[out] questions Array containing questions.
* @param total_number_of_questions Total number of available questions.
*/


void shuffleQuestions(struct question_info questions[],
                      int total_number_of_questions);

#endif

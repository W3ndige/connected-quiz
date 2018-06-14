/**
 * @file
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
* @param int client_fd File descriptor of client that got accepted.
* @param struct sockaddr_in destination Structure that contains information about client.
* @param struct question_info *questions Array containing questions.
* @param int question_number Number of current question.
* @return User score or -1 on error.
*/

int askQuestion(int client_fd, struct sockaddr_in destination,
                      struct question_info *questions,
                      int question_number);

/**
* @brief Shuffle the questions in order to make order random.
*
* @param[out] struct question_info *questions Pointer to rray containing questions.
* @param int total_number_of_questions Total number of available questions.
*/


void shuffleQuestions(struct question_info *questions,
                      int total_number_of_questions);

#endif

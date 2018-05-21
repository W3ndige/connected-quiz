#ifndef GAME_H
#define GAME_H

#include <time.h>
#include "files.h"
#include "networking.h"

extern const size_t NUM_OF_ANSWERS;
extern const size_t MAX_RECEIVE_BUFFER;
extern const size_t MAX_ANSWER_LINE_SIZE;

int askQuestion(int client_fd, struct sockaddr_in destination,
                      struct question_info questions[],
                      int question_number);
                      
void shuffleQuestions(struct question_info questions[],
                      int total_number_of_questions);

#endif

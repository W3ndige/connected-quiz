#ifndef GAME_H
#define GAME_H

#include <time.h>
#include "files.h"
#include "networking.h"

extern const size_t NUM_OF_ANSWERS;
extern const size_t MAX_RECEIVE_BUFFER;
extern const size_t MAX_ANSWER_LINE_SIZE;

int askRandomQuestion(int client_fd, struct sockaddr_in destination, int num_of_categories, int mode);

#endif

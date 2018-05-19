#ifndef INTERACTION_H
#define INTERACTION_H

#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

extern const int MAX_RECEIVE_BUFFER;
extern const int NUM_OF_ANSWERS;
extern const int NUM_OF_MODES;
extern SDL_Rect background_location;

bool sendClickedAnswer(int socket_fd, int mouse_x, int mouse_y, SDL_Rect answer_location);
bool chooseGameMode(int socket_fd, int mouse_x, int mouse_y, SDL_Rect text_location);

#endif

#ifndef PRINTING_H
#define PRINTING_H

#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

extern const int MAX_RECEIVE_BUFFER;
extern const int NUM_OF_ANSWERS;
extern const int NUM_OF_MODES;
extern SDL_Rect background_location;

bool getPrintQuestion(SDL_Surface *screen, SDL_Surface *background, TTF_Font *font, SDL_Surface *text_surface, int socket_fd, SDL_Rect text_location, SDL_Rect answer_location, SDL_Color foreground_color);
bool printGameMode(SDL_Surface *screen, SDL_Surface *background, TTF_Font *font, TTF_Font *header_font, SDL_Surface *text_surface, SDL_Rect mode_location, SDL_Color foreground_color);
bool getPrintAnswer(SDL_Surface *screen, TTF_Font *font, SDL_Surface *text_surface, int socket_fd, SDL_Rect score_location, SDL_Color foreground_color);
void getScore(int socket_fd, char *score);
void printUserScore(SDL_Surface *screen, TTF_Font *font, SDL_Surface *text_surface, SDL_Color foreground_color, char *score);

#endif

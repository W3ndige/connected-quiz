/**
 * @file
 * @date 29 May 2018
 * @brief File contains functions used in printing information for the user.
 *
 */

#ifndef PRINTING_H
#define PRINTING_H

#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

extern const int MAX_RECEIVE_BUFFER; /**< Maximum buffer that the client can receive. */
extern const int NUM_OF_ANSWERS;     /**< Number of available answers. */
extern const int NUM_OF_MODES;       /**< Number of available modes. */
extern SDL_Rect background_location; /**< Coordinates of background rectangle. */

/**
* @brief Converts the longer question into the one consisting of 2 lines.
*
* @param char *buffer Pointer to buffer variable that contains question.
* @param char *first_line Pointer to first_line variable that will contain first part of question.
* @param size_t first_line_size Size of first_line buffer.
* @param char *second_line  Pointer to second_line variable that will contain second part of question.
* @param size_t second_line_size Size of second_line buffer.
*/
void convertQuestionToMultipleLines(char *buffer, char *first_line, size_t first_line_size, 
                                    char *second_line, size_t second_line_size);

/**
* @brief Get the current question from the server, print it, get the answers and print them.
*
* @param SDL_Surface *screen: pointer to SDL surface of the screen.
* @param SDL_Surface *background: pointer to SDL surface of the background image.
* @param TTF_Font *font: pointer to TTF font used in rendering.
* @param SDL_Surface text_surface: pointer to SDL surface of the text area.
* @param int socket_fd: file descriptor for the open socket.
* @param SDL_Rect text_location: coordinates of the text location in SDL_Rect.
* @param SDL_Rect answer_location: coordinates of the answer location in SDL_Rect.
* @param SDL_Color foreground_color: foreground color used in rendering.
*/

bool getPrintQuestion(SDL_Surface *screen, SDL_Surface *background, TTF_Font *font, 
                      SDL_Surface *text_surface, int socket_fd, SDL_Rect text_location, 
                      SDL_Rect answer_location, SDL_Color foreground_color);

/**
* @brief Print all available game modes during startup of client.
*
* @param SDL_Surface *screen: pointer to SDL surface of the screen.
* @param SDL_Surface *background: pointer to SDL surface of the background image.
* @param TTF_Font *font: pointer to TTF font used in rendering.
* @param TTF_Font *header_font: pointer to TTF header font used in rendering of headers.
* @param SDL_Surface *text_surface: pointer to SDL surface of the text area.
* @param int socket_fd: file descriptor for the open socket.
* @param SDL_Rect mode_location: coordinates of the modes location in SDL_Rect.
* @param SDL_Color foreground_color: foreground color used in rendering.
*/

bool printGameMode(SDL_Surface *screen, SDL_Surface *background, TTF_Font *font, 
                   TTF_Font *header_font, SDL_Surface *text_surface, SDL_Rect mode_location, 
                   SDL_Color foreground_color);

/**
* @brief Print the information whether the answer was correct.
*
* @param SDL_Surface *screen: pointer to SDL surface of the screen.
* @param SDL_Surface *background: pointer to SDL surface of the background image.
* @param TTF_Font *font: pointer to TTF font used in rendering.
* @param TTF_Font *header_font: pointer to TTF header font used in rendering of headers.
* @param SDL_Surface *text_surface: pointer to SDL surface of the text area.
* @param int socket_fd: file descriptor for the open socket.
* @param SDL_Rect score_location: coordinates of the score location in SDL_Rect.
* @param SDL_Color foreground_color: foreground color used in rendering.
*/

bool getPrintAnswer(SDL_Surface *screen, TTF_Font *font, SDL_Surface *text_surface, 
                    int socket_fd, SDL_Rect score_location, SDL_Color foreground_color);

/**
* @brief Print current user score.
*
* @param SDL_Surface *screen: pointer to SDL surface of the screen.
* @param TTF_Font *font: pointer to TTF font used in rendering.
* @param SDL_Surface *text_surface: pointer to SDL surface of the text area.
* @param SDL_Color foreground_color: foreground color used in rendering.
* @param char *score: Pointer to variable containing current score.
*/

void printUserScore(SDL_Surface *screen, TTF_Font *font, SDL_Surface *text_surface, 
                    SDL_Color foreground_color, char *score);

#endif

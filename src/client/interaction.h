/**
 * @file
 * @date 29 May 2018
 * @brief File contains functions used in interactions between user and client.
 *
 */

#ifndef INTERACTION_H
#define INTERACTION_H

#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

extern const int MAX_RECEIVE_BUFFER;  /**< Maximum buffer that the client can receive. */
extern const int NUM_OF_ANSWERS;  /**< Number of available answers. */
extern const int NUM_OF_MODES;  /**< Number of available modes. */
extern SDL_Rect background_location;  /**< Coordinates of background rectangle. */

 /**
 * @brief Send code of the answer clicked by the user into the server.
 *
 * @param int socket_fd File desciptor of the socked used to send the data to server.
 * @param int mouse_x Value of X axis of the mouse.
 * @param int mouse_y Value of Y axis of the mouse.
 * @param SDL_Rect answer_location Coordinates of the answer rectangle.
 * @return True if the function ended correctly, otherwise false.
 */

bool sendClickedAnswer(int socket_fd, int mouse_x, int mouse_y, SDL_Rect answer_location);

/**
* @brief Parse user input in order to choose game mode, send value to server.
*
* @param int socket_fd File desciptor of the socked used to send the data to server.
* @param int mouse_x Value of X axis of the mouse.
* @param int mouse_y Value of Y axis of the mouse.
* @param SDL_Rect answer_location Coordinates of the text rectangle.
* @return False if the function ended correctly, otherwise true.
*/

bool chooseGameMode(int socket_fd, int mouse_x, int mouse_y, SDL_Rect text_location);

#endif

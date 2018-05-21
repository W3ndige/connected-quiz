#include "printing.h"
#include "networking.h"

void convertQuestionToMultipleLines(char *buffer, char *first_line,
                                    size_t first_line_size, char *second_line,
                                    size_t second_line_size) {
  memset(first_line, 0, first_line_size);
  memset(second_line, 0, second_line_size);

  size_t where_to_cut = 10;
  for (size_t i = 0; i < first_line_size - 1; i++) {
    if (buffer[i] == ' ') {
      where_to_cut = i;
    }
  }

  strncpy(first_line, buffer, where_to_cut);
  strncpy(second_line, buffer + where_to_cut + 1, 40);
  first_line[where_to_cut + 1] = '\0';
  second_line[41] = '\0';
}

/*
 * Function: getPrintQuestion
 * ----------------------------
 *   Receive question together with answers and update the SDL screen.
 *
 *   screen: pointer to SDL surface of the screen.
 *   font: pointer to TTF font used in rendering.
 *   text_surface: pointer to SDL surface of the text area.
 *   socket_fd: file descriptor for the open socket.
 *   text_location: coordinates of the text location in SDL_Rect.
 *   answer_location: coordinates of the answer location in SDL_Rect.
 *   foreground_color: foreground color used in rendering.
 *   background_color: background color used in rendering.
 *
 */

bool getPrintQuestion(SDL_Surface *screen, SDL_Surface *background,
                      TTF_Font *font, SDL_Surface *text_surface, int socket_fd,
                      SDL_Rect text_location, SDL_Rect answer_location,
                      SDL_Color foreground_color) {
  char buffer[MAX_RECEIVE_BUFFER];
  int text_width, text_height;
  SDL_BlitSurface(background, NULL, screen, &background_location);
  receiveAndVerify(socket_fd, buffer);
  if (!strncmp(buffer, "END_OF_QUESTIONS", 16)) {
    return true;
  }
  TTF_SizeText(font, buffer, &text_width, &text_height);
  if (text_width > 425) { // Now we have to perform  mutli line conversion.
    char first_line[41];
    char second_line[41];
    convertQuestionToMultipleLines(buffer, first_line, 41, second_line, 41);
    text_surface = TTF_RenderText_Blended(font, first_line, foreground_color);
    SDL_BlitSurface(text_surface, NULL, screen, &text_location);
    text_location.y += 25;
    text_surface = TTF_RenderText_Blended(font, second_line, foreground_color);
    SDL_BlitSurface(text_surface, NULL, screen, &text_location);
  }
  else {
    text_surface = TTF_RenderText_Blended(font, buffer, foreground_color);
    SDL_BlitSurface(text_surface, NULL, screen, &text_location);
  }
  for (int i = 0; i < NUM_OF_ANSWERS; i++) {
    receiveAndVerify(socket_fd, buffer);
    text_surface = TTF_RenderText_Blended(font, buffer, foreground_color);
    SDL_BlitSurface(text_surface, NULL, screen, &answer_location);
    answer_location.y += 50;
  }
  return false;
}

bool printGameMode(SDL_Surface *screen, SDL_Surface *background, TTF_Font *font,
                   TTF_Font *header_font, SDL_Surface *text_surface,
                   SDL_Rect mode_location, SDL_Color foreground_color) {
  SDL_Rect welcome_message = {128, 100, 228, 150};
  const char *game_options[] = {"Give me all you got!",
                                "Only one category please!", "Hotshot!"};
  SDL_BlitSurface(background, NULL, screen, &background_location);
  text_surface = TTF_RenderText_Blended(
      header_font, "Welcome to Connected Quiz!", foreground_color);
  SDL_BlitSurface(text_surface, NULL, screen, &welcome_message);
  for (int i = 0; i < NUM_OF_MODES; i++) {
    text_surface =
        TTF_RenderText_Blended(font, game_options[i], foreground_color);
    SDL_BlitSurface(text_surface, NULL, screen, &mode_location);
    mode_location.y += 50;
  }
  return false;
}

/*
 * Function: getPrintAnswer
 * ----------------------------
 *   Print message whether the answer is correct or not.
 *
 *   screen: pointer to SDL surface of the screen.
 *   font: pointer to TTF font used in rendering.
 *   text_surface: pointer to SDL surface of the text area.
 *   socket_fd: file descriptor for the open socket.
 *   score_location: coordinates of the score location in SDL_Rect.
 *   foreground_color: foreground color used in rendering.
 *   background_color: background color used in rendering.
 *
 */

bool getPrintAnswer(SDL_Surface *screen, TTF_Font *font,
                    SDL_Surface *text_surface, int socket_fd,
                    SDL_Rect score_location, SDL_Color foreground_color) {
  char buffer[MAX_RECEIVE_BUFFER];
  receiveAndVerify(socket_fd, buffer);
  text_surface = TTF_RenderText_Blended(font, buffer, foreground_color);
  SDL_BlitSurface(text_surface, NULL, screen, &score_location);
  return false;
}

void getScore(int socket_fd, char *score) {
  receiveAndVerify(socket_fd, score);
}

void printUserScore(SDL_Surface *screen, TTF_Font *font,
                    SDL_Surface *text_surface, SDL_Color foreground_color,
                    char *score) {
  SDL_Rect score_location = {500, 10, 10, 20};
  text_surface = TTF_RenderText_Blended(font, score, foreground_color);
  SDL_BlitSurface(text_surface, NULL, screen, &score_location);
}

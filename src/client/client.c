#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "networking.h"
#include "printing.h"
#include "interaction.h"

const int PORT = 1337;
const int MAX_RECEIVE_BUFFER = 500;
const int NUM_OF_ANSWERS = 4;
const int NUM_OF_MODES = 3;
const size_t WINDOW_WIDTH = 640;
const size_t WINDOW_HEIGHT = 480;

SDL_Rect background_location = {0, 0, 640, 480};

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused))) {

  struct sockaddr_in destination;
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  memset(&destination, 0, sizeof(struct sockaddr_in));

  destination.sin_family = AF_INET;
  destination.sin_addr.s_addr = inet_addr("127.0.0.1");
  destination.sin_port = htons(PORT);

  if (connect(socket_fd, (struct sockaddr *)&destination, sizeof(struct sockaddr_in)) < 0) {
    perror("Error while connecting to server");
    return 1;
  }

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  if (TTF_Init() < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", TTF_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Surface *screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 0, SDL_HWSURFACE | SDL_DOUBLEBUF );

  if (screen == NULL) {
    printf("SDL Surface could not initialize! SDL_Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Surface *image = SDL_LoadBMP("client_assets/background.bmp");

  if (image == NULL) {
    printf("Unable to load bitmap! SDL_Error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Surface *background = SDL_DisplayFormat(image);
  SDL_FreeSurface(image);

  TTF_Font *font = TTF_OpenFont("client_assets/Hack-Regular.ttf", 19);

  if (font == NULL) {
    printf("TTF Font could not initialize! TTF_Error: %s\n", TTF_GetError());
    SDL_Quit();
    return 1;
  }

  TTF_Font *header_font = TTF_OpenFont("client_assets/Hack-Bold.ttf", 21);

  if (header_font == NULL) {
    printf("TTF Font could not initialize! TTF_Error: %s\n", TTF_GetError());
    SDL_Quit();
    return 1;
  }

  TTF_SetFontKerning(font, 1);
  TTF_SetFontKerning(header_font, 1);

  SDL_WM_SetCaption("Connected Quiz", 0);

  SDL_Surface *text_surface = NULL;
  SDL_Color foreground_color = {0, 0, 0, 0};
  SDL_Rect text_location = {108, 100, 200, 50};
  SDL_Rect mode_location = {108, 150, 200, 50};
  SDL_Rect answer_location = {108, 200, 200, 50};
  SDL_Rect score_location = {168, 150 + 4 * 50 + 50, 200, 50};
  SDL_Event event;
  bool running = true;
  bool game_mode = true;
  bool get_question = false;
  bool get_answer = false;
  bool sleep = false;
  int mouse_x = 0;
  int mouse_y = 0;
  char score[20] = "0\0";

  while (running) {
    printUserScore(screen, font, text_surface, foreground_color, score);
    if (game_mode) {
      printGameMode(screen, background, font, header_font, text_surface, mode_location, foreground_color);
    }
    if (get_question) {
      get_question = getPrintQuestion(screen, background, font, text_surface, socket_fd, text_location, answer_location, foreground_color);
    }
    if (get_answer) {
      get_answer = getPrintAnswer(screen, font, text_surface, socket_fd, score_location, foreground_color);
      getScore(socket_fd, score);
      get_question = true;
      sleep = true;
    }
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          running = false;
          break;
        case SDL_MOUSEMOTION:
          mouse_x = event.motion.x;
          mouse_y = event.motion.y;
          break;
        case SDL_MOUSEBUTTONDOWN:
          switch (event.button.button) {
            case SDL_BUTTON_LEFT:
              if (game_mode) {
                game_mode = chooseGameMode(socket_fd, mouse_x, mouse_y, mode_location);
                if (!game_mode) {
                  get_question = true;
                  game_mode = false;
                }
              }
              else {
                get_answer = sendClickedAnswer(socket_fd, mouse_x, mouse_y, answer_location);
              }
          }
      }
    }
    if (sleep) {
      SDL_Flip(screen);
      //SDL_Delay(1000); // This will make input stacked.
      sleep = false;
    }
    SDL_Flip(screen);
  }

  SDL_FreeSurface(background);
  SDL_FreeSurface(text_surface);
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();
  close(socket_fd);
  return 0;
}

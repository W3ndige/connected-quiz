#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

const int PORT = 1337;
const int MAX_RECEIVE_BUFFER = 500;
const size_t NUM_OF_ANSWERS = 4;
const size_t WINDOW_WIDTH = 640;
const size_t WINDOW_HEIGHT = 480;

void receiveAndVerify(int socket_fd, char *buffer) {
  int len = recv(socket_fd, buffer, MAX_RECEIVE_BUFFER, 0);
  buffer[len] = '\0';
  send(socket_fd, "OK", strlen("OK"), 0);
}

int main(int argc, char *argv[]) {

  char buffer[MAX_RECEIVE_BUFFER];
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

  TTF_Font *font = TTF_OpenFont("client_assets/arial.ttf", 16);

  if (font == NULL) {
    printf("TTF Font could not initialize! TTF_Error: %s\n", TTF_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Surface *text_surface;
  SDL_Color foreground_color = { 255, 255, 255 };
  SDL_Color background_color = { 0, 0, 255 };
  SDL_Rect text_location = { 0, 0, 200, 50};
  SDL_Rect answer_location = {0, 50, 200, 50};
  SDL_Rect score_location = {0, 4 * 50 + 50, 200, 50};
  SDL_Event event;
  bool running = true;
  bool get_question = true;
  bool get_answer = false;
  int mouse_x = 0;
  int mouse_y = 0;

  while (running) {
    if (SDL_PollEvent(&event)) {
      if (get_question) {
        SDL_FillRect(screen, NULL, 0x000000);
        receiveAndVerify(socket_fd, buffer);
        text_surface = TTF_RenderText_Shaded(font, buffer, foreground_color, background_color);
        SDL_BlitSurface(text_surface, NULL, screen, &text_location);
        for (size_t i = 0; i < NUM_OF_ANSWERS; i++) {
          receiveAndVerify(socket_fd, buffer);
          text_surface = TTF_RenderText_Shaded(font, buffer, foreground_color, background_color);
          SDL_BlitSurface(text_surface, NULL, screen, &answer_location);
          answer_location.y += 50;
        }
        answer_location.y = 50;
        get_question = false;
      }

      if (get_answer) {
        receiveAndVerify(socket_fd, buffer);
        text_surface = TTF_RenderText_Shaded(font, buffer, foreground_color, background_color);
        SDL_BlitSurface(text_surface, NULL, screen, &score_location);
        get_answer = false;
        get_question = true;
      }

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
              if (mouse_x > answer_location.x && mouse_x < (answer_location.x + answer_location.w) && mouse_y > answer_location.y && mouse_y < (answer_location.y + answer_location.h)) {
                send(socket_fd, "A", strlen("A"), 0);
                get_answer = true;
              }
              else if (mouse_x > answer_location.x && mouse_x < (answer_location.x + answer_location.w) && mouse_y > answer_location.y + 50 && mouse_y < (answer_location.y + 50 + answer_location.h)) {
                send(socket_fd, "B", strlen("B"), 0);
                get_answer = true;
              }
              else if (mouse_x > answer_location.x && mouse_x < (answer_location.x + answer_location.w) && mouse_y > answer_location.y + 100 && mouse_y < (answer_location.y + 100 + answer_location.h)) {
                send(socket_fd, "C", strlen("C"), 0);
                get_answer = true;
              }
              else if (mouse_x > answer_location.x && mouse_x < (answer_location.x + answer_location.w) && mouse_y > answer_location.y + 150 && mouse_y < (answer_location.y + 150 + answer_location.h)) {
                send(socket_fd, "D", strlen("D"), 0);
                get_answer = true;
              }
          }
      }
    }
    SDL_Flip(screen);
  }

  SDL_FreeSurface(text_surface);
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();
  close(socket_fd);
  return 0;
}

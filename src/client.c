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

bool getPrintQuestion(SDL_Surface *screen, TTF_Font *font, SDL_Surface *text_surface, int socket_fd, SDL_Rect text_location, SDL_Rect answer_location, SDL_Color foreground_color, SDL_Color background_color) {
  char buffer[MAX_RECEIVE_BUFFER];
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
  return false;
}

bool getPrintAnswer(SDL_Surface *screen, TTF_Font *font, SDL_Surface *text_surface, int socket_fd, SDL_Rect score_location, SDL_Color foreground_color, SDL_Color background_color) {
  char buffer[MAX_RECEIVE_BUFFER];
  receiveAndVerify(socket_fd, buffer);
  text_surface = TTF_RenderText_Shaded(font, buffer, foreground_color, background_color);
  SDL_BlitSurface(text_surface, NULL, screen, &score_location);
  return false;
}

bool verifySendAnswers(int socket_fd, int mouse_x, int mouse_y, SDL_Rect answer_location) {
  char answer[2] = "\0";
  for (int i = 0; i < NUM_OF_ANSWERS; i++) {
    if (mouse_x > answer_location.x && mouse_x < (answer_location.x + answer_location.w) && mouse_y > (answer_location.y + 50 * i) && mouse_y < ((answer_location.y + 50 * i) + answer_location.h)) {
      answer[0] = 'A' + i;
      send(socket_fd, answer, strlen(answer), 0);
      return true;
    }
  }

  return false;
}

int main(int argc, char *argv[]) {

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

  SDL_Surface *text_surface = NULL;
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
        get_question = getPrintQuestion(screen, font, text_surface, socket_fd, text_location, answer_location, foreground_color, background_color);
      }

      if (get_answer) {
        get_answer = getPrintAnswer(screen, font, text_surface, socket_fd, score_location, foreground_color, background_color);
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
              get_answer = verifySendAnswers(socket_fd, mouse_x, mouse_y, answer_location);
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

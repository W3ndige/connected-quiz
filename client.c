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
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

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

  SDL_Surface *textSurface;
  SDL_Color foreground_color = { 255, 255, 255 };
  SDL_Color background_color = { 0, 0, 255 };
  SDL_Rect text_location = { 0, 0, 200, 50};
  SDL_Rect answer_location = {0, 50, 200, 50};
  SDL_Event event;
  bool running = true;

  while (running) {
    receiveAndVerify(socket_fd, buffer);
    textSurface = TTF_RenderText_Shaded(font, buffer, foreground_color, background_color);
    SDL_BlitSurface(textSurface, NULL, screen, &text_location);
    for (int i  = 0; i < 4; i++) {
      receiveAndVerify(socket_fd, buffer);
      textSurface = TTF_RenderText_Shaded(font, buffer, foreground_color, background_color);
      answer_location.y += 50;
      SDL_BlitSurface(textSurface, NULL, screen, &answer_location);
    }
    if (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          running = false;
          break;
        default:
          //printf("Enter your response:\n");
          //fgets(buffer, MAX_RECEIVE_BUFFER, stdin);
          //send(socket_fd, buffer, strlen(buffer), 0);

          //receiveAndVerify(socket_fd, buffer);
          //printf("%s\n", buffer);

          SDL_Flip(screen);

      }
    }
  }

  SDL_FreeSurface(textSurface);
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();
  close(socket_fd);
  return 0;
}

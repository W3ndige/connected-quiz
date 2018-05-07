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
const int NUM_OF_ANSWERS = 4;
const size_t WINDOW_WIDTH = 640;
const size_t WINDOW_HEIGHT = 480;

/*
 * Function: receiveAndVerify
 * ----------------------------
 *   Receive the data from the client and resend confirmation `OK`.
 *
 *   socket_fd: file descriptor for the open socket.
 *   buffer: pointer to buffer variable.
 *
 */

int receiveAndVerify(int client_fd, char *buffer) {
  int len = recv(client_fd, buffer, MAX_RECEIVE_BUFFER,0);
  if (len == -1) {
    perror("Error receiving data");
    return 0;
  }
  buffer[len] = '\0';
  send(client_fd, "OK", strlen("OK"), 0);
  return 1;
}

/*
 * Function: sendAndValidate
 * ----------------------------
 *   Sends the data and waits for verification from the connected client.
 *   Returns 0 on failure and 1 on success.
 *
 *   client_fd: file descriptor of client that got accepted.
 *   destination: structure that contains information about client.
 *   message: pointer to a message to be send.
 *
 */

int sendAndValidate(int client_fd, char *message) {
  char confirmation_buffer[3];
  send(client_fd, message, strlen(message), 0);
  printf("Sending data: %s\nWaiting for confirmation...\n", message);
  if (receiveAndVerify(client_fd, confirmation_buffer)) {
    printf("Got confirmation: %s\n", confirmation_buffer);
    if (!strncmp(confirmation_buffer, "OK", 2)) {
      return 1;
    }
  }
  return 0;
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

bool getPrintQuestion(SDL_Surface *screen, TTF_Font *font, SDL_Surface *text_surface, int socket_fd, SDL_Rect text_location, SDL_Rect answer_location, SDL_Color foreground_color, SDL_Color background_color) {
  char buffer[MAX_RECEIVE_BUFFER];
  SDL_FillRect(screen, NULL, 0x000000);
  receiveAndVerify(socket_fd, buffer);
  text_surface = TTF_RenderText_Shaded(font, buffer, foreground_color, background_color);
  SDL_BlitSurface(text_surface, NULL, screen, &text_location);
  for (int i = 0; i < NUM_OF_ANSWERS; i++) {
    receiveAndVerify(socket_fd, buffer);
    text_surface = TTF_RenderText_Shaded(font, buffer, foreground_color, background_color);
    SDL_BlitSurface(text_surface, NULL, screen, &answer_location);
    answer_location.y += 50;
  }
  return false;
}

bool printGameMode(SDL_Surface *screen, TTF_Font *font, SDL_Surface *text_surface, SDL_Rect text_location, SDL_Color foreground_color, SDL_Color background_color) {
  const char *game_options[] = {"Give me all you got!", "Only one category please!"};
  SDL_FillRect(screen, NULL, 0x000000);
  for (size_t i = 0; i < 2; i++) {
    text_surface = TTF_RenderText_Shaded(font, game_options[i], foreground_color, background_color);
    SDL_BlitSurface(text_surface, NULL, screen, &text_location);
    text_location.y += 50;
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

bool getPrintAnswer(SDL_Surface *screen, TTF_Font *font, SDL_Surface *text_surface, int socket_fd, SDL_Rect score_location, SDL_Color foreground_color, SDL_Color background_color) {
  char buffer[MAX_RECEIVE_BUFFER];
  receiveAndVerify(socket_fd, buffer);
  text_surface = TTF_RenderText_Shaded(font, buffer, foreground_color, background_color);
  SDL_BlitSurface(text_surface, NULL, screen, &score_location);
  return false;
}

/*
 * Function: verifySendAnswers
 * ----------------------------
 *  Send the answer that user submitted.
 *
 *   socket_fd: file descriptor for the open socket.
 *   mouse_x: position of mouse in X axis.
 *   mouse_y: position of mouse in Y axis.
 *   answer_location: coordinates of the answers location in SDL_Rect.
 *
 */

bool verifySendAnswers(int socket_fd, int mouse_x, int mouse_y, SDL_Rect answer_location) {
  char answer[2] = "\0";
  for (int i = 0; i < NUM_OF_ANSWERS; i++) {
    if (mouse_x > answer_location.x &&
      mouse_x < (answer_location.x + answer_location.w) &&
      mouse_y > (answer_location.y + 50 * i) &&
      mouse_y < ((answer_location.y + 50 * i) + answer_location.h))
    {
      answer[0] = 'A' + i;
      send(socket_fd, answer, strlen(answer), 0);
      return true;
    }
  }

  return false;
}

bool chooseGameMode(int socket_fd, int mouse_x, int mouse_y, SDL_Rect text_location) {
  char choosen_mode[2] = "\0";
  for (int i = 0; i < 2; i++) {
    if (mouse_x > text_location.x &&
      mouse_x < (text_location.x + text_location.w) &&
      mouse_y > (text_location.y + 50 * i) &&
      mouse_y < ((text_location.y + 50 * i) + text_location.h))
    {
      choosen_mode[0] = '1' + i;
      send(socket_fd, choosen_mode, strlen(choosen_mode), 0);
      return false;
    }
  }
  return true;
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
  SDL_Color foreground_color = { 255, 255, 255, 255 };
  SDL_Color background_color = { 0, 0, 255, 255 };
  SDL_Rect text_location = { 0, 0, 200, 50};
  SDL_Rect answer_location = {0, 50, 200, 50};
  SDL_Rect score_location = {0, 4 * 50 + 50, 200, 50};
  SDL_Event event;
  bool running = true;
  bool game_mode = true;
  bool get_question = false;
  bool get_answer = false;
  int mouse_x = 0;
  int mouse_y = 0;


  while (running) {
    if (game_mode) {
      printGameMode(screen, font, text_surface, text_location, foreground_color, background_color);
    }
    if (get_question) {
      get_question = getPrintQuestion(screen, font, text_surface, socket_fd, text_location, answer_location, foreground_color, background_color);
    }
    if (get_answer) {
      get_answer = getPrintAnswer(screen, font, text_surface, socket_fd, score_location, foreground_color, background_color);
      get_question = true;
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
                game_mode = chooseGameMode(socket_fd, mouse_x, mouse_y, text_location);
                if (!game_mode) {
                  get_question = true;
                  game_mode = false;
                }
              }
              else {
                get_answer = verifySendAnswers(socket_fd, mouse_x, mouse_y, answer_location);
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

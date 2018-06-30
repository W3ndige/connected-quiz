#include "interaction.h"
#include "networking.h"

bool sendClickedAnswer(int socket_fd, int mouse_x, int mouse_y, SDL_Rect answer_location) {
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
  for (int i = 0; i < NUM_OF_MODES; i++) {
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

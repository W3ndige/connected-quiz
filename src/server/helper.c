#include "helper.h"

void properlyTerminateString(char *string) {
  for (size_t i = 0; i <  strlen(string); i++) {
    if (string[i] == '\n') {
      string[i] = '\0';
    }
  }
}

#include "helper.h"

/*
 * Function: properlyTerminateString
 * ----------------------------
 *   Replaces the newline character with end of string character.
 *
 *   string: pointer to a char array
 *
 */

void properlyTerminateString(char *string) {
  for (size_t i = 0; i <  strlen(string); i++) {
    if (string[i] == '\n') {
      string[i] = '\0';
    }
  }
}

#include "game.h"

/*
 * Function: askRandomQuestion
 * ----------------------------
 *   Gets the random question from the random category, sens it to the client
 *   following with the sending of possible answers. Correct answer is being
 *   saved in a buffer and compared with client answer.
 *   Returns -1 on error, 0 if anwer is incorrect and 1 if correct.
 *
 *   client_fd: file descriptor of client that got accepted.
 *   destination: structure that contains information about client.
 *   num_of_categories: number of categories in categories file.
 *
 */

int askRandomQuestion(int client_fd, struct sockaddr_in destination, int num_of_categories, int mode) {
  char category_source[50];
  int category_number;

  if (mode < 0) {
    category_number = getCategory(category_source, sizeof(category_source) / sizeof(category_source[0]), num_of_categories, -1);
  }
  else {
    category_number = getCategory(category_source, sizeof(category_source) / sizeof(category_source[0]), num_of_categories, mode);
  }

  // Firstly check if getCategory() function will run correctly
  if (category_number == -1) {
    return -1;
  }

  properlyTerminateString(category_source);
  printf("Choosen category: %s\n", category_source);
  FILE *questions_file = fopen(category_source, "r");

  if (!questions_file) {
    perror("Could not open category file");
    return -1;
  }

  int counted_line = 0;
  int rand_question = rand() % calculateNumberOfQuestions(category_source) + 1;
  char correct_answer[2];
  char line[MAX_QUESTION_LINE_SIZE];

  while (!feof(questions_file)) {
    fgets(line, MAX_QUESTION_LINE_SIZE, questions_file);

    if (!strncmp(line, "BEGIN_OF_QUESTION", 17)) {
      counted_line++;
    }

    if (counted_line == rand_question) {
      memset(line, 0, MAX_QUESTION_LINE_SIZE);
      fgets(line, MAX_QUESTION_LINE_SIZE, questions_file);
      properlyTerminateString(line);
      if (!sendAndValidate(client_fd, destination, line)) {
        fclose(questions_file);
        return -1;
      }

      fgets(line, MAX_QUESTION_LINE_SIZE, questions_file);
      for (size_t i = 0; i < NUM_OF_ANSWERS; i++) {
        char answer[MAX_ANSWER_LINE_SIZE];
        fgets(answer, MAX_ANSWER_LINE_SIZE, questions_file);
        properlyTerminateString(answer);
        if (!sendAndValidate(client_fd, destination, answer)) {
          fclose(questions_file);
          return -1;
        }
      }
      fgets(correct_answer, 2, questions_file);
      properlyTerminateString(correct_answer);
      break;
    }
  }
  fclose(questions_file);

  return handleClientAnswers(client_fd, destination, correct_answer);
}

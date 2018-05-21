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

int askQuestion(int client_fd, struct sockaddr_in destination, struct question_info questions[], int question_number) {
  sendAndValidate(client_fd, destination, questions[question_number].question);
  sendAndValidate(client_fd, destination, questions[question_number].answer_a);
  sendAndValidate(client_fd, destination, questions[question_number].answer_b);
  sendAndValidate(client_fd, destination, questions[question_number].answer_c);
  sendAndValidate(client_fd, destination, questions[question_number].answer_d);

  return handleClientAnswers(client_fd, destination, questions[question_number].correct_answer);
}

// https://benpfaff.org/writings/clc/shuffle.html
void shuffleQuestions(struct question_info questions[], int total_number_of_questions) {
  if (total_number_of_questions > 1) {
    for (int i = 0; i < total_number_of_questions - 1; i++) {
      int j = i + rand() / (RAND_MAX / (total_number_of_questions - i) + 1);
      struct question_info tmp = questions[j];
      questions[j] = questions[i];
      questions[i] = tmp;
    }
  }
}

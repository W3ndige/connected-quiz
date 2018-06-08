#include "game.h"

int askQuestion(int client_fd, struct sockaddr_in destination, struct question_info *questions, int question_number) {
  sendAndValidate(client_fd, destination, questions[question_number].question);
  sendAndValidate(client_fd, destination, questions[question_number].answer_a);
  sendAndValidate(client_fd, destination, questions[question_number].answer_b);
  sendAndValidate(client_fd, destination, questions[question_number].answer_c);
  sendAndValidate(client_fd, destination, questions[question_number].answer_d);

  return handleClientAnswers(client_fd, destination, questions[question_number].correct_answer);
}

// https://benpfaff.org/writings/clc/shuffle.html
void shuffleQuestions(struct question_info *questions, int total_number_of_questions) {
  if (total_number_of_questions > 1) {
    for (int i = 0; i < total_number_of_questions - 1; i++) {
      int j = i + rand() / (RAND_MAX / (total_number_of_questions - i) + 1);
      struct question_info tmp = questions[j];
      questions[j] = questions[i];
      questions[i] = tmp;
    }
  }
}

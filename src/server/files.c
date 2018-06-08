#include "files.h"

int calculateNumberOfCategories() {
  FILE *categories_file = fopen(CATEGORIES_FILENAME, "r");
  if (!categories_file) {
    perror("Could not open category file");
    return -1;
  }
  else {
    int num_of_categories = 0;
    while (!feof(categories_file)) {
      char line[MAX_CATEGORY_LINE_SIZE];
      fgets(line, MAX_CATEGORY_LINE_SIZE, categories_file);
      if (strncmp(line,"END_OF_LIST",11) == 0) {
        break;
      }
      else {
        num_of_categories++;
      }
    }
    fclose(categories_file);
    return num_of_categories - 1;
  }
}

int calculateNumberOfQuestions(char *filename) {
  FILE *questions_file = fopen(filename, "r");
  if (!questions_file) {
    perror("Could not open category file");
    puts(filename);
    return -1;
  }
  else {
    int num_of_questions = 0;
    while (!feof(questions_file)) {
      char line[MAX_QUESTION_LINE_SIZE];
      fgets(line, MAX_QUESTION_LINE_SIZE, questions_file);
      if (strncmp(line,"BEGIN_OF_QUESTION",17) == 0) {
        num_of_questions++;
      }
    }
    fclose(questions_file);
    return num_of_questions;
  }
}

int calculateTotalNumberOfQuestions() {
  int total_number_of_questions = 0;
  FILE *categories_file = fopen(CATEGORIES_FILENAME, "r");
  if (!categories_file) {
    fprintf(stderr, "Could not open %s: %s\n", CATEGORIES_FILENAME, strerror(errno));
    return -1;
  }
  char category_source[MAX_CATEGORY_LINE_SIZE];
  fgets(category_source, sizeof(category_source) / sizeof(category_source[0]), categories_file);
  while (!feof(categories_file) ) {
    fgets(category_source, sizeof(category_source) / sizeof(category_source[0]), categories_file);
    if (strncmp(category_source,"END_OF_LIST",11) == 0) {
      break;
    }
    properlyTerminateString(category_source);
    int number_of_questions = calculateNumberOfQuestions(category_source);
    if (number_of_questions > 0) {
      total_number_of_questions += number_of_questions;
    }
  }
  fclose(categories_file);
  return total_number_of_questions;
}

 int getCategory(char *category_source, size_t size, int category_number) {

   FILE *categories_file = fopen(CATEGORIES_FILENAME, "r");
   if (!categories_file) {
     fprintf(stderr, "Could not open %s: %s\n", CATEGORIES_FILENAME, strerror(errno));
     return -1;
   }
   int counted_line = 0;
   while (!feof(categories_file)) {
     fgets(category_source, size, categories_file);
     if (counted_line == category_number) {
       fgets(category_source, size, categories_file);
       break;
     }
      counted_line++;
    }
   fclose(categories_file);
   return counted_line;
 }

void populateQuestions(struct question_info *questions, size_t num_of_categories) {
  char category_source[50];
  int question_number = 0;
  for (size_t i = 0; i < num_of_categories; i++) {
    int category_number = getCategory(category_source, sizeof(category_source) / sizeof(category_source[0]), i);
    if (category_number == -1) {
      continue;
    }

    properlyTerminateString(category_source);
    FILE *questions_file = fopen(category_source, "r");

    if (!questions_file) {
      fprintf(stderr, "Could not open %s: %s\n", category_source, strerror(errno));
      continue;
    }

    char line[MAX_QUESTION_LINE_SIZE];
    int counted_line = 0;

    while (!feof(questions_file)) {
      fgets(line, MAX_QUESTION_LINE_SIZE, questions_file);

      if (!strncmp(line, "BEGIN_OF_QUESTION", 17)) {
        questions[question_number].category_number = i;
        fgets(line, MAX_QUESTION_LINE_SIZE, questions_file);
        properlyTerminateString(line);
        strncpy(questions[question_number].question, line, MAX_QUESTION_LINE_SIZE);

        char answer[MAX_ANSWER_LINE_SIZE];
        fgets(answer, MAX_ANSWER_LINE_SIZE, questions_file);
        properlyTerminateString(answer);
        strncpy(questions[question_number].answer_a, answer, MAX_ANSWER_LINE_SIZE);

        fgets(answer, MAX_ANSWER_LINE_SIZE, questions_file);
        properlyTerminateString(answer);
        strncpy(questions[question_number].answer_b, answer, MAX_ANSWER_LINE_SIZE);

        fgets(answer, MAX_ANSWER_LINE_SIZE, questions_file);
        properlyTerminateString(answer);
        strncpy(questions[question_number].answer_c, answer, MAX_ANSWER_LINE_SIZE);

        fgets(answer, MAX_ANSWER_LINE_SIZE, questions_file);
        properlyTerminateString(answer);
        strncpy(questions[question_number].answer_d, answer, MAX_ANSWER_LINE_SIZE);

        char correct_answer[2];
        fgets(correct_answer, 2, questions_file);
        properlyTerminateString(correct_answer);
        strncpy(questions[question_number].correct_answer, correct_answer, 2);

        counted_line++;
        question_number++;
      }
    }
    fclose(questions_file);


  }

}

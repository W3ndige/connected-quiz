#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include <errno.h>
#include "helper.h"

extern const size_t MAX_CATEGORY_LINE_SIZE;
extern const size_t MAX_QUESTION_LINE_SIZE;
extern const size_t MAX_ANSWER_LINE_SIZE;
extern const size_t MAX_NUMBER_OF_CONNECTIONS;
extern const char *CATEGORIES_FILENAME;

struct question_info {
  int category_number;
  char question[300];
  char answer_a[100];
  char answer_b[100];
  char answer_c[100];
  char answer_d[100];
  char correct_answer[2];
};

int calculateNumberOfCategories(const char *filename);
int calculateNumberOfQuestions(char *filename);
int calculateTotalNumberOfQuestions();
int getCategory(char *category_source, size_t size, int category_number);
void populateQuestions(struct question_info questions[], size_t num_of_categories);

#endif

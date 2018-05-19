#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include <errno.h>
#include "helper.h"

extern const size_t MAX_CATEGORY_LINE_SIZE;
extern const size_t MAX_QUESTION_LINE_SIZE;
extern const size_t MAX_NUMBER_OF_CONNECTIONS;
extern const char *CATEGORIES_FILENAME;

int calculateNumberOfCategories(const char *filename);
int calculateNumberOfQuestions(char *filename);
int calculateTotalNumberOfQuestions();
int getCategory(char *category_source, size_t size, size_t num_of_categories, int category_number);

#endif

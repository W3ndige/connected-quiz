#include "files.h"

/*
 * Function: calculateNumberOfCategories
 * ----------------------------
 *   Returns the number of categories in categories file.
 *   or -1 on error.
 *
 *   filename: filename of categories file.
 *
 */

int calculateNumberOfCategories(const char *filename) {
  FILE *categories_file = fopen(filename, "r");
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

/*
 * Function: calculateNumberOfQuestions
 * ----------------------------
 *   Returns the number of questions in question file,
 *   or -1 on error.
 *
 *   filename: filename of questions file.
 *
 */

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

/*
 * Function: getCategory
 * ----------------------------
 *   Chooses the random category and puts it's filename
 *   into the category_source. Returns -1 on error and
 *   line of choosen category.
 *
 *   category_source: pointer to which a choosen category will be saved.
 *   size: size of the buffer that fget will get from the line.
 *   num_of_categories: number of categories.
 *
 */

 int getCategory(char *category_source, size_t size, size_t num_of_categories, int category_number) {

   if (category_number < 0) {
     category_number = rand() % num_of_categories;
   }

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

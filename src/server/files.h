/**
 * @file
 * @date 29 May 2018
 * @brief File contains functions used in order to send question data and
 * manipulate questions array.
 *
 */

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

struct question_info { /**< Structure containing information about question. */
  int category_number;
  char question[300];
  char answer_a[100];
  char answer_b[100];
  char answer_c[100];
  char answer_d[100];
  char correct_answer[2];
};

/**
* @brief Calculate number of categories in categories file.
*
* @param filename String containing filename.
* @return Number of categories or -1 on error.
*/

int calculateNumberOfCategories();

/**
* @brief Calculate number of questions in specified file.
*
* @param filename String containing filename.
* @return Number of questions or -1 on error.
*/

int calculateNumberOfQuestions(char *filename);

/**
* @brief Calculate total number of questions in all categories.
*
* @param filename String containing filename.
* @return Total number of questions or -1 on error.
*/

int calculateTotalNumberOfQuestions();

 /**
 * @brief Chooses the random category and puts it's filename
 * into the category_source. Returns -1 on error and
 * line of choosen category.
 *
 * @param category_source Pointer to which a choosen category will be saved.
 * @param size Size of the buffer that fget will get from the line.
 * @param num_of_categories Number of categories.
 * @return Number of category or -1 on error.
 */

int getCategory(char *category_source, size_t size, int category_number);

/**
* @brief Fill the questions array with information about questions.
*
* @param questions Array of structs question_info that will be filled with question info.
* @param num_of_categories Number of categories.
*/

void populateQuestions(struct question_info *questions,
                       size_t num_of_categories);

#endif

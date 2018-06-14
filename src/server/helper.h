/**
 * @file
 * @date 29 May 2018
 * @brief File contains functions used to assist in different tasks.
 *
 */

#ifndef HELPER_H
#define HELPER_H

#include <stdlib.h>
#include <string.h>

/*
 * Function: properlyTerminateString
 * ----------------------------
 *   Replaces the newline character with end of string character.
 *
 *   string: pointer to a char array
 *
 */

 /**
 * @brief  Replaces the newline character with end of string character.
 *
 * @param[out] string: pointer to a char array
 */

void properlyTerminateString(char *string);

#endif

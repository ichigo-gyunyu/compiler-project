#ifndef UTILS_H
#define UTILS_H

// required standard library headers
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// get the size of an array (whose size is fixed at compile time)
#define NUM_ELEM(x) (sizeof(x) / sizeof((x)[0]))

/**
 * Error printing function
 * In case of a fatal error, print the error message and exit
 */
void exit_msg(char *msg);

/**
 * Allocates memory and copies the input string into it
 * Slightly inefficient, but helps avoiding double frees
 */
char *duplicate_str(char *str);

#endif

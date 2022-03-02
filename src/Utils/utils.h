
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#ifndef UTILS_H
#define UTILS_H

// required standard library headers
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// for printing colours
#define RED   "\x1B[31m"
#define YEL   "\x1B[33m"
#define GRN   "\x1B[32m"
#define BLU   "\x1B[34m"
#define RESET "\x1B[0m"

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


/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#ifndef STACK_H
#define STACK_H

#include "utils.h"

typedef struct StackElement {
    int val;
    int type;

    struct StackElement *next;
} StackElement;

typedef StackElement *Stack;

// typical stack functionality
bool          st_isEmpty(Stack root);
void          st_push(Stack *root, int val, int type);
int           st_pop(Stack *root);
StackElement *st_top(Stack *root);
void          st_free(Stack *root); // cleanup

#endif

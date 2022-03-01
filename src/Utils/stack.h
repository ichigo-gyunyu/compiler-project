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

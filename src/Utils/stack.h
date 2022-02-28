#ifndef STACK_H
#define STACK_H

#include "utils.h"

typedef struct StackElement {
    int val;
    int type;

    struct StackElement *next;
} StackElement;
typedef StackElement *stack;

bool st_isEmpty(StackElement *root);

void st_push(StackElement **root, int val, int type);

int st_pop(StackElement **root);

StackElement *st_top(StackElement **root);

#endif

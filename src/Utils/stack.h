
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

/**
 * Generic Stack ADT
 *
 * (type unsafe!)
 *
 */

#include "utils.h"

typedef struct StackNode {
    struct StackNode *next;
    void             *data;
} StackNode;

typedef struct Stack {
    size_t     width; // width of the stack element data type
    uint       num_elem;
    StackNode *top;

    void (*copy_ctr)(void *dest, const void *src); // copy constructor (for deep copy)
    void (*dtr)(void *p);                          // destructor (for deep free)
} Stack;

/**
 * Initialize the stack
 * Width is the size of the stack element data type.
 * User can provide an explicit copy constructor and destructor, or pass NULL
 */
Stack *st_init(size_t width, void (*copy_ctr)(void *dest, const void *src), void (*dtr)(void *p));

/**
 * Push an element onto the stack
 */
void st_push(Stack *s, const void *data);

/**
 * Pop and free the top of the stack
 * Does nothing if the stack is empty
 */
void st_pop(Stack *s);

/**
 * Returns a pointer to the data on the top of the stack
 * Returns NULL if empty
 */
void *st_top(const Stack *s);

/**
 * Free any allocations on the stack and then free the stack itself
 * using the destructor if provided
 */
void st_free(Stack *s);

/**
 * Checks if the stack is empty
 */
bool st_isempty(const Stack *s);

#endif

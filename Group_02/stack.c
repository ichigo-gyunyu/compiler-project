
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "stack.h"

StackElement *newElement(int val, int type) {
    StackElement *element = malloc(sizeof(*element));
    element->val          = val;
    element->type         = type;
    element->next         = NULL;

    return element;
}

bool st_isEmpty(StackElement *root) { return !root; }

void st_push(Stack *root, int val, int type) {
    StackElement *element = newElement(val, type);
    element->next         = *root;
    *root                 = element;
}

int st_pop(Stack *root) {
    if (st_isEmpty(*root))
        return -1;

    StackElement *temp = *root;
    *root              = temp->next;

    free(temp);
    return 1;
}

StackElement *st_top(Stack *root) {
    if (!(*root))
        return NULL;

    return *root;
}

void st_free(Stack *root) {
    if (*root && (*root)->next)
        st_free(&(*root)->next);
    if (*root)
        free(*root);
}

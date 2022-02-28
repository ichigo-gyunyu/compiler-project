#include "stack.h"

StackElement *newElement(int val, int type) {
    StackElement *element = malloc(sizeof(*element));
    element->val          = val;
    element->type         = type;
    element->next         = NULL;

    return element;
}

bool st_isEmpty(StackElement *root) { return !root; }

void st_push(StackElement **root, int val, int type) {
    StackElement *element = newElement(val, type);
    element->next         = *root;
    *root                 = element;
}

int st_pop(StackElement **root) {
    if (st_isEmpty(*root))
        return -1;

    StackElement *temp = *root;
    *root              = temp->next;

    free(temp);
    return 1;
}

StackElement *st_top(StackElement **root) {
    if (!(*root))
        return NULL;

    return *root;
}

void st_free(stack *root) {
    if (*root && (*root)->next)
        st_free(&(*root)->next);
    if (*root)
        free(*root);
}

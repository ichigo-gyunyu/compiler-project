#include "stack.h"

StackElement *newElement(int val, int type){
    StackElement *element = malloc(sizeof(*element));
    element->val = val;
    element->type = type;
    element->next = NULL;
}

bool isEmpty(StackElement *root){
    return !root;
}

void push(StackElement **root, int val, int type){
    StackElement *element = newElement(val, type);
    element->next = *root;
    *root = element;
}

int pop(StackElement **root){
    if(isEmpty(*root))
        return -1;
    
    StackElement *temp = *root;
    *root = temp->next;

    free(temp);

}

StackElement *top(StackElement **root){
    if(!(*root))
        return NULL;

    return *root;
}

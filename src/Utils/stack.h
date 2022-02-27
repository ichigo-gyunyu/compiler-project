#include "utils.h"

typedef struct StackElement{
    int val;
    int type;
    struct StackElement *next;
}StackElement;
typedef StackElement* stack;

StackElement *newElement(int val, int type);

bool isEmpty(StackElement *root);

void push(StackElement **root, int val, int type);

int pop(StackElement **root);

StackElement *top(StackElement **root);


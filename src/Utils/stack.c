
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "stack.h"

Stack *st_init(size_t width, void (*copy_ctr)(void *dest, const void *src), void (*dtr)(void *p)) {
    Stack *s = malloc(sizeof *s);

    *s = (Stack){
        .width    = width,
        .num_elem = 0,
        .top      = NULL,
        .copy_ctr = copy_ctr,
        .dtr      = dtr,
    };

    return s;
}

void st_push(Stack *s, const void *data) {

    StackNode *new_top = malloc(sizeof *new_top);

    *new_top = (StackNode){
        .next = NULL,
        .data = malloc(s->width),
    };

    if (s->copy_ctr)
        s->copy_ctr(new_top->data, data);
    else
        memcpy(new_top->data, data, s->width);

    new_top->next = s->top;
    s->top        = new_top;
    s->num_elem++;
}

void st_pop(Stack *s) {

    if (s->num_elem == 0)
        return;

    s->num_elem--;
    StackNode *tmp = s->top;
    s->top         = s->top->next;

    if (s->dtr)
        s->dtr(tmp->data);
    else
        free(tmp->data);
    free(tmp);
}

void *st_top(const Stack *s) {

    // empty
    if (s->num_elem == 0)
        return NULL;

    return s->top->data;
}

void st_free(Stack *s) {
    for (StackNode *n = s->top; n != NULL;) {
        StackNode *tmp = n;
        n              = n->next;

        if (s->dtr)
            s->dtr(tmp->data);
        else
            free(tmp->data);
        free(tmp);
    }

    free(s);
}

bool st_isempty(const Stack *s) { return (s->num_elem == 0); }

/* void cpy(char **dest, const char **src) { *dest = strdup(*src); }
void dtr(char **p) {
    free(*p);
    free(p);
}

int main() {

    // testing
    int    a[] = {3, 4, 5, 6};
    Stack *s1  = st_init(sizeof(int), NULL, NULL);
    for (int i = 0; i < 4; i++) {
        st_push(s1, a + i);
    }
    int *top = (int *)st_top(s1);
    printf("Actual - %d. Expected - %d\n", *top, 6);
    free(top);
    st_pop(s1);
    st_pop(s1);
    top = (int *)st_top(s1);
    printf("Actual - %d. Expected - %d\n", *top, 4);
    free(top);
    st_free(s1);

    Stack *s2 = st_init(sizeof(char *), (void (*)(void *, const void *))cpy, (void (*)(void *))dtr);
    char  *w1 = strdup("one");
    char  *w2 = strdup("two");
    char  *w3 = strdup("three");
    st_push(s2, &w1);
    char **top2 = (char **)st_top(s2);
    printf("Actual - %s. Expected - %s\nReturned ptr %p ; Alloc ptr %p\n", *top2, "one", *top2, w1);
    free(top2);
    st_pop(s2);
    st_pop(s2);
    st_pop(s2);
    st_push(s2, &w1);
    free(w1);
    st_push(s2, &w2);
    free(w2);
    top2 = (char **)st_top(s2);
    printf("Actual - %s. Expected - %s\nReturned ptr %p ; Alloc ptr %p\n", *top2, "two", *top2, w1);
    free(top2);
    st_push(s2, &w3);
    free(w3);
    top2 = (char **)st_top(s2);
    printf("Actual - %s. Expected - %s\nReturned ptr %p ; Alloc ptr %p\n", *top2, "three", *top2, w1);
    free(top2);
    st_free(s2);

    return 0;
} */

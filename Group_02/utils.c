
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "utils.h"

void exit_msg(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

char *duplicate_str(char *str) {
    char *tmp = calloc(strlen(str) + 1, sizeof *tmp);
    memcpy(tmp, str, strlen(str) + 1);

    return tmp;
}

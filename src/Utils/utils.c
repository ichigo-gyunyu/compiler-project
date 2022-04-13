
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

void exit_perror(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

char *str_dup(const char *str) {
    char *tmp = calloc(strlen(str) + 1, sizeof *tmp);
    memcpy(tmp, str, strlen(str) + 1);

    return tmp;
}

void str_cpyctr(char **dest, const char **src) { *dest = strdup(*src); }

void str_dtr(char **p) { free(*p); }

bool str_equal(const char **s1, const char **s2) { return !(strcmp(*s1, *s2)); }

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

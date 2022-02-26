#include <stdio.h>

#include "Lexer/lexer.h"

int main(int argc, char **argv) {
    FILE *fp = fopen("data/a.txt", "r");
    if (fp == NULL) {
        printf("noooo\n");
        return 1;
    }

    initLexer(fp);
    l_test();

    fclose(fp);

    /* while (1) {
        char ch = fgetc(fp);
        if (ch == '\n') {
            printf("newline %d\n", ch);
            fseek(fp, 1, SEEK_CUR);
        } else
            printf("%c ascii: %d\n", ch, ch);
        if (ch == EOF) {
            printf("hmm %d\n", ch);
            break;
        }
    } */
}

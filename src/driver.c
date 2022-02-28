#include <stdio.h>

#include "Lexer/lexer.h"
#include "Parser/parser.h"
#include "Utils/twinbuffer.h"

#define GRAMMAR_FILE "data/grammar.txt"

int main(int argc, char **argv) {

    FILE *fp = fopen("data/a.txt", "r");
    /* for (;;) {
        char c = fgetc(fp);
        printf("%c - %d\n", c, c);
        if (feof(fp))
            break;
    } */

    TwinBuffer tb = initLexer(fp);
    TokenInfo  t  = getNextToken(&tb);
    while (t.tk_type != TK_EOF) {
        /* if (t.tk_type != END_TOKENTYPE)
            printTokenInfo(t); */
        freeToken(&t);
        t = getNextToken(&tb);
    }
    if (t.tk_type == TK_EOF)
        printf("done\n");
    freeToken(&t);
    freeTwinBuffer(&tb);
    fclose(fp);

    // parseInputSourceCode("data/a.txt");

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

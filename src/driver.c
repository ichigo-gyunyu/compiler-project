#include <stdio.h>

#include "Lexer/lexer.h"
#include "Parser/parser.h"
#include "Utils/twinbuffer.h"

#define GRAMMAR_FILE "data/grammar.txt"

int main(int argc, char **argv) {

    static char *tc[] = {
        "testcases/t1.txt", "testcases/t2.txt", "testcases/t3.txt", "testcases/t4.txt",
        "testcases/t5.txt", "testcases/t6.txt", "testcases/t7.txt",
    };

    /* for (int i = 5; i < 6; i++) {
        FILE       *fp = fopen(tc[i], "r");
        TwinBuffer *tb = initLexer(&fp);
        TokenInfo   t  = getNextToken(tb);
        while (t.tk_type != TK_EOF) {
            if (t.tk_type != END_TOKENTYPE)
                printTokenInfo(t);
            freeToken(&t);
            t = getNextToken(tb);
        }
        freeToken(&t);
        freeTwinBuffer(tb);
        printf("\n----------------------------\n");
    } */

    for (int i = 0; i < 7; i++) {
        parseInputSourceCode(tc[i]);
        printf("\n----------------------------\n");
    }

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

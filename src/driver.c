#include <stdio.h>

#include "Lexer/lexer.h"
#include "Parser/parser.h"

#define GRAMMAR_FILE "data/grammar.txt"

int main(int argc, char **argv) {
    /* FILE *fp = fopen("data/a.txt", "r");
    if (fp == NULL) {
        printf("noooo\n");
        return 1;
    }

    initLexer(fp);
    l_test();

    fclose(fp); */

    /* bitvector bv;
    bv_init(&bv, NONTERMINAL_COUNT);
    bv_set(bv, 0);
    bv_set(bv, 2);
    bv_set(bv, 12);

    bitvector bv2;
    bv_init(&bv2, NONTERMINAL_COUNT);
    bv_set(bv2, 0);
    bv_set(bv2, 12);
    bv_set(bv2, 14);
    bv_union(bv, bv2, NONTERMINAL_COUNT);

    for (int i = 0; i < NONTERMINAL_COUNT; i++) {
        if (bv_contains(bv, i)) {
            printf("At %d\n", i);
        }
    }

    free(bv);
    free(bv2); */

    Grammar g = initParser(GRAMMAR_FILE);
    printGrammar(g);

    FirstAndFollow *fnf = computeFirstAndFollow(g);
    printFirstAndFollow(fnf);

    ParseTable pt = createParseTable(fnf);
    printParseTable(pt);
    //
    freeParserData(&g, fnf, pt);

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

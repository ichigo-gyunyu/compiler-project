#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

/**
 * Takes a pointer to the source file
 * Initializes the twin buffer
 * Sets the begin and lookahead pointers
 */
void initLexer(FILE *src_ptr);

TokenInfo getNextToken(int *status);

void removeComments(char *testcaseFile, char *cleanFile);

#endif

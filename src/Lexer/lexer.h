#ifndef LEXER_H
#define LEXER_H

#include "Utils/hashtable.h"
#include "lexerDef.h"

/**
 * Takes a pointer to the source file
 * Initializes the twin buffer
 * Sets the begin and lookahead pointers
 */
void initLexer(FILE *src_ptr);

TokenInfo getNextToken();

void removeComments(char *testcaseFile, char *cleanFile);

char *getTokenTypeName(TokenType tk);

extern hashtable lookup_table;

#endif

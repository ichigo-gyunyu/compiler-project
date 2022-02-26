#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

/**
 * Takes a pointer to the source file
 * Initializes twin buffers and other data structures
 * required by the lexical analyzer
 */
void initLexer(FILE *src_ptr);

TokenInfo nextToken();
void      removeComments(char *testcaseFile, char *cleanFile);
void      l_test();

#endif

#ifndef LEXER_H
#define LEXER_H

#include "Utils/hashtable.h"
#include "Utils/twinbuffer.h"
#include "lexerDef.h"

/**
 * Takes a pointer to the source file
 * Initializes the twin buffer
 * Sets the begin and lookahead pointers
 */
TwinBuffer initLexer(FILE *src_ptr);

TokenInfo getNextToken(TwinBuffer *tb);

void removeComments(char *testcaseFile, char *cleanFile);

char *getTokenTypeName(TokenType tk);

void printTokenInfo(TokenInfo t);

void freeToken(TokenInfo *t);

void freeTwinBuffer(TwinBuffer *tb);

extern hashtable lookup_table;

#endif

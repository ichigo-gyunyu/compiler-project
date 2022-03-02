
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#ifndef LEXER_H
#define LEXER_H

#include "Utils/hashtable.h"
#include "Utils/twinbuffer.h"
#include "lexerDef.h"

/**
 * Takes a pointer to the source file
 * Initializes the twin buffer
 * Sets the begin and lookahead pointers
 * Initializes starting character hashtable
 */
TwinBuffer *initLexer(FILE **src_ptr);

/**
 * Reads the source file using a twin buffering
 * mechanism and simulates the DFA to get token and lexeme
 * Also reports lexical errors
 */
TokenInfo getNextToken(TwinBuffer *tb);

/**
 * Removes comments from testcaseFile
 * and saves the output in cleanFile.
 * This is only to showcase comment removal and is not
 * used directly by the lexical analyzer
 */
void removeComments(char *testcaseFile, char *cleanFile);

/**
 * Get string from the enumerated token type value
 */
char *getTokenTypeName(TokenType tk);

/**
 * Run the lexical analyzer module on the input source code
 * Detect errors
 * Free up memory
 */
void runLexerOnInputSourceCode(char *testcaseFile);

/**
 * Print out all the information in a toke
 */
void printTokenInfo(TokenInfo t);

/**
 * Memory cleanup functions
 */
void freeToken(TokenInfo *t);
void freeTwinBuffer(TwinBuffer *tb);

// might be required in later stages
extern Hashtable lookup_table;

#endif

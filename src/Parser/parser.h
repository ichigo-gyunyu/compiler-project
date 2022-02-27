#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

// calcualte first and follow
// parse table related functions

Grammar initParser(char *grammarFile);

FirstAndFollow *computeFirstAndFollow(Grammar g);

void freeParserData(Grammar *g, FirstAndFollow *fnf, ParseTable pt);

void printGrammar(Grammar g);

void printFirstAndFollow(FirstAndFollow *fnf);

ParseTable createParseTable(FirstAndFollow *fnf);

void printParseTable(ParseTable pt);

extern hashtable NtToEnum;
extern hashtable TkToEnum;

#endif

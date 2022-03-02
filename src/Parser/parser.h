#ifndef PARSER_H
#define PARSER_H

#include "Utils/nary.h"
#include "parserDef.h"

/**
 * Loads the grammar from the grammar file into
 * respective data structures.
 * Initializes hashtables for terminals and nonterminals
 */
Grammar initParser(char *grammarFile);

/**
 * Recursively computes first sets
 * Iteratively computes follow sets
 * Implementation details are documented in the source
 */
FirstAndFollow *computeFirstAndFollow(Grammar g);

/**
 * Create the parese table form the first
 * and follow sets
 */
ParseTable createParseTable(FirstAndFollow *fnf);

/**
 * Parse the input source code using bottom up parsing
 * Reports and recovers from syntax errors
 */
Nary_tree parseInputSourceCode(char *testcaseFile);

/**
 * Printing functions for logging
 */
void printGrammar(Grammar g);
void printFirstAndFollow(FirstAndFollow *fnf);
void printParseTable(ParseTable pt);
Nary_tree printParseTree(Nary_tree pt, char *outfile);

/**
 * Memory cleanup functions
 */
void freeParserData(Grammar *g, FirstAndFollow *fnf, ParseTable pt);

// might be required in other stages
extern Hashtable NtToEnum;
extern Hashtable TkToEnum;

#endif

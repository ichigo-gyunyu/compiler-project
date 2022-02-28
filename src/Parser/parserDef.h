#ifndef PARSERDEF_H
#define PARSERDEF_H

// TODO
// grammar - terminals, non terminals?
// production rules
// first and follow sets - bitvectors
// tree, parse table

#include "Lexer/lexer.h"
#include "Utils/bitvector.h"
#include "Utils/hashtable.h"

#define TYPE_TK 1
#define TYPE_NT 2

typedef enum Nonterminals {
    actualOrRedefined,
    arithmeticExpression,
    assignmentStmt,
    booleanExpression,
    conditionalStmt,
    constructedDatatype,
    dataType,
    declaration,
    declarations,
    definetypestmt,
    elsePart,
    expPrime,
    factor,
    fieldDefinition,
    fieldDefinitions,
    fieldType,
    funCallStmt,
    function,
    global_or_not,
    highPrecedenceOperators,
    idList,
    input_par,
    inputParameters,
    ioStmt,
    iterativeStmt,
    logicalOp,
    lowPrecedenceOperators,
    mainFunction,
    moreExpansions,
    moreFields,
    more_ids,
    oneExpansion,
    optionalReturn,
    option_single_constructed,
    otherFunctions,
    otherStmts,
    output_par,
    outputParameters,
    parameter_list,
    primitiveDatatype,
    program,
    recOrUnion,
    relationalOp,
    remaining_list,
    returnStmt,
    singleOrRecId,
    stmt,
    stmts,
    term,
    termPrime,
    typeDefinition,
    typeDefinitions,
    var,

    END_NT
} NonTerminal;

static const uint NONTERMINAL_COUNT = END_NT - actualOrRedefined;

typedef struct FirstAndFollow {
    bitvector first;
    bitvector follow;
    bool      has_eps; // if first contains eps
} FirstAndFollow;

typedef union TorNT {
    TokenType   val_tk;
    NonTerminal val_nt;
} TorNT;

typedef struct SymbolNode {
    TorNT val;
    int   type; // Terminal or NonTerminal

    struct SymbolNode *prev;
    struct SymbolNode *next;
} SymbolNode;

// TODO consider making a vector ds
// linked lists are slow
typedef struct ProductionRule {
    SymbolNode *head;
    SymbolNode *tail;
    uint        rule_length;
} ProductionRule;

typedef struct Derivation {
    NonTerminal     lhs; // lhs
    uint            num_rhs;
    ProductionRule *rhs;
} Derivation;

typedef struct Grammar {
    uint        num_nonterminals;
    NonTerminal start_symbol;
    Derivation *derivations;
} Grammar;

typedef struct ParseTableInfo {
    bool           filled;
    ProductionRule rule;
} ParseTableInfo;

typedef ParseTableInfo **ParseTable;

#endif

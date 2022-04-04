
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "Lexer/lexer.h"
#include "Utils/bitvector.h"
#include "Utils/hashtable.h"
#include "Utils/vector.h"

#define TYPE_TK 1 // terminal symbol
#define TYPE_NT 2 // nonterminal symbol

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
    Bitvector first;
    Bitvector follow;
    bool      has_eps; // if first contains eps
} FirstAndFollow;

// terminal or nonterminal
typedef union TorNT {
    NonTerminal val_nt;
    TokenType   val_tk;
} TorNT;

// terminal or nonterminal in the RHS of a production rule
typedef struct SymbolNode {
    TorNT val;
    int   type; // Terminal or NonTerminal
} SymbolNode;

// list of symbols (vector)
typedef struct ProductionRule {
    Vector *symbols;
    uint8_t rule_length;
    uint8_t rule_no; // according to grammar.txt
} ProductionRule;

typedef struct Derivation {
    NonTerminal     lhs; // lhs
    uint8_t         num_rhs;
    ProductionRule *rhs; // lhs->rhs1, lhs->rhs2...
} Derivation;

typedef struct Grammar {
    uint8_t     num_nonterminals;
    NonTerminal start_symbol;
    Derivation *derivations; // lhs1->... lhs2->...
} Grammar;

typedef struct ParseTableInfo {
    bool           filled;
    bool           syn;
    ProductionRule rule;
} ParseTableInfo;

typedef ParseTableInfo **ParseTable;

#endif

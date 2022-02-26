#ifndef PARSERDEF_H
#define PARSERDEF_H

// TODO
// grammar - terminals, non terminals?
// production rules
// first and follow sets - bitvectors
// tree, parse table

#include "Utils/bitvector.h"

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
    eps,
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
} Nonterminals;

static const unsigned int NONTERMINAL_COUNT = END_NT - actualOrRedefined;

typedef struct FirstAndFollow {
    bitvector first;
    bitvector follow;
} FirstAndFollow;

#endif

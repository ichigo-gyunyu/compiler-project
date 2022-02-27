#include "hashtable.h"

// Computes the hash value based on d.key
uint ht_hash(data d) {}

int ht_init(hashtable *ht, uint sz) {}

void ht_print(hashtable ht) {}

int ht_insert(hashtable ht, data d) {}

int ht_lookup(hashtable ht, char *key) {
    // TODO: remove hardcoding later
    const char *nts[] = {
        "actualOrRedefined",
        "arithmeticExpression",
        "assignmentStmt",
        "booleanExpression",
        "conditionalStmt",
        "constructedDatatype",
        "dataType",
        "declaration",
        "declarations",
        "definetypestmt",
        "elsePart",
        "expPrime",
        "factor",
        "fieldDefinition",
        "fieldDefinitions",
        "fieldType",
        "funCallStmt",
        "function",
        "global_or_not",
        "highPrecedenceOperators",
        "idList",
        "input_par",
        "inputParameters",
        "ioStmt",
        "iterativeStmt",
        "logicalOp",
        "lowPrecedenceOperators",
        "mainFunction",
        "moreExpansions",
        "moreFields",
        "more_ids",
        "oneExpansion",
        "optionalReturn",
        "option_single_constructed",
        "otherFunctions",
        "otherStmts",
        "output_par",
        "outputParameters",
        "parameter_list",
        "primitiveDatatype",
        "program",
        "recOrUnion",
        "relationalOp",
        "remaining_list",
        "returnStmt",
        "singleOrRecId",
        "stmt",
        "stmts",
        "term",
        "termPrime",
        "typeDefinition",
        "typeDefinitions",
        "var",
    };

    // printf("Looking for %s... \n", key);

    for (int i = 0; i < 53; i++) {
        // printf("%d:", i);
        // fflush(stdout);
        if (strcmp(key, nts[i]) == 0) {
            // printf("Returning %d\n", i);
            return i;
        }
    }
    return -1;
}

// TODO: remove once ht is done
int ht_lookup2(hashtable ht, char *key) {
    const char *ts[] = {"TK_ASSIGNOP", "TK_COMMENT",   "TK_FIELDID",    "TK_ID",         "TK_NUM",       "TK_RNUM",
                        "TK_FUNID",    "TK_RUID",      "TK_WITH",       "TK_PARAMETERS", "TK_END",       "TK_WHILE",
                        "TK_UNION",    "TK_ENDUNION",  "TK_DEFINETYPE", "TK_AS",         "TK_TYPE",      "TK_MAIN",
                        "TK_GLOBAL",   "TK_PARAMETER", "TK_LIST",       "TK_SQL",        "TK_SQR",       "TK_INPUT",
                        "TK_OUTPUT",   "TK_INT",       "TK_REAL",       "TK_COMMA",      "TK_SEM",       "TK_COLON",
                        "TK_DOT",      "TK_ENDWHILE",  "TK_OP",         "TK_CL",         "TK_IF",        "TK_THEN",
                        "TK_ENDIF",    "TK_READ",      "TK_WRITE",      "TK_RETURN",     "TK_PLUS",      "TK_MINUS",
                        "TK_MUL",      "TK_DIV",       "TK_CALL",       "TK_RECORD",     "TK_ENDRECORD", "TK_ELSE",
                        "TK_AND",      "TK_OR",        "TK_NOT",        "TK_LT",         "TK_LE",        "TK_EQ",
                        "TK_GT",       "TK_GE",        "TK_NE",         "TK_EOF"};

    for (int i = 0; i < 58; i++) {
        if (strcmp(key, ts[i]) == 0)
            return i;
    }
    return -1;
}

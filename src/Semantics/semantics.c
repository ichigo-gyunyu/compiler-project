
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "Semantics/semantics.h"

static SymbolTable *st_global;
static Hashtable   *assignments; // <char *, astID *>
static Hashtable   *functions;   // <char *, astFunction *>

void rule3x5x(const astFunction *fn);
void rule8x(const astFunction *fn);
void rule3(const astID *id);
void rule5(const astID *id);
void rule8(const Vector *stmts);
void rule12(const char *functionName, const uint32_t line_num);
void rule13x17x(const Vector *stmts, const char *functionName);

void checkSemantics(const AST ast) {

    st_global = *(SymbolTable **)vec_getAt(symbol_tables, 0);

    functions = ht_init(sizeof(char *), sizeof(astFunction *), (ht_hash)ht_polyRollingHash, (ht_kcopy)str_cpyctr, NULL,
                        (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);

    // go through all scopes (functions)
    for (int i = 0; i < ast->otherFunctions->size; i++) {
        astFunction *fn = *(astFunction **)vec_getAt(ast->otherFunctions, i);

        rule12(fn->functionName, fn->line_num);

        ht_insert(&functions, &fn->functionName, &fn);

        rule3x5x(fn);

        assignments = ht_init(sizeof(char *), sizeof(astID *), (ht_hash)ht_polyRollingHash, (ht_kcopy)str_cpyctr, NULL,
                              (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);
        rule8x(fn);
        ht_free(assignments);

        rule13x17x(fn->statements, fn->functionName);
    }

    // main function
    rule3x5x(ast->mainFunction);

    // free
    ht_free(functions);
}

void rule3x5x(const astFunction *fn) {

    // input params
    if (fn->inputParams != NULL) {
        for (int i = 0; i < fn->inputParams->size; i++) {
            astID *id = *(astID **)vec_getAt(fn->inputParams, i);
            rule3(id);
            rule5(id);
        }
    }

    // output params
    if (fn->outputParams != NULL) {
        for (int i = 0; i < fn->outputParams->size; i++) {
            astID *id = *(astID **)vec_getAt(fn->outputParams, i);
            rule3(id);
            rule5(id);
        }
    }

    // declarations
    for (int i = 0; i < fn->statements->size; i++) {
        genericStatement *stmt = *(genericStatement **)vec_getAt(fn->statements, i);
        if (stmt->tag_stmt_type != STMT_DECLARATION)
            continue;

        astStmtDeclaration *dec = stmt->stmt;
        astID              *id  = dec->id;
        if (!dec->global)
            rule3(id);
    }
}

void rule3(const astID *id) {
    void **lookup = ht_lookup(st_global->table, &id->id);
    if (lookup != NULL)
        printf(RED "Line: %d - Redclaration of global variable %s" RESET "\n", id->line_num, id->id);
}

void rule5(const astID *id) {
    void **lookup = ht_lookup(unions, &id->id);
    if (lookup != NULL)
        printf(RED "Line: %d - Union is passed as parameter %s" RESET "\n", id->line_num, id->id);
}

void rule8x(const astFunction *fn) {
    if (fn->outputParams == NULL)
        return;

    rule8(fn->statements);

    for (int i = 0; i < fn->outputParams->size; i++) {
        astID *id     = *(astID **)vec_getAt(fn->outputParams, i);
        void **lookup = ht_lookup(assignments, &id->id);
        if (lookup == NULL)
            printf(RED "Line: %d - Output parameter %s has not been assigned" RESET "\n", id->line_num, id->id);
    }
}

void rule8(const Vector *stmts) {

    if (stmts == NULL)
        return;

    for (int i = 0; i < stmts->size; i++) {
        genericStatement *stmt = *(genericStatement **)vec_getAt(stmts, i);

        switch (stmt->tag_stmt_type) {
        case STMT_TYPEDEFINITION:
        case STMT_DEFINETYPE:
        case STMT_DECLARATION:
            break;
        case STMT_ASSIGNMENT: {
            astSingleOrRecID *sori = ((astStmtAssignment *)stmt->stmt)->lhs;
            if (sori->fields == NULL)
                ht_insert(&assignments, &sori->id->id, &sori->id);
            break;
        }
        case STMT_ITERATIVE:
            rule8(((astStmtIterative *)stmt->stmt)->statements);
            break;
        case STMT_CONDITIONAL:
            rule8(((astStmtConditional *)stmt->stmt)->ifStatements);
            rule8(((astStmtConditional *)stmt->stmt)->elseStatements);
            break;
        case STMT_IOREAD:
        case STMT_IOWRITE:
        case STMT_FNCALL:
        case STMT_RETURN:
            break;
        }
    }
}

void rule12(const char *functionName, const uint32_t line_num) {
    void **lookup = ht_lookup(functions, &functionName);
    if (lookup != NULL) {
        printf(RED "Line: %d - Function overloading detected %s" RESET "\n", line_num, functionName);
    }
}

void rule13x17x(const Vector *stmts, const char *functionName) {

    if (stmts == NULL)
        return;

    for (int i = 0; i < stmts->size; i++) {
        genericStatement *stmt = *(genericStatement **)vec_getAt(stmts, i);

        switch (stmt->tag_stmt_type) {
        case STMT_TYPEDEFINITION:
        case STMT_DEFINETYPE:
        case STMT_DECLARATION:
        case STMT_ASSIGNMENT:
            break;
        case STMT_ITERATIVE:
            rule13x17x(((astStmtIterative *)stmt->stmt)->statements, functionName);
            break;
        case STMT_CONDITIONAL:
            rule13x17x(((astStmtConditional *)stmt->stmt)->ifStatements, functionName);
            rule13x17x(((astStmtConditional *)stmt->stmt)->elseStatements, functionName);
            break;
        case STMT_IOREAD:
        case STMT_IOWRITE:
            break;
        case STMT_FNCALL: {
            astStmtFunCall *fncall = stmt->stmt;

            // rule 13
            if (strcmp(functionName, fncall->functionName) == 0) {
                printf(RED "Line: %d - Recursive function call" RESET "\n", fncall->line_num);
                break;
            }

            // rule 17
            void **lookup = ht_lookup(functions, &fncall->functionName);
            if (lookup == NULL)
                printf(RED "Line: %d - Function call %s before definition" RESET "\n", fncall->line_num,
                       fncall->functionName);
            break;
        }
        case STMT_RETURN:
            break;
        }
    }
}

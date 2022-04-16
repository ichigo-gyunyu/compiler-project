
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

bool semantic_error = false;

void rule3x5x(const astFunction *fn);
void rule3(const astID *id);
void rule5(const astID *id);
void rule8x(const astFunction *fn);
void rule8(const Vector *stmts);
void rule12(const char *functionName, const uint32_t line_num);
void rule13x17x(const Vector *stmts, const char *functionName);
void rule18x(const Vector *stmts);
void rule18BE(const astBooleanExpression *be, Hashtable **iter_vars);
void rule18Var(const astVar *var, Hashtable **iter_vars);
bool rule18(const Vector *stmts, Hashtable **iter_vars);

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

        rule18x(fn->statements);
    }

    // main function
    rule3x5x(ast->mainFunction);
    rule13x17x(ast->mainFunction->statements, ast->mainFunction->functionName);
    rule18x(ast->mainFunction->statements);

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
    if (lookup != NULL) {
        semantic_error = true;
        printf(RED "Line: %d - Redclaration of global variable %s" RESET "\n", id->line_num, id->id);
    }
}

void rule5(const astID *id) {
    void **lookup = ht_lookup(unions, &id->id);
    if (lookup != NULL) {
        semantic_error = true;
        printf(RED "Line: %d - Union is passed as parameter %s" RESET "\n", id->line_num, id->id);
    }
}

void rule8x(const astFunction *fn) {
    if (fn->outputParams == NULL)
        return;

    rule8(fn->statements);

    for (int i = 0; i < fn->outputParams->size; i++) {
        astID *id     = *(astID **)vec_getAt(fn->outputParams, i);
        void **lookup = ht_lookup(assignments, &id->id);
        if (lookup == NULL) {
            semantic_error = true;
            printf(RED "Line: %d - Output parameter %s has not been assigned" RESET "\n", id->line_num, id->id);
        }
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
        case STMT_IOREAD: {
            astVar *var = stmt->stmt;
            if (var->tag_var_or_const == VAR) {
                astSingleOrRecID *sori = var->var;
                ht_insert(&assignments, &sori->id->id, &sori->id);
            }
            break;
        }
        case STMT_IOWRITE:
            break;
        case STMT_FNCALL: {
            astStmtFunCall *funcall = stmt->stmt;
            if (funcall->outputParams != NULL) {
                for (int i = 0; i < funcall->outputParams->size; i++) {
                    astID *id = *(astID **)vec_getAt(funcall->outputParams, i);
                    ht_insert(&assignments, &id->id, &id);
                }
            }
        }
        case STMT_RETURN:
            break;
        }
    }
}

void rule12(const char *functionName, const uint32_t line_num) {
    void **lookup = ht_lookup(functions, &functionName);
    if (lookup != NULL) {
        semantic_error = true;
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
                semantic_error = true;
                printf(RED "Line: %d - Recursive function call" RESET "\n", fncall->line_num);
                break;
            }

            // rule 17
            void **lookup = ht_lookup(functions, &fncall->functionName);
            if (lookup == NULL) {
                semantic_error = true;
                printf(RED "Line: %d - Function call %s before definition" RESET "\n", fncall->line_num,
                       fncall->functionName);
            }
            break;
        }
        case STMT_RETURN:
            break;
        }
    }
}

void rule18x(const Vector *stmts) {

    if (stmts == NULL)
        return;

    for (int i = 0; i < stmts->size; i++) {
        genericStatement *stmt = *(genericStatement **)vec_getAt(stmts, i);

        if (stmt->tag_stmt_type != STMT_ITERATIVE)
            continue;

        astStmtIterative *iter = stmt->stmt;

        Hashtable *iter_vars; // <char *, astID *>

        iter_vars = ht_init(sizeof(char *), sizeof(astStmtTypeDefinition *), (ht_hash)ht_polyRollingHash,
                            (ht_kcopy)str_cpyctr, NULL, (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);

        rule18BE(iter->precondition, &iter_vars);

        bool reassigned = rule18(iter->statements, &iter_vars);
        if (!reassigned) {
            semantic_error = true;
            printf(RED "Line: %d - No variables participating in iterations have been reassigned" RESET "\n",
                   iter->line_num);
        }

        ht_free(iter_vars);
    }
}

void rule18BE(const astBooleanExpression *be, Hashtable **iter_vars) {

    switch (be->tag_be_type) {
    case LOGICAL:
        rule18BE(((astBooleanExpressionLogical *)be->be)->lhs, iter_vars);
        rule18BE(((astBooleanExpressionLogical *)be->be)->rhs, iter_vars);
        break;
    case RELATIONAL:
        rule18Var(((astBooleanExpressionRelational *)be->be)->lhs, iter_vars);
        rule18Var(((astBooleanExpressionRelational *)be->be)->rhs, iter_vars);
        break;
    case NEGATION:
        rule18BE(((astBooleanExpressionNegation *)be->be)->exp, iter_vars);
        break;
    }
}

void rule18Var(const astVar *var, Hashtable **iter_vars) {

    if (var->tag_var_or_const != VAR)
        return;

    astSingleOrRecID *sori = var->var;
    astID            *id   = sori->id;

    ht_insert(iter_vars, &id->id, &id);
}

bool rule18(const Vector *stmts, Hashtable **iter_vars) {

    if (stmts == NULL)
        return false;

    for (int i = 0; i < stmts->size; i++) {
        genericStatement *stmt = *(genericStatement **)vec_getAt(stmts, i);

        switch (stmt->tag_stmt_type) {
        case STMT_TYPEDEFINITION:
        case STMT_DEFINETYPE:
        case STMT_DECLARATION:
            break;

        case STMT_ASSIGNMENT: {
            astSingleOrRecID *sori   = ((astStmtAssignment *)stmt->stmt)->lhs;
            void            **lookup = ht_lookup(*iter_vars, &sori->id->id);
            if (lookup != NULL)
                return true;

            break;
        }

        case STMT_ITERATIVE: {

            astStmtIterative *iter = stmt->stmt;

            Hashtable *iter_vars2; // <char *, astID *>

            iter_vars2 =
                ht_init(sizeof(char *), sizeof(astStmtTypeDefinition *), (ht_hash)ht_polyRollingHash,
                        (ht_kcopy)str_cpyctr, NULL, (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);

            rule18BE(iter->precondition, &iter_vars2);

            bool reassigned = rule18(iter->statements, &iter_vars2);
            if (!reassigned) {
                semantic_error = true;
                printf(RED "Line: %d - No variables participating in iterations have been reassigned" RESET "\n",
                       iter->line_num);
            }

            ht_free(iter_vars2);

            break;
        }

        case STMT_CONDITIONAL:
            rule18(((astStmtConditional *)stmt->stmt)->ifStatements, iter_vars);
            rule18(((astStmtConditional *)stmt->stmt)->elseStatements, iter_vars);
            break;

        case STMT_IOREAD: {
            astVar *var = stmt->stmt;
            if (var->tag_var_or_const == VAR) {
                astSingleOrRecID *sori   = var->var;
                void            **lookup = ht_lookup(*iter_vars, &sori->id->id);
                if (lookup != NULL)
                    return true;
            }

            break;
        }

        case STMT_IOWRITE:
            break;

        case STMT_FNCALL: {
            astStmtFunCall *fncall = stmt->stmt;
            if (fncall->outputParams != NULL) {
                for (int i = 0; i < fncall->outputParams->size; i++) {
                    astID *id = *(astID **)vec_getAt(fncall->outputParams, i);

                    void **lookup = ht_lookup(*iter_vars, &id->id);
                    if (lookup != NULL)
                        return true;
                }
            }

            break;
        }

        case STMT_RETURN:
            break;
        }
    }

    return false;
}

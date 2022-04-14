
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "TypeChecker/typeChecker.h"

static SymbolTable *st_global;
static Hashtable   *fn_signature; // <char *, astFunction *>

void initTypeValidator(const AST ast);
void typeValidatorFn(const astFunction *fn);
void typeValidatorID(const astID *id);

void initTypeChecker(const AST ast);
void typeCheckerStmts(const Vector *stmts, const SymbolTable *st);
void typeCheckAssignment(const astStmtAssignment *assign, const SymbolTable *st);
void typeCheckBE(const astBooleanExpression *be, const SymbolTable *st);
void typeCheckFnCall(const astStmtFunCall *fncall, const SymbolTable *st);
void typeCheckReturn(const astStmtReturn *ret, const SymbolTable *st);
void typeCheckParamList(const Vector *expected, const Vector *got, const SymbolTable *st, const uint32_t line_num);

char *getTypeSoRI(const astSingleOrRecID *sori, const SymbolTable *st);
char *getTypeAE(const astArithmeticExpression *ae, const SymbolTable *st, const uint32_t line_num);
char *getTypeVar(const astVar *var, const SymbolTable *st);
char *getTypeID(const astID *id, const SymbolTable *st);

void initTypeValidator(const AST ast) {

    // go through all scopes (functions)
    for (int i = 0; i < ast->otherFunctions->size; i++) {
        astFunction *fn = *(astFunction **)vec_getAt(ast->otherFunctions, i);
        typeValidatorFn(fn);
    }

    // main function
    typeValidatorFn(ast->mainFunction);
}

void typeValidatorFn(const astFunction *fn) {

    // input params
    if (fn->inputParams != NULL) {
        for (int i = 0; i < fn->inputParams->size; i++) {
            astID *id = *(astID **)vec_getAt(fn->inputParams, i);
            typeValidatorID(id);
        }
    }

    // output params
    if (fn->outputParams != NULL) {
        for (int i = 0; i < fn->outputParams->size; i++) {
            astID *id = *(astID **)vec_getAt(fn->outputParams, i);
            typeValidatorID(id);
        }
    }

    // declarations
    for (int i = 0; i < fn->statements->size; i++) {
        genericStatement *stmt = *(genericStatement **)vec_getAt(fn->statements, i);
        if (stmt->tag_stmt_type != STMT_DECLARATION)
            continue;

        astStmtDeclaration *dec = stmt->stmt;
        astID              *id  = dec->id;
        typeValidatorID(id);
    }
}

void typeValidatorID(const astID *id) {

    if (strcmp(id->type, "int") == 0 || strcmp(id->type, "real") == 0)
        return;

    void **lookup = ht_lookup(rinfo, &id->type);
    if (lookup == NULL)
        printf(RED "Line: %d - Invalid type %s" RESET "\n", id->line_num, id->type);
}

void initTypeChecker(const AST ast) {

    st_global = *(SymbolTable **)vec_getAt(symbol_tables, 0);

    fn_signature = ht_init(sizeof(char *), sizeof(astFunction *), (ht_hash)ht_polyRollingHash, (ht_kcopy)str_cpyctr,
                           NULL, (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);

    // go through all scopes (functions)
    for (int i = 0; i < ast->otherFunctions->size; i++) {
        astFunction *fn = *(astFunction **)vec_getAt(ast->otherFunctions, i);
        SymbolTable *st = *(SymbolTable **)vec_getAt(symbol_tables, i + 1);
        ht_insert(&fn_signature, &fn->functionName, &fn);
        typeCheckerStmts(fn->statements, st);
    }

    // main function
    astFunction *fn = ast->mainFunction;
    SymbolTable *st = *(SymbolTable **)vec_getAt(symbol_tables, symbol_tables->size - 1);
    ht_insert(&fn_signature, &fn->functionName, &fn);
    typeCheckerStmts(fn->statements, st);

    // free
    ht_free(fn_signature);
}

void typeCheckerStmts(const Vector *stmts, const SymbolTable *st) {

    if (stmts == NULL)
        return;

    for (int i = 0; i < stmts->size; i++) {
        genericStatement *stmt = *(genericStatement **)vec_getAt(stmts, i);

        switch (stmt->tag_stmt_type) {
        case STMT_TYPEDEFINITION:
        case STMT_DEFINETYPE:
        case STMT_DECLARATION:
            break;
        case STMT_ASSIGNMENT:
            typeCheckAssignment(stmt->stmt, st);
            break;
        case STMT_ITERATIVE:
            typeCheckBE(((astStmtIterative *)stmt->stmt)->precondition, st);
            typeCheckerStmts(((astStmtIterative *)stmt->stmt)->statements, st);
            break;
        case STMT_CONDITIONAL:
            typeCheckBE(((astStmtConditional *)stmt->stmt)->condition, st);
            typeCheckerStmts(((astStmtConditional *)stmt->stmt)->ifStatements, st);
            typeCheckerStmts(((astStmtConditional *)stmt->stmt)->elseStatements, st);
            break;
        case STMT_IOREAD:
        case STMT_IOWRITE:
            break;
        case STMT_FNCALL:
            typeCheckFnCall(stmt->stmt, st);
            break;
        case STMT_RETURN:
            typeCheckReturn(stmt->stmt, st);
            break;
        }
    }
}

void typeCheckAssignment(const astStmtAssignment *assign, const SymbolTable *st) {

    char *lhs_type = getTypeSoRI(assign->lhs, st);
    char *rhs_type = getTypeAE(assign->rhs, st, assign->lhs->id->line_num);

    if (lhs_type && rhs_type && strcmp(lhs_type, rhs_type)) {
        printf(RED "Line: %d - Type mismatch %s and %s" RESET "\n", assign->lhs->id->line_num, lhs_type, rhs_type);
    }
}

void typeCheckBE(const astBooleanExpression *be, const SymbolTable *st) {

    switch (be->tag_be_type) {
    case LOGICAL:
        typeCheckBE(((astBooleanExpressionLogical *)be->be)->lhs, st);
        typeCheckBE(((astBooleanExpressionLogical *)be->be)->rhs, st);
        break;

    case RELATIONAL: {
        astBooleanExpressionRelational *ber = be->be;

        char *lhs_type = getTypeVar(ber->lhs, st);
        char *rhs_type = getTypeVar(ber->rhs, st);

        if (lhs_type && rhs_type && strcmp(lhs_type, rhs_type)) {
            printf(RED "Line: %d - Type mismatch %s and %s" RESET "\n", ber->line_num, lhs_type, rhs_type);
        }

        break;
    }

    case NEGATION:
        typeCheckBE(((astBooleanExpressionNegation *)be->be)->exp, st);
        break;
    }
}

void typeCheckFnCall(const astStmtFunCall *fncall, const SymbolTable *st) {

    void **lookup = ht_lookup(fn_signature, &fncall->functionName);
    if (lookup == NULL) {
        printf(RED "Line: %d - No such function" RESET "\n", fncall->line_num);
        return;
    }
    astFunction *fn = *lookup;

    // check input params
    if (fncall->inputParams == NULL && fn->inputParams != NULL)
        printf(RED "Line: %d - Expected %d input parameters, got 0" RESET "\n", fncall->line_num,
               fn->inputParams->size);
    if (fncall->inputParams != NULL && fn->inputParams == NULL)
        printf(RED "Line: %d - Expected 0 input parameters, got %d" RESET "\n", fncall->line_num,
               fncall->inputParams->size);

    if (fncall->inputParams != NULL && fn->inputParams != NULL) {
        if (fncall->inputParams->size != fn->inputParams->size)
            printf(RED "Line: %d - Expected %d input parameters, got %d" RESET "\n", fncall->line_num,
                   fn->inputParams->size, fncall->inputParams->size);
        else
            typeCheckParamList(fn->inputParams, fncall->inputParams, st, fncall->line_num);
    }

    // check output params
    if (fncall->outputParams == NULL && fn->outputParams != NULL)
        printf(RED "Line: %d - Expected %d output parameters, got 0" RESET "\n", fncall->line_num,
               fn->outputParams->size);
    if (fncall->outputParams != NULL && fn->outputParams == NULL)
        printf(RED "Line: %d - Expected 0 output parameters, got %d" RESET "\n", fncall->line_num,
               fncall->outputParams->size);

    if (fncall->outputParams != NULL && fn->outputParams != NULL) {
        if (fncall->outputParams->size != fn->outputParams->size)
            printf(RED "Line: %d - Expected %d output parameters, got %d" RESET "\n", fncall->line_num,
                   fn->outputParams->size, fncall->outputParams->size);
        else
            typeCheckParamList(fn->outputParams, fncall->outputParams, st, fncall->line_num);
    }
}

void typeCheckReturn(const astStmtReturn *ret, const SymbolTable *st) {

    astFunction *fn = *(astFunction **)ht_lookup(fn_signature, &st->scope);

    if (ret->returnList == NULL && fn->outputParams != NULL)
        printf(RED "Line: %d - Expected %d parameters, got 0" RESET "\n", ret->line_num, fn->outputParams->size);
    if (ret->returnList != NULL && fn->outputParams == NULL)
        printf(RED "Line: %d - Expected 0 parameters, got %d" RESET "\n", ret->line_num, ret->returnList->size);
    if (ret->returnList == NULL && fn->outputParams == NULL)
        return;

    if (ret->returnList != NULL && fn->outputParams != NULL) {
        if (ret->returnList->size != fn->outputParams->size)
            printf(RED "Line: %d - Expected %d parameters, got %d" RESET "\n", ret->line_num, fn->outputParams->size,
                   ret->returnList->size);
        else
            typeCheckParamList(fn->outputParams, ret->returnList, st, ret->line_num);
    }

    typeCheckParamList(fn->outputParams, ret->returnList, st, ret->line_num);
}

void typeCheckParamList(const Vector *expected, const Vector *got, const SymbolTable *st, const uint32_t line_num) {

    if (expected->size != got->size)
        return;

    uint32_t sz = expected->size;
    for (int i = 0; i < sz; i++) {
        astID *e = *(astID **)vec_getAt(expected, i);
        astID *g = *(astID **)vec_getAt(got, i);

        char *etype = getTypeID(e, st);
        char *gtype = getTypeID(g, st);

        if (etype != NULL && gtype != NULL && strcmp(etype, gtype)) {
            printf(RED "Line: %d - Type mismatch %s (%s) and %s" RESET "\n", g->line_num, g->id, gtype, etype);
        }
    }
}

char *getTypeSoRI(const astSingleOrRecID *sori, const SymbolTable *st) {

    char *type = getTypeID(sori->id, st);

    if (type == NULL || sori->fields == NULL)
        return type;

    for (int i = 0; i < sori->fields->size; i++) {
        astFieldID *fid = *(astFieldID **)vec_getAt(sori->fields, i);

        if (strcmp(type, "int") == 0 || strcmp(type, "real") == 0) {
            printf(RED "Line: %d - Primitive type %s cannot have fields" RESET "\n", fid->line_num, type);
            return NULL;
        }

        astStmtTypeDefinition *td = *(astStmtTypeDefinition **)ht_lookup(rec_defns, &type);

        // check if valid field name
        bool validname = false;
        for (int j = 0; j < td->fieldDefinitions->size; j++) {
            astFieldID *f = *(astFieldID **)vec_getAt(td->fieldDefinitions, j);
            if (strcmp(f->id, fid->id) == 0) {
                validname = true;
                type      = f->type;
                break;
            }
        }

        if (!validname) {
            printf(RED "Line: %d - No such field name (%s)" RESET "\n", fid->line_num, fid->id);
            return NULL;
        }
    }

    return type;
}

char *getTypeAE(const astArithmeticExpression *ae, const SymbolTable *st, const uint32_t line_num) {

    if (ae->isVar)
        return getTypeVar(ae->var, st);

    char *lhs_type = getTypeAE(ae->lhs, st, line_num);
    char *rhs_type = getTypeAE(ae->rhs, st, line_num);

    if (lhs_type == NULL || rhs_type == NULL)
        return NULL;

    bool lhs_primitive = false;
    if (strcmp(lhs_type, "int") == 0 || strcmp(lhs_type, "real") == 0)
        lhs_primitive = true;

    bool rhs_primitive = false;
    if (strcmp(rhs_type, "int") == 0 || strcmp(rhs_type, "real") == 0)
        rhs_primitive = true;

    switch (ae->operator) {
    case PLUS:
    case MINUS: {
        if (strcmp(lhs_type, rhs_type) != 0) {
            printf(RED "Line: %d - Cannot add/subtract %s with %s" RESET "\n", line_num, lhs_type, rhs_type);
            return NULL;
        }
        return lhs_type;
    }

    case MUL: {
        if (!lhs_primitive && !rhs_primitive) {
            printf(RED "Line: %d - Cannot multiply %s with %s" RESET "\n", line_num, lhs_type, rhs_type);
            return NULL;
        }

        if (strcmp(lhs_type, rhs_type) != 0) {
            if (strcmp(lhs_type, "int") == 0)
                return rhs_type;
            if (strcmp(rhs_type, "int") == 0)
                return lhs_type;

            printf(RED "Line: %d - Cannot multiply %s with %s" RESET "\n", line_num, lhs_type, rhs_type);
            return NULL;
        } else
            return lhs_type;
    }

    case DIV: {
        if (!lhs_primitive || !rhs_primitive) {
            printf(RED "Line: %d - Cannot divide %s with %s" RESET "\n", line_num, lhs_type, rhs_type);
            return NULL;
        }

        free(lhs_type);
        free(rhs_type);
        return str_dup("real");
    }
    }

    return NULL; // -Wall
}

char *getTypeVar(const astVar *var, const SymbolTable *st) {

    switch (var->tag_var_or_const) {
    case VAR:
        return getTypeSoRI(var->var, st);
    case INT:
        return str_dup("int");
    case REAL:
        return str_dup("real");
    }

    return NULL; // -Wall
}

char *getTypeID(const astID *id, const SymbolTable *st) {

    void **lookup = ht_lookup(st->table, &id->id);
    if (lookup == NULL) {
        lookup = ht_lookup(st_global->table, &id->id); // check the global table
        if (lookup == NULL) {
            printf(RED "Line: %d - Unknown identifier %s" RESET "\n", id->line_num, id->id);
            return NULL;
        }
    }

    SymbolTableEntry *ste = *lookup;
    if (ste->type_expression->is_primitive)
        return str_dup(ste->type_expression->prim_or_rec);

    else {
        char *ruid = *(char **)vec_getAt(ste->type_name, ste->type_name->size - 1);

        // resolve aliases
        astStmtTypeDefinition *td = *(astStmtTypeDefinition **)ht_lookup(rec_defns, &ruid);
        ruid                      = td->ruid;

        return str_dup(ruid);
    }
}

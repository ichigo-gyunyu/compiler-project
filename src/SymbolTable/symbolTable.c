
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "SymbolTable/symbolTable.h"

Vector    *ruids; // Vector <char *>
RecordInfo rinfo;
Hashtable *rec_defns;     // <char *, astStmtTypeDefinition *>
Hashtable *aliases;       // <char *, Vector<char *>>
Vector    *symbol_tables; // Vector <SymbolTable *>

void initRecordInfo(const AST ast);

void fillRecDefns(Hashtable **rec_defns, const Vector *statements);
void fillRecInfo(const char *ruid);

SymbolTable *createSymbolTable(char *scope);
void         constructSymbolTables(const AST ast);
void         fillSymbolTable(SymbolTable *st, const astFunction *fn, const bool global_table);

SymbolTableEntry *makeSymbolTableEntry(const char *type, const char *id, const bool is_global, const uint32_t offset,
                                       const UsageType usage);

void printSymbolTableEntry(const uint32_t line_num, const char *id, const SymbolTableEntry *ste, const char *scope);
void printTypeExpression(const TypeExpression *te);

/* void printSTEFunction(const astFunction *fn, const SymbolTable *st, const SymbolTable *st_global);
bool printSTE(const astID *astid, const SymbolTable *st);
void printSTEStatements(const Vector *stmts, const SymbolTable *st, const SymbolTable *st_global);
void printSTEAE(const astArithmeticExpression *ae, const SymbolTable *st, const SymbolTable *st_global);
void printSTEBE(const astBooleanExpression *be, const SymbolTable *st, const SymbolTable *st_global);
void printSTEVar(const astVar *var, const SymbolTable *st, const SymbolTable *st_global);
void printSTEFnCall(const astStmtFunCall *fncall, const SymbolTable *st, const SymbolTable *st_global); */

void initRecordInfo(const AST ast) {
    ruids = vec_init(sizeof(char *), (vec_copyctr)str_cpyctr, (vec_dtr)str_dtr, VEC_START_SIZE);

    rinfo = ht_init(sizeof(char *), sizeof(RecordInfoEntry *), (ht_hash)ht_polyRollingHash, (ht_kcopy)str_cpyctr, NULL,
                    (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);

    rec_defns = ht_init(sizeof(char *), sizeof(astStmtTypeDefinition *), (ht_hash)ht_polyRollingHash,
                        (ht_kcopy)str_cpyctr, NULL, (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);

    aliases = ht_init(sizeof(char *), sizeof(Vector *), (ht_hash)ht_polyRollingHash, (ht_kcopy)str_cpyctr, NULL,
                      (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);

    Vector *stmts = ast->mainFunction->statements;
    fillRecDefns(&rec_defns, stmts);
    Vector *fns = ast->otherFunctions;
    for (int i = 0; i < fns->size; i++) {
        astFunction *fn    = *(astFunction **)vec_getAt(fns, i);
        Vector      *stmts = fn->statements;
        fillRecDefns(&rec_defns, stmts);
    }

    for (int i = 0; i < ruids->size; i++)
        fillRecInfo(*(char **)vec_getAt(ruids, i));

    ht_free(rec_defns);
}

void fillRecDefns(Hashtable **rec_defns, const Vector *statements) {

    // record definitions
    for (int i = 0; i < statements->size; i++) {
        genericStatement *stmt = *(genericStatement **)vec_getAt(statements, i);

        if (stmt->tag_stmt_type != STMT_TYPEDEFINITION)
            continue;

        astStmtTypeDefinition *td = stmt->stmt;
        if (td->tag_rec_or_union == UNION)
            continue;

        ht_insert(rec_defns, &(td->ruid), &td);
        vec_pushBack(ruids, &(td->ruid));
    }

    // aliases
    for (int i = 0; i < statements->size; i++) {
        genericStatement *stmt = *(genericStatement **)vec_getAt(statements, i);

        if (stmt->tag_stmt_type != STMT_DEFINETYPE)
            continue;

        astStmtDefineType *dt = stmt->stmt;
        if (dt->tag_rec_or_union == UNION)
            continue;

        astStmtTypeDefinition **td = ht_lookup(*rec_defns, &dt->ruid);
        if (td == NULL) {
            printf(RED "definetype error\n" RESET);
            exit(EXIT_FAILURE); // TODO
        }

        ht_insert(rec_defns, &(dt->ruid_as), td);
        vec_pushBack(ruids, &(dt->ruid_as));

        // make it known that ruid_as is an alias for ruid
        Vector **alias_list = ht_lookup(aliases, &dt->ruid);
        if (alias_list == NULL) {
            Vector *new_list = vec_init(sizeof(char *), (vec_copyctr)str_cpyctr, (vec_dtr)str_dtr, VEC_START_SIZE);
            ht_insert(&aliases, &dt->ruid, &new_list);
            alias_list = ht_lookup(aliases, &dt->ruid);
        }
        vec_pushBack(*alias_list, &dt->ruid_as);
    }
}

void fillRecInfo(const char *ruid) {

    // already filled
    if (ht_lookup(rinfo, &ruid) != NULL)
        return;

    uint32_t        width = 0;
    TypeExpression *te    = malloc(sizeof *te);

    *te = (TypeExpression){
        .is_primitive = false,
        .prim_or_rec  = vec_init(sizeof(TypeExpression *), NULL, NULL, VEC_START_SIZE),
    };

    astStmtTypeDefinition *td = *(astStmtTypeDefinition **)ht_lookup(rec_defns, &ruid);
    for (int i = 0; i < td->fieldDefinitions->size; i++) {
        astFieldID *field = *(astFieldID **)vec_getAt(td->fieldDefinitions, i);
        char       *type  = field->type;

        if ((strcmp(type, "int") == 0 && (width += WIDTH_INT)) ||
            (strcmp(type, "real") == 0 && (width += WIDTH_REAL))) {
            TypeExpression *teprim = malloc(sizeof *teprim);

            *teprim = (TypeExpression){
                .is_primitive = true,
                .prim_or_rec  = str_dup(type),
            };

            vec_pushBack(te->prim_or_rec, &teprim);
            continue;
        }

        fillRecInfo(type);
        RecordInfoEntry *entry = *(RecordInfoEntry **)ht_lookup(rinfo, &type);
        TypeExpression  *terec = entry->type_expression;
        width += entry->width;
        vec_pushBack(te->prim_or_rec, &terec);
    }

    Vector **type_name = ht_lookup(aliases, &ruid);
    if (type_name == NULL) {
        Vector *tn = vec_init(sizeof(char *), (vec_copyctr)str_cpyctr, (vec_dtr)str_dtr, VEC_START_SIZE);
        ht_insert(&aliases, &ruid, &tn);
        type_name = ht_lookup(aliases, &ruid);
    }
    vec_pushBack(*type_name, &ruid);

    RecordInfoEntry *entry = malloc(sizeof *entry);

    *entry = (RecordInfoEntry){
        .width           = width,
        .type_expression = te,
        .type_name       = *type_name,
    };

    ht_insert(&rinfo, &ruid, &entry);
}

SymbolTable *createSymbolTable(char *scope) {
    SymbolTable *st = malloc(sizeof *st);

    *st = (SymbolTable){
        .scope       = str_dup(scope),
        .total_width = 0,
        .table = ht_init(sizeof(char *), sizeof(RecordInfoEntry *), (ht_hash)ht_polyRollingHash, (ht_kcopy)str_cpyctr,
                         NULL, (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE),
    };

    return st;
}

void constructSymbolTables(const AST ast) {
    symbol_tables = vec_init(sizeof(SymbolTable *), NULL, NULL, VEC_START_SIZE);

    // create the global symbol tables
    SymbolTable *st_global = createSymbolTable("global");
    vec_pushBack(symbol_tables, &st_global);

    // function scoped symbol tables
    SymbolTable *st;
    for (int i = 0; i < ast->otherFunctions->size; i++) {
        astFunction *fn = *(astFunction **)vec_getAt(ast->otherFunctions, i);
        st              = createSymbolTable(fn->functionName);
        vec_pushBack(symbol_tables, &st);
        fillSymbolTable(st, fn, false);
        fillSymbolTable(st_global, fn, true);
    }

    st = createSymbolTable(ast->mainFunction->functionName);
    vec_pushBack(symbol_tables, &st);
    fillSymbolTable(st, ast->mainFunction, false);
    fillSymbolTable(st_global, ast->mainFunction, true);
}

SymbolTableEntry *makeSymbolTableEntry(const char *type, const char *id, const bool is_global, const uint32_t offset,
                                       const UsageType usage) {

    SymbolTableEntry *ste = malloc(sizeof *ste);
    uint32_t          width;

    if ((strcmp(type, "int") == 0 && (width = WIDTH_INT)) || (strcmp(type, "real") == 0 && (width = WIDTH_REAL))) {

        *ste = (SymbolTableEntry){
            .type_name       = NULL,
            .type_expression = malloc(sizeof(TypeExpression)),
            .width           = width,
            .is_global       = is_global,
            .offset          = offset,
            .usage           = usage,
        };

        *(ste->type_expression) = (TypeExpression){
            .is_primitive = true,
            .prim_or_rec  = str_dup(type),
        };

        return ste;
    }

    // not a primitive type
    RecordInfoEntry **rie = ht_lookup(rinfo, &type);
    if (rie == NULL) {
        printf(RED "invalid type name (symbol table construction)\n" RESET);
        return NULL;
    }

    *ste = (SymbolTableEntry){
        .type_name       = (*rie)->type_name,
        .type_expression = (*rie)->type_expression,
        .width           = (*rie)->width,
        .is_global       = is_global,
        .offset          = offset,
        .usage           = usage,
    };

    return ste;
}

void fillSymbolTable(SymbolTable *st, const astFunction *fn, const bool global_table) {

    // go through all input parameters
    if (!global_table && fn->inputParams != NULL) {
        for (int i = 0; i < fn->inputParams->size; i++) {
            astID            *astid    = *(astID **)vec_getAt(fn->inputParams, i);
            char             *type     = astid->type;
            char             *id       = astid->id;
            uint32_t          line_num = astid->line_num;
            SymbolTableEntry *ste      = makeSymbolTableEntry(type, id, false, st->total_width, INPARAM);

            if (ste == NULL)
                continue;

            st->total_width += ste->width;
            ht_insert(&st->table, &id, &ste);
            printSymbolTableEntry(line_num, id, ste, st->scope);
        }
    }

    // go through all output parameters
    if (!global_table && fn->outputParams != NULL) {
        for (int i = 0; i < fn->outputParams->size; i++) {
            astID            *astid    = *(astID **)vec_getAt(fn->outputParams, i);
            char             *type     = astid->type;
            char             *id       = astid->id;
            uint32_t          line_num = astid->line_num;
            SymbolTableEntry *ste      = makeSymbolTableEntry(type, id, false, st->total_width, INPARAM);

            if (ste == NULL)
                continue;

            st->total_width += ste->width;
            ht_insert(&st->table, &id, &ste);
            printSymbolTableEntry(line_num, id, ste, st->scope);
        }
    }

    // go through all declarations in a function
    for (int i = 0; i < fn->statements->size; i++) {
        genericStatement *stmt = *(genericStatement **)vec_getAt(fn->statements, i);

        if (stmt->tag_stmt_type != STMT_DECLARATION)
            continue;

        bool is_global = ((astStmtDeclaration *)stmt->stmt)->global;
        if (is_global != global_table)
            continue;

        char             *type     = ((astStmtDeclaration *)stmt->stmt)->id->type;
        char             *id       = ((astStmtDeclaration *)stmt->stmt)->id->id;
        uint32_t          line_num = ((astStmtDeclaration *)stmt->stmt)->id->line_num;
        SymbolTableEntry *ste      = makeSymbolTableEntry(type, id, is_global, st->total_width, LOCAL);

        if (ste == NULL)
            continue;

        st->total_width += ste->width;
        ht_insert(&st->table, &id, &ste);
        printSymbolTableEntry(line_num, id, ste, st->scope);
    }
}

void printSymbolTableEntry(const uint32_t line_num, const char *id, const SymbolTableEntry *ste, const char *scope) {
    printf("%8d ", line_num);
    printf("%15s ", id);
    printf("%30s ", scope);
    printf("%10d ", ste->width);
    printf("%10d ", ste->offset);

    switch (ste->usage) {
    case LOCAL:
        printf("%10s ", "local");
        break;
    case GLOBAL:
        printf("%10s ", "---");
        break;
    case INPARAM:
        printf("%10s ", "in param");
        break;
    case OUTPARAM:
        printf("%10s ", "out param");
        break;
    }

    if (ste->is_global)
        printf("%10s ", "global");
    else
        printf("%10s ", "---");

    printf("\t");

    if (ste->type_name == NULL) {
        printf("---");
    } else {
        for (int i = 0; i < ste->type_name->size; i++) {
            char *tn = *(char **)vec_getAt(ste->type_name, i);
            printf("%s, ", tn);
        }
    }

    printf("\t\t");
    printTypeExpression(ste->type_expression);

    printf("\n");
}

void printTypeExpression(const TypeExpression *te) {

    if (te->is_primitive) {
        printf("%s, ", (char *)te->prim_or_rec);
        return;
    }

    printf("< ");

    Vector *rec_exp = te->prim_or_rec;
    for (int i = 0; i < rec_exp->size; i++) {
        TypeExpression *te2 = *(TypeExpression **)vec_getAt(rec_exp, i);
        printTypeExpression(te2);
    }

    printf(">, ");
}

void printSymbolTable(const AST ast) {
    printf("\n-------- SYMBOL TABLE ENTRIES --------\n\n");
    printf("%8s %15s %30s %10s %10s %10s %10s \t %s \t %s\n", "LINE_NUM", "NAME", "SCOPE", "WIDTH", "OFFSET", "USAGE",
           "ISGLOBAL", "TYPE EXP", "TYPE NAME");

    initRecordInfo(ast);
    constructSymbolTables(ast);
}

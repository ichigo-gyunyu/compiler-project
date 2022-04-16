
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
Hashtable *unions;        // <char *, astStmtTypeDefinition *>
Vector    *symbol_tables; // Vector <SymbolTable *>
Hashtable *scope_st;      // <char *, SymbolTable *>

void initRecordInfo(const AST ast);

void fillRecDefns(const Vector *statements);
void fillRecAliases(const Vector *statements);
void fillRecInfo(const char *ruid, const uint32_t line_num);

SymbolTable *createSymbolTable(char *scope);
void         constructSymbolTables(const AST ast, const bool print_all, const bool print_entry);
void         fillSymbolTable(SymbolTable *st, const astFunction *fn, const bool global_table, const bool print_all,
                             const bool print_entry);

SymbolTableEntry *makeSymbolTableEntry(const char *type, const char *id, const bool is_global, const uint32_t offset,
                                       const UsageType usage, const uint32_t line_num, const bool to_print);

void printSymbolTableEntry(const uint32_t line_num, const char *id, const SymbolTableEntry *ste, const char *scope);
void printTypeExpression(const TypeExpression *te);

void rie_dtr(RecordInfoEntry **rie);
void ste_dtr(SymbolTableEntry **st);
void te_dtr(TypeExpression **te);

void initRecordInfo(const AST ast) {
    ruids = vec_init(sizeof(char *), (vec_copyctr)str_cpyctr, (vec_dtr)str_dtr, VEC_START_SIZE);

    rinfo = ht_init(sizeof(char *), sizeof(RecordInfoEntry *), (ht_hash)ht_polyRollingHash, (ht_kcopy)str_cpyctr, NULL,
                    (ht_kdtr)str_dtr, (ht_vdtr)rie_dtr, (ht_kequal)str_equal, HT_START_SIZE);

    rec_defns = ht_init(sizeof(char *), sizeof(astStmtTypeDefinition *), (ht_hash)ht_polyRollingHash,
                        (ht_kcopy)str_cpyctr, NULL, (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);

    unions = ht_init(sizeof(char *), sizeof(astStmtTypeDefinition *), (ht_hash)ht_polyRollingHash, (ht_kcopy)str_cpyctr,
                     NULL, (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);

    aliases = ht_init(sizeof(char *), sizeof(Vector *), (ht_hash)ht_polyRollingHash, (ht_kcopy)str_cpyctr, NULL,
                      (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);

    // type definitions
    Vector *stmts = ast->mainFunction->statements;
    fillRecDefns(stmts);
    Vector *fns = ast->otherFunctions;
    for (int i = 0; i < fns->size; i++) {
        astFunction *fn    = *(astFunction **)vec_getAt(fns, i);
        Vector      *stmts = fn->statements;
        fillRecDefns(stmts);
    }

    // definetype
    stmts = ast->mainFunction->statements;
    fillRecAliases(stmts);
    fns = ast->otherFunctions;
    for (int i = 0; i < fns->size; i++) {
        astFunction *fn    = *(astFunction **)vec_getAt(fns, i);
        Vector      *stmts = fn->statements;
        fillRecAliases(stmts);
    }

    for (int i = 0; i < ruids->size; i++)
        fillRecInfo(*(char **)vec_getAt(ruids, i), 0);
}

void fillRecAliases(const Vector *statements) {

    // aliases
    for (int i = 0; i < statements->size; i++) {
        genericStatement *stmt = *(genericStatement **)vec_getAt(statements, i);

        if (stmt->tag_stmt_type != STMT_DEFINETYPE)
            continue;

        astStmtDefineType *dt = stmt->stmt;

        astStmtTypeDefinition **td = ht_lookup(rec_defns, &dt->ruid);
        if (td == NULL) {
            printf(RED "Line %d - unknown type %s (definetype error)\n" RESET, dt->line_num, dt->ruid);
            exit(EXIT_FAILURE); // TODO
        }

        ht_insert(&rec_defns, &dt->ruid_as, td);
        vec_pushBack(ruids, &(dt->ruid_as));

        if (dt->tag_rec_or_union == UNION)
            ht_insert(&unions, &dt->ruid_as, td);

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

void fillRecDefns(const Vector *statements) {

    // record definitions
    for (int i = 0; i < statements->size; i++) {
        genericStatement *stmt = *(genericStatement **)vec_getAt(statements, i);

        if (stmt->tag_stmt_type != STMT_TYPEDEFINITION)
            continue;

        astStmtTypeDefinition *td = stmt->stmt;

        ht_insert(&rec_defns, &(td->ruid), &td);
        vec_pushBack(ruids, &(td->ruid));

        if (td->tag_rec_or_union == UNION)
            ht_insert(&unions, &(td->ruid), &td);
    }
}

void fillRecInfo(const char *ruid, const uint32_t line_num) {

    // already filled
    if (ht_lookup(rinfo, &ruid) != NULL)
        return;

    uint32_t        width  = 0;
    uint32_t        max_sz = 0;
    TypeExpression *te     = malloc(sizeof *te);

    *te = (TypeExpression){
        .is_primitive = false,
        .prim_or_rec  = vec_init(sizeof(TypeExpression *), NULL, NULL, VEC_START_SIZE),
    };

    astStmtTypeDefinition **td1 = ht_lookup(rec_defns, &ruid);
    if (td1 == NULL) {
        printf(RED "\nLine: %d Unknown type %s" RESET "\n", line_num, ruid);
        exit(EXIT_FAILURE); // TODO
    }
    astStmtTypeDefinition *td = *td1;
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

        fillRecInfo(type, field->line_num);
        RecordInfoEntry *entry = *(RecordInfoEntry **)ht_lookup(rinfo, &type);
        TypeExpression  *terec = entry->type_expression;
        width += entry->width;
        if (entry->width > max_sz)
            max_sz = entry->width;
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

    // if union
    void **lookup = ht_lookup(unions, &ruid);
    if (lookup != NULL) {
        width      = max_sz;
        te         = NULL;
        *type_name = NULL;
    }

    *entry = (RecordInfoEntry){
        .width           = width,
        .type_expression = te,
        .type_name       = *type_name,
        .ruid            = str_dup(ruid),
    };

    ht_insert(&rinfo, &ruid, &entry);
}

SymbolTable *createSymbolTable(char *scope) {
    SymbolTable *st = malloc(sizeof *st);

    *st = (SymbolTable){
        .scope       = str_dup(scope),
        .total_width = 0,
        .table = ht_init(sizeof(char *), sizeof(SymbolTableEntry *), (ht_hash)ht_polyRollingHash, (ht_kcopy)str_cpyctr,
                         NULL, (ht_kdtr)str_dtr, (ht_vdtr)ste_dtr, (ht_kequal)str_equal, HT_START_SIZE),
    };

    return st;
}

void constructSymbolTables(const AST ast, const bool print_all, const bool print_entry) {
    symbol_tables = vec_init(sizeof(SymbolTable *), NULL, NULL, VEC_START_SIZE);
    scope_st = ht_init(sizeof(char *), sizeof(SymbolTable *), (ht_hash)ht_polyRollingHash, (ht_kcopy)str_cpyctr, NULL,
                       (ht_kdtr)str_dtr, NULL, (ht_kequal)str_equal, HT_START_SIZE);

    // create the global symbol tables
    SymbolTable *st_global = createSymbolTable("global");
    vec_pushBack(symbol_tables, &st_global);

    // function scoped symbol tables
    SymbolTable *st;
    for (int i = 0; i < ast->otherFunctions->size; i++) {
        astFunction *fn = *(astFunction **)vec_getAt(ast->otherFunctions, i);
        st              = createSymbolTable(fn->functionName);
        vec_pushBack(symbol_tables, &st);
        fillSymbolTable(st_global, fn, true, print_all, print_entry);
        fillSymbolTable(st, fn, false, print_all, print_entry);
        ht_insert(&scope_st, &fn->functionName, &st);
    }

    st = createSymbolTable(ast->mainFunction->functionName);
    vec_pushBack(symbol_tables, &st);
    fillSymbolTable(st_global, ast->mainFunction, true, print_all, print_entry);
    fillSymbolTable(st, ast->mainFunction, false, print_all, print_entry);
    ht_insert(&scope_st, &ast->mainFunction->functionName, &st);
}

SymbolTableEntry *makeSymbolTableEntry(const char *type, const char *id, const bool is_global, const uint32_t offset,
                                       const UsageType usage, const uint32_t line_num, const bool to_print) {

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
        if (to_print)
            printf(RED "Line: %d - unknown type name %s (symbol table construction)\n" RESET, line_num, type);
        ste_dtr(&ste);
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

void fillSymbolTable(SymbolTable *st, const astFunction *fn, const bool global_table, const bool print_all,
                     const bool print_entry) {

    // go through all input parameters
    if (!global_table && fn->inputParams != NULL) {
        for (int i = 0; i < fn->inputParams->size; i++) {
            astID            *astid    = *(astID **)vec_getAt(fn->inputParams, i);
            char             *type     = astid->type;
            char             *id       = astid->id;
            uint32_t          line_num = astid->line_num;
            SymbolTableEntry *ste =
                makeSymbolTableEntry(type, id, false, st->total_width, INPARAM, line_num, print_entry);

            if (ste == NULL)
                continue;

            // check for multiple declarations
            void **lookup = ht_lookup(st->table, &id);
            if (lookup != NULL) {
                ste_dtr(&ste);
                printf(RED "Line %d - multiple declaration of %s\n" RESET, astid->line_num, astid->id);
                continue;
            }

            st->total_width += ste->width;
            ht_insert(&st->table, &id, &ste);
            if (print_entry && print_all)
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
            SymbolTableEntry *ste =
                makeSymbolTableEntry(type, id, false, st->total_width, OUTPARAM, line_num, print_entry);

            if (ste == NULL)
                continue;

            // check for multiple declarations
            void **lookup = ht_lookup(st->table, &id);
            if (lookup != NULL) {
                ste_dtr(&ste);
                printf(RED "Line %d - multiple declaration of %s\n" RESET, astid->line_num, astid->id);
                continue;
            }

            st->total_width += ste->width;
            ht_insert(&st->table, &id, &ste);
            if (print_entry && print_all)
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
        SymbolTableEntry *ste =
            makeSymbolTableEntry(type, id, is_global, st->total_width, LOCAL, line_num, print_entry);

        if (ste == NULL)
            continue;

        // check for multiple declarations
        void **lookup = ht_lookup(st->table, &id);
        if (lookup != NULL) {
            ste_dtr(&ste);
            printf(RED "Line %d - multiple declaration of %s\n" RESET, line_num, id);
            continue;
        }

        st->total_width += ste->width;
        ht_insert(&st->table, &id, &ste);

        if (print_entry) {
            if (!print_all) {
                if (is_global)
                    printSymbolTableEntry(line_num, id, ste, st->scope);
            } else
                printSymbolTableEntry(line_num, id, ste, st->scope);
        }
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

    if (te == NULL) {
        printf("--- ");
        return;
    }

    if (te->is_primitive) {
        printf("%s, ", (char *)te->prim_or_rec);
        return;
    }

    printf("<");

    Vector *rec_exp = te->prim_or_rec;
    for (int i = 0; i < rec_exp->size; i++) {
        TypeExpression *te2 = *(TypeExpression **)vec_getAt(rec_exp, i);
        printTypeExpression(te2);
    }

    printf("\b>, ");
}

void printSymbolTable(const AST ast) {
    printf("\n-------- SYMBOL TABLE ENTRIES --------\n\n");
    printf("%8s %15s %30s %10s %10s %10s %10s \t %s \t %s\n", "LINE_NUM", "NAME", "SCOPE", "WIDTH", "OFFSET", "USAGE",
           "ISGLOBAL", "TYPE NAME", "TYPE EXP");

    initRecordInfo(ast);
    constructSymbolTables(ast, true, true);
}

void printGlobalVars(const AST ast) {
    printf("\n-------- GLOBAL VARIABLES --------\n\n");
    printf("%8s %15s %30s %10s %10s %10s %10s \t %s \t %s\n", "LINE_NUM", "NAME", "SCOPE", "WIDTH", "OFFSET", "USAGE",
           "ISGLOBAL", "TYPE EXP", "TYPE NAME");

    initRecordInfo(ast);
    constructSymbolTables(ast, false, true);
}

void printActivationRecordInfo(const AST ast) {
    initRecordInfo(ast);
    constructSymbolTables(ast, false, false);

    printf("\n------- ACTIVATION RECORD SIZES -------\n\n");

    printf("%30s %5s\n", "FUNCTION", "SIZE");

    // skip global symbol table
    for (int i = 1; i < symbol_tables->size; i++) {
        SymbolTable *st = *(SymbolTable **)vec_getAt(symbol_tables, i);
        printf("%30s %5d\n", st->scope, st->total_width);
    }
}

void printRecordInfo(const AST ast) {
    initRecordInfo(ast);
    constructSymbolTables(ast, false, false);

    printf("\n------- RECORD INFO -------\n\n");

    printf("%15s %5s %10s\n", "NAME", "SIZE", "TYPE");

    for (int i = 0; i < ruids->size; i++) {
        char            *name = *(char **)vec_getAt(ruids, i);
        RecordInfoEntry *rie  = *(RecordInfoEntry **)ht_lookup(rinfo, &name);
        printf("%15s ", name);
        printf("%5d ", rie->width);
        printTypeExpression(rie->type_expression);
        printf("\n");
    }
}

void freeSymbolTable() {
    vec_free(ruids);
    ht_free(rinfo);
    ht_free(rec_defns);
    ht_free(aliases);
    ht_free(unions);

    for (int i = 0; i < symbol_tables->size; i++) {
        SymbolTable *st = *(SymbolTable **)vec_getAt(symbol_tables, i);
        free(st->scope);
        ht_free(st->table);
        free(st);
    }
    vec_free(symbol_tables);
    ht_free(scope_st);
}

void rie_dtr(RecordInfoEntry **rie) { free(*rie); }

void ste_dtr(SymbolTableEntry **st) { free(*st); }

void te_dtr(TypeExpression **te) {

    if ((*te)->is_primitive)
        free((*te)->prim_or_rec);
    else {
        Vector *tes = (*te)->prim_or_rec;
        if (tes != NULL) {
            for (int i = 0; i < tes->size; i++) {
                TypeExpression **te2 = vec_getAt(tes, i);
                te_dtr(te2);
            }
            vec_free(tes);
        }
    }
    free(*te);
}

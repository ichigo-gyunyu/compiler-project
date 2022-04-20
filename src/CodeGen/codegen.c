
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "CodeGen/codegen.h"

#define AE_LEFT  1
#define AE_RIGHT 2

static FILE        *codeasm; // pointer to code.asm
static SymbolTable *st_global;
static SymbolTable *st_main;

static char *rax = "rax";
static char *rbx = "rbx";
// static char *rcx       = "rcx";
static char *rdi        = "rdi";
static char *rsi        = "rsi";
static char *opmov      = "mov\t\t";
static char *oppop      = "pop\t\t";
static char *opxor      = "xor\t\t";
static char *opcmp      = "cmp\t\t";
static char *opjmp      = "jmp\t\t";
static char *opjge      = "jge\t\t";
static char *opjg       = "jg\t\t";
static char *opjle      = "jle\t\t";
static char *opjl       = "jl\t\t";
static char *opje       = "je\t\t";
static char *opjne      = "jne\t\t";
static char *opadd      = "add\t\t";
static char *opsub      = "sub\t\t";
static char *opmul      = "mul\t\t";
static char *opneg      = "neg\t\t";
static char *oppush     = "push\t";
static char *opcall     = "call\t";
static char *cmdscanf   = "scanf";
static char *cmdprintf  = "printf";
static char *informat   = "informat";
static char *outformat  = "outformat";
static char *rinformat  = "rinformat";
static char *routformat = "routformat";

static uint32_t count_if   = 0;
static uint32_t count_else = 0;
static uint32_t count_cond = 0;

void genBoilerPlate();
bool genbss(SymbolTable *st);
void genText();
bool genIns(const Vector *stmts);
void genIORead(const astVar *var);
void genIOWrite(const astVar *var);
void genAssign(const astStmtAssignment *assign);
void genAEint(const astArithmeticExpression *ae, const int left_or_right, const bool subtract);
void genCond(const astStmtConditional *cond);
int  genBE(const astBooleanExpression *be, const bool invert);
int  genBELog(const astBooleanExpressionLogical *be, const bool invert);
int  genBERel(const astBooleanExpressionRelational *be, const bool invert);
void genJmpIfTrue(const RelationalOps op, const char *label);
void genJmpIfFalse(const RelationalOps op, const char *label);
void genIter(const astStmtIterative *iter);

char         *getType(const astID *id);
RelationalOps invertRelopMaybe(const RelationalOps op, const bool invert);
LogicalOps    invertLogopMaybe(const LogicalOps op, const bool invert);

void genCode(const AST ast, const char *asm_file) {

    if ((codeasm = fopen(asm_file, "w")) == NULL)
        exit_msg("Could not open file to write asm");

    // handle only main function for now
    if (ast->otherFunctions->size != 0) {
        printf("Code generation cannot handle multiple functions yet\n");
        return;
    }

    // get the symbol tables
    st_global = *(SymbolTable **)vec_getAt(symbol_tables, 0);
    st_main   = *(SymbolTable **)vec_getAt(symbol_tables, 1);

    genBoilerPlate();

    if (!genbss(st_global))
        return;

    if (!genbss(st_main))
        return;

    fprintf(codeasm, "\n");

    genText();

    if (!genIns(ast->mainFunction->statements))
        return;

    fprintf(codeasm, "\n\tret\n");

    fclose(codeasm);
}

void genBoilerPlate() {
    fprintf(codeasm, "\tglobal main\n");
    fprintf(codeasm, "\textern printf\n");
    fprintf(codeasm, "\textern scanf\n");
    fprintf(codeasm, "\n");

    fprintf(codeasm, "section .data\n");
    fprintf(codeasm, "\t%s: db \"%%ld\",0\n", informat);
    fprintf(codeasm, "\t%s: db \"%%ld\",10,0\n", outformat);
    fprintf(codeasm, "\t%s: db \"%%lf\",0\n", rinformat);
    fprintf(codeasm, "\t%s: db \"%%lf\",10,0\n", routformat);
    fprintf(codeasm, "\n");

    fprintf(codeasm, "section .bss\n");
}

bool genbss(SymbolTable *st) {

    // go through symbol table
    Bucket **table = st->table->table;
    for (int i = 0; i < st->table->capacity; i++) {

        if (table[i] == NULL)
            continue;

        Bucket           *entry = table[i];
        char             *id    = *(char **)entry->key;
        SymbolTableEntry *ste   = *(SymbolTableEntry **)entry->val;

        // avoid record types
        if (ste->type_name != NULL) {
            printf("Code generation cannot handle record types yet\n");
            return false;
        }

        if (ste->width == 2)
            fprintf(codeasm, "\t%s\t%12s 1\n", id, "resq");
        else
            fprintf(codeasm, "\t%s\t%12s 1\n", id, "resq");
    }

    return true;
}

void genText() {
    fprintf(codeasm, "section .text\n");
    fprintf(codeasm, "main:\n");
}

bool genIns(const Vector *stmts) {

    if (stmts == NULL)
        return true;

    for (int i = 0; i < stmts->size; i++) {
        genericStatement *stmt = *(genericStatement **)vec_getAt(stmts, i);

        switch (stmt->tag_stmt_type) {
        case STMT_TYPEDEFINITION:

        case STMT_DEFINETYPE:
            printf("Code generation cannot handle record types yet\n");
            return false;

        case STMT_DECLARATION:
            break;

        case STMT_ASSIGNMENT:
            genAssign(stmt->stmt);
            break;

        case STMT_ITERATIVE:
            genIter(stmt->stmt);
            break;

        case STMT_CONDITIONAL:
            genCond(stmt->stmt);
            break;

        case STMT_IOREAD:
            genIORead(stmt->stmt);
            break;

        case STMT_IOWRITE:
            genIOWrite(stmt->stmt);
            break;

        case STMT_FNCALL:
            printf("Code generation cannot handle multiple functions yet\n");
            return false;

        case STMT_RETURN:
            break;
        }
    }

    return true;
}

void genIORead(const astVar *var) {

    astID *id = ((astSingleOrRecID *)var->var)->id; // assumes not a record id

    fprintf(codeasm, "\t; read\n");

    fprintf(codeasm, "\t%s%s\n", oppush, rbx);
    fprintf(codeasm, "\t%s%s, %s\n", opmov, rdi, informat);
    fprintf(codeasm, "\t%s%s, %s\n", opmov, rsi, id->id);
    fprintf(codeasm, "\t%s%s, %s\n", opxor, rax, rax);
    fprintf(codeasm, "\t%s%s\n", opcall, cmdscanf);
    fprintf(codeasm, "\t%s%s\n", oppop, rbx);

    fprintf(codeasm, "\n");
}

void genIOWrite(const astVar *var) {

    fprintf(codeasm, "\t; write\n");

    if (var->tag_var_or_const == VAR) {

        char *id = ((astSingleOrRecID *)var->var)->id->id; // assumes not a record id

        fprintf(codeasm, "\t%s%s, [%s]\n", opmov, rax, id);
        fprintf(codeasm, "\t%s%s\n", oppush, rbx);
        fprintf(codeasm, "\t%s%s, %s\n", opmov, rdi, outformat);
        fprintf(codeasm, "\t%s%s, %s\n", opmov, rsi, rax);
        fprintf(codeasm, "\t%s%s, %s\n", opxor, rax, rax);
        fprintf(codeasm, "\t%s%s\n", opcall, cmdprintf);
        fprintf(codeasm, "\t%s%s\n", oppop, rbx);
    }

    else if (var->tag_var_or_const == INT) {

        int64_t val = *(int64_t *)var->var;

        fprintf(codeasm, "\t%s%s, %ld\n", opmov, rax, val);
        fprintf(codeasm, "\t%s%s\n", oppush, rbx);
        fprintf(codeasm, "\t%s%s, %s\n", opmov, rdi, outformat);
        fprintf(codeasm, "\t%s%s, %s\n", opmov, rsi, rax);
        fprintf(codeasm, "\t%s%s, %s\n", opxor, rax, rax);
        fprintf(codeasm, "\t%s%s\n", opcall, cmdprintf);
        fprintf(codeasm, "\t%s%s\n", oppop, rbx);
    }

    else {
    }

    fprintf(codeasm, "\n");
}

void genAssign(const astStmtAssignment *assign) {

    fprintf(codeasm, "\t; assign\n");

    astID *lhs  = assign->lhs->id;
    char  *type = getType(lhs);

    if (strcmp(type, "int") == 0) {
        genAEint(assign->rhs, AE_LEFT, false);

        fprintf(codeasm, "\t%s[%s], %s\n", opmov, lhs->id, rax);
    }

    else if (strcmp(type, "real") == 0) {
    }

    fprintf(codeasm, "\n");
}

void genAEint(const astArithmeticExpression *ae, const int left_or_right, const bool subtract) {

    if (ae->isVar) {

        if (ae->var->tag_var_or_const == VAR) {
            char *id = ((astSingleOrRecID *)ae->var->var)->id->id;

            if (left_or_right == AE_LEFT) {
                fprintf(codeasm, "\t%s%s, [%s]\n", opmov, rax, id);
                if (subtract)
                    fprintf(codeasm, "\t%s%s\n", opneg, rax);
            } else {
                fprintf(codeasm, "\t%s%s, [%s]\n", opmov, rbx, id);
                if (subtract)
                    fprintf(codeasm, "\t%s%s\n", opneg, rbx);
            }
        }

        else if (ae->var->tag_var_or_const == INT) {
            int64_t val = *(int64_t *)ae->var->var;

            if (left_or_right == AE_LEFT) {
                fprintf(codeasm, "\t%s%s, %ld\n", opmov, rax, val);
                if (subtract)
                    fprintf(codeasm, "\t%s%s\n", opneg, rax);

            } else {
                fprintf(codeasm, "\t%s%s, %ld\n", opmov, rbx, val);
                if (subtract)
                    fprintf(codeasm, "\t%s%s\n", opneg, rbx);
            }
        }

        return;
    }

    genAEint(ae->lhs, AE_LEFT, false);

    // rax has the result of left subtree, but rax will be used inside
    // the right subtree. so save rax elsewhere
    fprintf(codeasm, "\t%s%s\n", oppush, rax);

    bool sub = false;
    if (ae->rhs->isVar && ae->operator== MINUS)
        sub = true;
    genAEint(ae->rhs, AE_RIGHT, sub);

    // now rbx has the result of right subtree. mov original rax back
    fprintf(codeasm, "\t%s%s\n", oppop, rax);

    char *op;

    switch (ae->operator) {
    case PLUS:
        op = opadd;
        fprintf(codeasm, "\t%s%s, %s\n", op, rax, rbx);
        break;
    case MINUS:
        if (ae->rhs->isVar)
            op = opadd;
        else
            op = opsub;
        fprintf(codeasm, "\t%s%s, %s\n", op, rax, rbx);
        break;
    case MUL:
        op = opmul;
        fprintf(codeasm, "\t%s%s\n", op, rbx);
        break;
    case DIV:
        printf("help! should not be here (div in int ae)\n");
        return;
    }

    if (left_or_right == AE_RIGHT)
        fprintf(codeasm, "\t%s%s, %s\n", opmov, rbx, rax);
}

void genCond(const astStmtConditional *cond) {

    char     tmp[TMP_BUFLEN];
    uint32_t count_if_save   = count_if;
    uint32_t count_else_save = count_else;

    fprintf(codeasm, "\t; cond\n");

    int op = genBE(cond->condition, false);
    snprintf(tmp, TMP_BUFLEN, "e%d", count_else_save);
    genJmpIfFalse(op, tmp);

    snprintf(tmp, TMP_BUFLEN, "i%d", count_if_save);
    count_if++;
    fprintf(codeasm, "%s:\n", tmp);
    fprintf(codeasm, "\t; if part\n");
    genIns(cond->ifStatements);
    snprintf(tmp, TMP_BUFLEN, "c%d", count_cond);
    fprintf(codeasm, "\t%s%s\n", opjmp, tmp);

    snprintf(tmp, TMP_BUFLEN, "e%d", count_else_save);
    count_else++;
    fprintf(codeasm, "%s:\n", tmp);
    fprintf(codeasm, "\t; else part\n");
    genIns(cond->elseStatements);

    fprintf(codeasm, "\n");
    snprintf(tmp, TMP_BUFLEN, "c%d", count_cond++);
    fprintf(codeasm, "%s:\n", tmp);
    fprintf(codeasm, "\t; rem part\n");
}

int genBE(const astBooleanExpression *be, const bool invert) {

    switch (be->tag_be_type) {
    case LOGICAL:
        return genBELog(be->be, invert);
        break;
    case RELATIONAL:
        return genBERel(be->be, invert);
        break;
    case NEGATION:
        return genBE(((astBooleanExpressionNegation *)be->be)->exp, !invert);
        break;
    }

    return -1; // -Wall
}

int genBELog(const astBooleanExpressionLogical *be, const bool invert) {

    LogicalOps op = invertLogopMaybe(be->operator, invert);

    int oprel = genBE(be->lhs, invert);

    // short circuiting
    char iflabel[TMP_BUFLEN];
    snprintf(iflabel, TMP_BUFLEN, "i%d", count_if);
    char ellabel[TMP_BUFLEN];
    snprintf(ellabel, TMP_BUFLEN, "e%d", count_else);
    switch (op) {
    case AND:
        genJmpIfFalse(oprel, ellabel);
        break;
    case OR:
        genJmpIfTrue(oprel, iflabel);
        break;
    }

    oprel = genBE(be->rhs, invert);
    switch (op) {
    case AND:
        genJmpIfFalse(oprel, ellabel);
        break;
    case OR:
        genJmpIfTrue(oprel, iflabel);
        break;
    }

    return -1;
}

int genBERel(const astBooleanExpressionRelational *be, const bool invert) {

    RelationalOps op = invertRelopMaybe(be->operator, invert);

    astVar *lhs = be->lhs;
    astVar *rhs = be->rhs;
    if (lhs->tag_var_or_const == VAR) {
        char *lhsid = ((astSingleOrRecID *)lhs->var)->id->id;

        if (rhs->tag_var_or_const == VAR) {
            char *rhsid = ((astSingleOrRecID *)rhs->var)->id->id;
            fprintf(codeasm, "\t%s%s, [%s]\n", opmov, rax, lhsid);
            fprintf(codeasm, "\t%s%s, [%s]\n", opmov, rbx, rhsid);
        }

        else if (rhs->tag_var_or_const == INT) {
            int64_t rhsid = *(int64_t *)rhs->var;
            fprintf(codeasm, "\t%s%s, [%s]\n", opmov, rax, lhsid);
            fprintf(codeasm, "\t%s%s, %ld\n", opmov, rbx, rhsid);
        }
    }

    else if (lhs->tag_var_or_const == INT) {
        int64_t lhsid = *(int64_t *)lhs->var;

        if (rhs->tag_var_or_const == VAR) {
            char *rhsid = ((astSingleOrRecID *)rhs->var)->id->id;
            fprintf(codeasm, "\t%s%s, %ld\n", opmov, rax, lhsid);
            fprintf(codeasm, "\t%s%s, [%s]\n", opmov, rbx, rhsid);
        }

        else if (rhs->tag_var_or_const == INT) {
            int64_t rhsid = *(int64_t *)rhs->var;
            fprintf(codeasm, "\t%s%s, %ld\n", opmov, rax, lhsid);
            fprintf(codeasm, "\t%s%s, %ld\n", opmov, rbx, rhsid);
        }
    }

    fprintf(codeasm, "\t%s%s, %s\n", opcmp, rax, rbx);
    return op;
}

void genJmpIfTrue(const RelationalOps op, const char *label) { genJmpIfFalse(invertRelopMaybe(op, true), label); }

void genJmpIfFalse(const RelationalOps op, const char *label) {

    switch (op) {
    case LT:
        fprintf(codeasm, "\t%s%s\n\n", opjge, label);
        break;
    case LE:
        fprintf(codeasm, "\t%s%s\n\n", opjg, label);
        break;
    case GT:
        fprintf(codeasm, "\t%s%s\n\n", opjle, label);
        break;
    case GE:
        fprintf(codeasm, "\t%s%s\n\n", opjl, label);
        break;
    case EQ:
        fprintf(codeasm, "\t%s%s\n\n", opjne, label);
        break;
    case NE:
        fprintf(codeasm, "\t%s%s\n\n", opje, label);
        break;
    }
}

void genIter(const astStmtIterative *iter) {

    char     tmp[TMP_BUFLEN];
    uint32_t count_if_save   = count_if;
    uint32_t count_else_save = count_else;
    uint32_t count_cond_save = count_cond;

    fprintf(codeasm, "\t; iter\n");

    snprintf(tmp, TMP_BUFLEN, "c%d", count_cond_save);
    fprintf(codeasm, "%s:\n", tmp);
    fprintf(codeasm, "\t; precondition\n");

    // check condition
    int op = genBE(iter->precondition, false);
    snprintf(tmp, TMP_BUFLEN, "e%d", count_else_save);
    genJmpIfFalse(op, tmp);

    // statements
    snprintf(tmp, TMP_BUFLEN, "i%d", count_if_save);
    fprintf(codeasm, "%s:\n", tmp);
    fprintf(codeasm, "\t; iter stmts\n");
    count_if++, count_cond++;
    genIns(iter->statements);

    // jump back to condition check
    snprintf(tmp, TMP_BUFLEN, "c%d", count_cond_save);
    fprintf(codeasm, "\t%s%s\n\n", opjmp, tmp);

    // after loop stuff
    snprintf(tmp, TMP_BUFLEN, "e%d", count_else_save);
    count_else++;
    fprintf(codeasm, "%s:\n", tmp);
    fprintf(codeasm, "\n");
    fprintf(codeasm, "\t; rem part\n");
}

char *getType(const astID *id) {
    void **lookup = ht_lookup(st_main->table, &id->id);
    if (lookup == NULL) {
        lookup = ht_lookup(st_global->table, &id->id);
        if (lookup == NULL)
            return NULL;
    }

    SymbolTableEntry *ste = *lookup;

    return ste->type_expression->prim_or_rec;
}

RelationalOps invertRelopMaybe(const RelationalOps op, const bool invert) {
    if (!invert)
        return op;

    switch (op) {
    case LT:
        return GE;
    case LE:
        return GT;
    case GT:
        return LE;
    case GE:
        return LT;
    case EQ:
        return NE;
    case NE:
        return EQ;
    }

    return -1; // -Wall
}

LogicalOps invertLogopMaybe(const LogicalOps op, const bool invert) {
    if (!invert)
        return op;

    switch (op) {
    case AND:
        return OR;
    case OR:
        return AND;
    }

    return -1; // -Wall
}

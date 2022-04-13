
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "ast.h"

// clang-format off
#define CHILD1 first_child
#define CHILD2 first_child->next_sibling
#define CHILD3 first_child->next_sibling->next_sibling
#define CHILD4 first_child->next_sibling->next_sibling->next_sibling
#define CHILD5 first_child->next_sibling->next_sibling->next_sibling->next_sibling
#define CHILD6 first_child->next_sibling->next_sibling->next_sibling->next_sibling->next_sibling
#define CHILD7 first_child->next_sibling->next_sibling->next_sibling->next_sibling->next_sibling->next_sibling
#define CHILD8 first_child->next_sibling->next_sibling->next_sibling->next_sibling->next_sibling->next_sibling->next_sibling
// clang-format on

void *constructRecursively(const Nary_tree pt);

void print_astRoot(const AST root);
void print_astFunction(const astFunction *node, const int depth);
void print_astID(const astID *node, const int depth);
void print_genericStatement(const genericStatement *node, const int depth);
void print_astStmtTypeDefinition(const astStmtTypeDefinition *node, const int depth);
void print_astStmtDefineType(const astStmtDefineType *node, const int depth);
void print_astStmtDeclaration(const astStmtDeclaration *node, const int depth);
void print_astStmtAssignment(const astStmtAssignment *node, const int depth);
void print_astStmtIterative(const astStmtIterative *node, const int depth);
void print_astStmtConditional(const astStmtConditional *node, const int depth);
void print_astStmtIORead(const astStmtIORead *node, const int depth);
void print_astStmtIOWrite(const astStmtIOWrite *node, const int depth);
void print_astStmtFnCall(const astStmtFunCall *node, const int depth);
void print_astStmtReturn(const astStmtReturn *node, const int depth);
void print_astFieldID(const astFieldID *node, const int depth);
void print_astSingleOrRecID(const astSingleOrRecID *node, const int depth);
void print_astArithmeticExpression(const astArithmeticExpression *node, const int depth);
void print_astBooleanExpression(const astBooleanExpression *node, const int depth);
void print_astBooleanExpressionLogical(const astBooleanExpressionLogical *node, const int depth);
void print_astBooleanExpressionRelational(const astBooleanExpressionRelational *node, const int depth);
void print_astBooleanExpressionNegation(const astBooleanExpressionNegation *node, const int depth);
void print_astVar(const astVar *node, const int depth);
void print_tabs(const int n);

void mem_astFunction(const astFunction *fn, uint32_t *mem, uint32_t *nodes);
void mem_astID(const astID *id, uint32_t *mem, uint32_t *nodes);
void mem_stmt(const genericStatement *stmt, uint32_t *mem, uint32_t *nodes);
void mem_astFieldID(const astFieldID *fid, uint32_t *mem, uint32_t *nodes);
void mem_astSingleOrRecID(const astSingleOrRecID *sori, uint32_t *mem, uint32_t *nodes);
void mem_astAE(const astArithmeticExpression *ae, uint32_t *mem, uint32_t *nodes);
void mem_astBE(const astBooleanExpression *be, uint32_t *mem, uint32_t *nodes);
void mem_astVar(const astVar *var, uint32_t *mem, uint32_t *nodes);

AST constructAST(const Nary_tree pt) { return (AST)constructRecursively(pt); }

void *constructRecursively(const Nary_tree pt) {
    uint8_t rule_num = pt->rule_no;

    // printf("Rule: %d %s %s\n", rule_num, pt->node_symbol, pt->t_info.lexeme);

    switch (rule_num) {

    // program -> otherFunctions mainFunction
    case 1: {
        astProgram *node = malloc(sizeof *node);

        Vector      *otherFunctions = vec_init(sizeof(astFunction *), NULL, NULL, VEC_START_SIZE);
        TreeNode    *pt_copy        = pt->CHILD1;
        astFunction *fn;

        while ((fn = constructRecursively(pt_copy)) != NULL) {
            vec_pushBack(otherFunctions, &fn);
            pt_copy = pt_copy->CHILD2;
        }

        *node = (astProgram){
            .otherFunctions = otherFunctions,
            .mainFunction   = constructRecursively(pt->CHILD2),
        };

        return node;
    }

    // mainFunction -> TK_MAIN stmts TK_END
    case 2: {
        astFunction *node = malloc(sizeof *node);

        *node = (astFunction){
            .line_num     = pt->CHILD1->t_info.line_no,
            .functionName = str_dup("main"),
            .inputParams  = NULL,
            .outputParams = NULL,
            .statements   = constructRecursively(pt->CHILD2),
        };

        return node;
    }

    // otherFunctions -> function otherFunctions
    case 3:
        return constructRecursively(pt->CHILD1);

    // otherFunctions ->
    case 4:
        return NULL;

    // function -> TK_FUNID input_par output_par TK_SEM stmts TK_END
    case 5: {
        astFunction *node = malloc(sizeof *node);

        *node = (astFunction){
            .line_num     = pt->CHILD1->t_info.line_no,
            .functionName = str_dup(pt->CHILD1->t_info.lexeme),
            .inputParams  = constructRecursively(pt->CHILD2),
            .outputParams = constructRecursively(pt->CHILD3),
            .statements   = constructRecursively(pt->CHILD5),
        };

        return node;
    }

    // input_par -> TK_INPUT TK_PARAMETER TK_LIST TK_SQL parameter_list TK_SQR
    case 6:
        return constructRecursively(pt->CHILD5);

    // output_par -> TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL parameter_list TK_SQR
    case 7:
        return constructRecursively(pt->CHILD5);

    // output_par ->
    case 8:
        return NULL;

    // parameter_list -> dataType TK_ID remaining_list
    case 9: {
        Vector   *param_list = vec_init(sizeof(astID *), NULL, NULL, VEC_START_SIZE);
        TreeNode *pt_copy    = pt;

        while (pt_copy != NULL) {
            astID *node = malloc(sizeof *node);

            *node = (astID){
                .line_num = pt_copy->CHILD2->t_info.line_no,
                .id       = str_dup(pt_copy->CHILD2->t_info.lexeme),
                .type     = constructRecursively(pt->CHILD1),
            };

            vec_pushBack(param_list, &node);

            pt_copy = pt_copy->CHILD3->CHILD2;
        }

        return param_list;
    }

    // dataType -> primitiveDatatype
    case 10:
        return constructRecursively(pt->CHILD1);

    // dataType -> constructedDatatype
    case 11:
        return constructRecursively(pt->CHILD1);

    // primitiveDatatype -> TK_INT
    case 12:
        return str_dup("int");

    // primitiveDatatype -> TK_REAL
    case 13:
        return str_dup("real");

    // constructedDatatype -> TK_RECORD TK_RUID
    case 14:
        return str_dup(pt->CHILD2->t_info.lexeme);

    // constructedDatatype -> TK_UNION TK_RUID
    case 15:
        return str_dup(pt->CHILD2->t_info.lexeme);

    // constructedDatatype -> TK_RUID
    case 16:
        return str_dup(pt->CHILD1->t_info.lexeme);

    // remaining_list -> TK_COMMA parameter_list
    case 17:
        printf("should not be here - 17\n");
        return NULL;

    // remaining_list ->
    case 18:
        printf("should not be here - 18\n");
        return NULL;

    // stmts -> typeDefinitions declarations otherStmts returnStmt
    case 19: {

        // cannot be NULL as returnStmt exists
        Vector           *statements = vec_init(sizeof(genericStatement *), NULL, NULL, VEC_START_SIZE);
        TreeNode         *pt_copy    = pt;
        genericStatement *ele;

        // add type definitions and define types (if any)
        pt_copy = pt->CHILD1;
        while ((ele = constructRecursively(pt_copy)) != NULL) {
            vec_pushBack(statements, &ele);
            pt_copy = pt_copy->CHILD2;
        }

        // add declarations (if any)
        pt_copy = pt->CHILD2;
        while ((ele = constructRecursively(pt_copy)) != NULL) {
            vec_pushBack(statements, &ele);
            pt_copy = pt_copy->CHILD2;
        }

        // add other statements (if any)
        pt_copy = pt->CHILD3;
        while ((ele = constructRecursively(pt_copy)) != NULL) {
            vec_pushBack(statements, &ele);
            pt_copy = pt_copy->CHILD2;
        }

        // add returnStmt
        pt_copy = pt->CHILD4;
        ele     = constructRecursively(pt_copy);
        vec_pushBack(statements, &ele);

        return statements;
    }

    // typeDefinitions -> actualOrRedefined typeDefinitions
    case 20:
        return constructRecursively(pt->CHILD1);

    // typeDefinitions ->
    case 21:
        return NULL;

    // actualOrRedefined -> typeDefinition
    case 22: {
        genericStatement *node = malloc(sizeof *node);

        *node = (genericStatement){
            .tag_stmt_type = STMT_TYPEDEFINITION,
            .stmt          = constructRecursively(pt->CHILD1),
        };

        return node;
    }

    // actualOrRedefined -> definetypestmt
    case 23: {
        genericStatement *node = malloc(sizeof *node);

        *node = (genericStatement){
            .tag_stmt_type = STMT_DEFINETYPE,
            .stmt          = constructRecursively(pt->CHILD1),
        };

        return node;
    }

    // typeDefinition -> TK_RECORD TK_RUID fieldDefinitions TK_ENDRECORD
    case 24: {
        astStmtTypeDefinition *stmt = malloc(sizeof *stmt);

        *stmt = (astStmtTypeDefinition){
            .tag_rec_or_union = RECORD,
            .ruid             = str_dup(pt->CHILD2->t_info.lexeme),
            .fieldDefinitions = constructRecursively(pt->CHILD3),
        };

        return stmt;
    }

    // typeDefinition -> TK_UNION TK_RUID fieldDefinitions TK_ENDUNION
    case 25: {
        astStmtTypeDefinition *stmt = malloc(sizeof *stmt);

        *stmt = (astStmtTypeDefinition){
            .tag_rec_or_union = UNION,
            .ruid             = str_dup(pt->CHILD2->t_info.lexeme),
            .fieldDefinitions = constructRecursively(pt->CHILD3),
        };

        return stmt;
    }

    // fieldDefinitions -> fieldDefinition fieldDefinition moreFields
    case 26: {
        Vector *fieldDefinitions = vec_init(sizeof(astFieldID *), NULL, NULL, VEC_START_SIZE);

        // add first fieldDefinition
        astFieldID *fieldDefinition = constructRecursively(pt->CHILD1);
        vec_pushBack(fieldDefinitions, &fieldDefinition);

        // add second fieldDefinition
        fieldDefinition = constructRecursively(pt->CHILD2);
        vec_pushBack(fieldDefinitions, &fieldDefinition);

        // add more fields (if any)
        TreeNode *pt_copy = pt->CHILD3;
        while ((fieldDefinition = constructRecursively(pt_copy)) != NULL) {
            vec_pushBack(fieldDefinitions, &fieldDefinition);
            pt_copy = pt_copy->CHILD2;
        }

        return fieldDefinitions;
    }

    // fieldDefinition -> TK_TYPE fieldType TK_COLON TK_FIELDID TK_SEM
    case 27: {
        astFieldID *node = malloc(sizeof *node);

        *node = (astFieldID){
            .line_num = pt->CHILD4->t_info.line_no,
            .id       = str_dup(pt->CHILD4->t_info.lexeme),
            .type     = constructRecursively(pt->CHILD2),
        };

        return node;
    }

    // fieldType -> primitiveDatatype
    case 28:
        return constructRecursively(pt->CHILD1);

    // fieldType -> TK_RUID
    case 29:
        return str_dup(pt->CHILD1->t_info.lexeme);

    // moreFields -> fieldDefinition moreFields
    case 30:
        return constructRecursively(pt->CHILD1);

    // moreFields ->
    case 31:
        return NULL;

    // declarations -> declaration declarations
    case 32: {
        genericStatement *node = malloc(sizeof *node);

        *node = (genericStatement){
            .tag_stmt_type = STMT_DECLARATION,
            .stmt          = constructRecursively(pt->CHILD1),
        };

        return node;
    }

    // declarations ->
    case 33:
        return NULL;

    // declaration -> TK_TYPE dataType TK_COLON TK_ID global_or_not TK_SEM
    case 34: {
        astStmtDeclaration *node = malloc(sizeof *node);

        void *tmp_ptr = constructRecursively(pt->CHILD5); // used to check if global

        *node = (astStmtDeclaration){
            .id     = malloc(sizeof(astID)),
            .global = (tmp_ptr) ? true : false,
        };

        if (tmp_ptr)
            free(tmp_ptr);

        *(node->id) = (astID){
            .line_num = pt->CHILD4->t_info.line_no,
            .id       = str_dup(pt->CHILD4->t_info.lexeme),
            .type     = constructRecursively(pt->CHILD2),
        };

        return node;
    }

    // global_or_not -> TK_COLON TK_GLOBAL
    case 35:
        return malloc(1); // tmp_ptr, will be freed immediately

    // global_or_not ->
    case 36:
        return NULL;

    // otherStmts -> stmt otherStmts
    case 37:
        return constructRecursively(pt->CHILD1);

    // otherStmts ->
    case 38:
        return NULL;

    // stmt -> assignmentStmt
    case 39: {
        genericStatement *node = malloc(sizeof *node);

        *node = (genericStatement){
            .tag_stmt_type = STMT_ASSIGNMENT,
            .stmt          = constructRecursively(pt->CHILD1),
        };

        return node;
    }

    // stmt -> iterativeStmt
    case 40: {
        genericStatement *node = malloc(sizeof *node);

        *node = (genericStatement){
            .tag_stmt_type = STMT_ITERATIVE,
            .stmt          = constructRecursively(pt->CHILD1),
        };

        return node;
    }

    // stmt -> conditionalStmt
    case 41: {
        genericStatement *node = malloc(sizeof *node);

        *node = (genericStatement){
            .tag_stmt_type = STMT_CONDITIONAL,
            .stmt          = constructRecursively(pt->CHILD1),
        };

        return node;
    }

    // stmt -> ioStmt
    case 42:
        return constructRecursively(pt->CHILD1);

    // stmt -> funCallStmt
    case 43: {
        genericStatement *node = malloc(sizeof *node);

        *node = (genericStatement){
            .tag_stmt_type = STMT_FNCALL,
            .stmt          = constructRecursively(pt->CHILD1),
        };

        return node;
    }

    // assignmentStmt -> singleOrRecId TK_ASSIGNOP arithmeticExpression TK_SEM
    case 44: {
        astStmtAssignment *stmt = malloc(sizeof *stmt);

        *stmt = (astStmtAssignment){
            .lhs = constructRecursively(pt->CHILD1),
            .rhs = constructRecursively(pt->CHILD3),
        };

        return stmt;
    }

    // singleOrRecId -> TK_ID option_single_constructed
    case 45: {
        astSingleOrRecID *node = malloc(sizeof *node);

        *node = (astSingleOrRecID){
            .id     = malloc(sizeof(astID)),
            .fields = constructRecursively(pt->CHILD2),
        };

        *(node->id) = (astID){
            .line_num = pt->CHILD1->t_info.line_no,
            .id       = str_dup(pt->CHILD1->t_info.lexeme),
        };

        return node;
    }

    // option_single_constructed -> oneExpansion moreExpansions
    case 46: {
        Vector     *fields = vec_init(sizeof(astFieldID *), NULL, NULL, VEC_START_SIZE);
        astFieldID *field  = constructRecursively(pt->CHILD1);
        vec_pushBack(fields, &field);

        // add if moreExpansions
        TreeNode *pt_copy = pt->CHILD2;
        while ((field = constructRecursively(pt_copy)) != NULL) {
            vec_pushBack(fields, &field);
            pt_copy = pt_copy->CHILD2;
        }

        return fields;
    }

    // option_single_constructed ->
    case 47:
        return NULL;

    // oneExpansion -> TK_DOT TK_FIELDID
    case 48: {
        astFieldID *node = malloc(sizeof *node);

        *node = (astFieldID){
            .line_num = pt->CHILD2->t_info.line_no,
            .id       = str_dup(pt->CHILD2->t_info.lexeme),
        };

        return node;
    }

    // moreExpansions -> oneExpansion moreExpansions
    case 49:
        return constructRecursively(pt->CHILD1);

    // moreExpansions ->
    case 50:
        return NULL;

    // funCallStmt -> outputParameters TK_CALL TK_FUNID TK_WITH TK_PARAMETERS inputParameters TK_SEM
    case 51: {
        astStmtFunCall *stmt = malloc(sizeof *stmt);

        *stmt = (astStmtFunCall){
            .outputParams = constructRecursively(pt->CHILD1),
            .functionName = str_dup(pt->CHILD3->t_info.lexeme),
            .inputParams  = constructRecursively(pt->CHILD6),
        };

        return stmt;
    }

    // outputParameters -> TK_SQL idList TK_SQR TK_ASSIGNOP
    case 52: {
        Vector *outputParams = vec_init(sizeof(astID *), NULL, NULL, VEC_START_SIZE);

        TreeNode *pt_copy = pt->CHILD2;
        while (pt_copy != NULL) {
            astID *ele = malloc(sizeof(astID));
            *ele       = (astID){
                      .line_num = pt_copy->CHILD1->t_info.line_no,
                      .id       = str_dup(pt_copy->CHILD1->t_info.lexeme),
            };

            vec_pushBack(outputParams, &ele);
            pt_copy = pt_copy->CHILD2->CHILD2;
        }

        return outputParams;
    }

    // outputParameters ->
    case 53:
        return NULL;

    // inputParameters -> TK_SQL idList TK_SQR
    case 54: {
        Vector *inputParams = vec_init(sizeof(astID *), NULL, NULL, VEC_START_SIZE);

        TreeNode *pt_copy = pt->CHILD2;
        while (pt_copy != NULL) {
            astID *ele = malloc(sizeof(astID));
            *ele       = (astID){
                      .line_num = pt_copy->CHILD1->t_info.line_no,
                      .id       = str_dup(pt_copy->CHILD1->t_info.lexeme),
            };

            vec_pushBack(inputParams, &ele);
            pt_copy = pt_copy->CHILD2->CHILD2;
        }

        return inputParams;
    }

    // iterativeStmt -> TK_WHILE TK_OP booleanExpression TK_CL stmt otherStmts TK_ENDWHILE
    case 55: {
        astStmtIterative *stmt = malloc(sizeof *stmt);

        *stmt = (astStmtIterative){
            .precondition = constructRecursively(pt->CHILD3),
            .statements   = vec_init(sizeof(genericStatement *), NULL, NULL, VEC_START_SIZE),
        };

        // add the first statement
        genericStatement *ele = constructRecursively(pt->CHILD5);
        vec_pushBack(stmt->statements, &ele);

        // add otherStmts (if any)
        TreeNode *pt_copy = pt->CHILD6;
        while ((ele = constructRecursively(pt_copy)) != NULL) {
            vec_pushBack(stmt->statements, &ele);
            pt_copy = pt_copy->CHILD2;
        }

        return stmt;
    }

    // conditionalStmt -> TK_IF TK_OP booleanExpression TK_CL TK_THEN stmt otherStmts elsePart
    case 56: {
        astStmtConditional *stmt = malloc(sizeof *stmt);

        *stmt = (astStmtConditional){
            .condition      = constructRecursively(pt->CHILD3),
            .ifStatements   = vec_init(sizeof(genericStatement *), NULL, NULL, VEC_START_SIZE),
            .elseStatements = constructRecursively(pt->CHILD8),
        };

        // add the first statement
        genericStatement *ele = constructRecursively(pt->CHILD6);
        vec_pushBack(stmt->ifStatements, &ele);

        // add otherStmts (if any)
        TreeNode *pt_copy = pt->CHILD7;
        while ((ele = constructRecursively(pt_copy)) != NULL) {
            vec_pushBack(stmt->ifStatements, &ele);
            pt_copy = pt_copy->CHILD2;
        }

        return stmt;
    }

    // elsePart -> TK_ELSE stmt otherStmts TK_ENDIF
    case 57: {
        Vector *elseStatements = vec_init(sizeof(genericStatement *), NULL, NULL, VEC_START_SIZE);

        // add the first statement
        genericStatement *ele = constructRecursively(pt->CHILD2);
        vec_pushBack(elseStatements, &ele);

        // add otherStmts (if any)
        TreeNode *pt_copy = pt->CHILD3;
        while ((ele = constructRecursively(pt_copy)) != NULL) {
            vec_pushBack(elseStatements, &ele);
            pt_copy = pt_copy->CHILD2;
        }

        return elseStatements;
    }

    // elsePart -> TK_ENDIF
    case 58:
        return NULL;

    // ioStmt -> TK_READ TK_OP var TK_CL TK_SEM
    case 59: {
        genericStatement *stmt = malloc(sizeof *stmt);

        *stmt = (genericStatement){
            .tag_stmt_type = STMT_IOREAD,
            .stmt          = malloc(sizeof(astVar)),
        };

        stmt->stmt = constructRecursively(pt->CHILD3);

        return stmt;
    }

    // ioStmt -> TK_WRITE TK_OP var TK_CL TK_SEM
    case 60: {
        genericStatement *stmt = malloc(sizeof *stmt);

        *stmt = (genericStatement){
            .tag_stmt_type = STMT_IOWRITE,
            .stmt          = malloc(sizeof(astVar)),
        };

        stmt->stmt = constructRecursively(pt->CHILD3);

        return stmt;
    }

    // arithmeticExpression -> term expPrime
    case 61: {
        astArithmeticExpression *lhs = constructRecursively(pt->CHILD1);
        astArithmeticExpression *rhs = constructRecursively(pt->CHILD2);

        if (rhs == NULL)
            return lhs;

        astArithmeticExpression *exp     = malloc(sizeof *exp);
        ArithmeticOps           *operator= constructRecursively(pt->CHILD2->CHILD1);

        *exp = (astArithmeticExpression){
            .isVar   = false,
            .var     = NULL,
            .operator= * operator,
            .lhs     = lhs,
            .rhs     = rhs,
        };

        free(operator);

        return exp;
    }

    // expPrime -> lowPrecedenceOperators term expPrime
    case 62: {
        astArithmeticExpression *lhs = constructRecursively(pt->CHILD2);
        astArithmeticExpression *rhs = constructRecursively(pt->CHILD3);

        if (rhs == NULL)
            return lhs;

        astArithmeticExpression *exp     = malloc(sizeof *exp);
        ArithmeticOps           *operator= constructRecursively(pt->CHILD3->CHILD1);

        *exp = (astArithmeticExpression){
            .isVar   = false,
            .var     = NULL,
            .operator= * operator,
            .lhs     = lhs,
            .rhs     = rhs,
        };

        free(operator);

        return exp;
    }

    // expPrime ->
    case 63:
        return NULL;

    // term -> factor termPrime
    case 64: {
        astArithmeticExpression *lhs = constructRecursively(pt->CHILD1);
        astArithmeticExpression *rhs = constructRecursively(pt->CHILD2);

        if (rhs == NULL)
            return lhs;

        astArithmeticExpression *exp     = malloc(sizeof *exp);
        ArithmeticOps           *operator= constructRecursively(pt->CHILD2->CHILD1);

        *exp = (astArithmeticExpression){
            .isVar   = false,
            .var     = NULL,
            .operator= * operator,
            .lhs     = lhs,
            .rhs     = rhs,
        };

        free(operator);

        return exp;
    }

    // termPrime -> highPrecedenceOperators factor termPrime
    case 65: {
        astArithmeticExpression *lhs = constructRecursively(pt->CHILD2);
        astArithmeticExpression *rhs = constructRecursively(pt->CHILD3);

        if (rhs == NULL)
            return lhs;

        astArithmeticExpression *exp     = malloc(sizeof *exp);
        ArithmeticOps           *operator= constructRecursively(pt->CHILD3->CHILD1);

        *exp = (astArithmeticExpression){
            .isVar   = false,
            .var     = NULL,
            .operator= * operator,
            .lhs     = lhs,
            .rhs     = rhs,
        };

        free(operator);

        return exp;
    }

    // termPrime ->
    case 66:
        return NULL;

    // factor -> TK_OP arithmeticExpression TK_CL
    case 67:
        return constructRecursively(pt->CHILD2);

    // factor -> var
    case 68: {
        astArithmeticExpression *exp = malloc(sizeof *exp);

        *exp = (astArithmeticExpression){
            .isVar = true,
            .var   = constructRecursively(pt->CHILD1),
        };

        return exp;
    }

    // highPrecedenceOperators -> TK_MUL
    case 69: {
        ArithmeticOps *op = malloc(sizeof *op);
        *op               = MUL;
        return op;
    }

    // highPrecedenceOperators->TK_DIV
    case 70: {
        ArithmeticOps *op = malloc(sizeof *op);
        *op               = DIV;
        return op;
    }

    // lowPrecedenceOperators->TK_PLUS
    case 71: {
        ArithmeticOps *op = malloc(sizeof *op);
        *op               = PLUS;
        return op;
    }

    // lowPrecedenceOperators->TK_MINUS
    case 72: {
        ArithmeticOps *op = malloc(sizeof *op);
        *op               = MINUS;
        return op;
    }

    // booleanExpression -> TK_OP booleanExpression TK_CL logicalOp TK_OP booleanExpression TK_CL
    case 73: {
        astBooleanExpression *node = malloc(sizeof *node);

        *node = (astBooleanExpression){
            .tag_be_type = LOGICAL,
            .be          = malloc(sizeof(astBooleanExpressionLogical)),
        };

        LogicalOps *operator= constructRecursively(pt->CHILD4);

        *((astBooleanExpressionLogical *)node->be) = (astBooleanExpressionLogical){
            .operator= * operator,
            .lhs     = constructRecursively(pt->CHILD2),
            .rhs     = constructRecursively(pt->CHILD6),
        };

        free(operator);

        return node;
    }

    // booleanExpression -> var relationalOp var
    case 74: {
        astBooleanExpression *node = malloc(sizeof *node);

        *node = (astBooleanExpression){
            .tag_be_type = RELATIONAL,
            .be          = malloc(sizeof(astBooleanExpressionRelational)),
        };

        RelationalOps *operator= constructRecursively(pt->CHILD2);

        *((astBooleanExpressionRelational *)node->be) = (astBooleanExpressionRelational){
            .operator= * operator,
            .lhs     = constructRecursively(pt->CHILD1),
            .rhs     = constructRecursively(pt->CHILD3),
        };

        free(operator);

        return node;
    }

    // booleanExpression -> TK_NOT TK_OP booleanExpression TK_CL
    case 75: {
        astBooleanExpression *node = malloc(sizeof *node);

        *node = (astBooleanExpression){
            .tag_be_type = NEGATION,
            .be          = malloc(sizeof(astBooleanExpressionNegation)),
        };

        *((astBooleanExpressionNegation *)node->be) = (astBooleanExpressionNegation){
            .exp = constructRecursively(pt->CHILD3),
        };

        return node;
    }

    // var -> singleOrRecId
    case 76: {
        astVar *node = malloc(sizeof *node);

        *node = (astVar){
            .tag_var_or_const = VAR,
            .var              = constructRecursively(pt->CHILD1),
        };

        return node;
    }

    // var -> TK_NUM
    case 77: {
        astVar *node = malloc(sizeof *node);

        *node = (astVar){
            .tag_var_or_const = INT,
            .var              = malloc(sizeof(int64_t)),
        };

        *((int64_t *)node->var) = pt->CHILD1->t_info.val.val_int;

        return node;
    }

    // var -> TK_RNUM
    case 78: {
        astVar *node = malloc(sizeof *node);

        *node = (astVar){
            .tag_var_or_const = REAL,
            .var              = malloc(sizeof(double)),
        };

        *((double *)node->var) = pt->CHILD1->t_info.val.val_real;

        return node;
    }

    // logicalOp -> TK_AND
    case 79: {
        LogicalOps *ops = malloc(sizeof *ops);
        *ops            = AND;
        return ops;
    }

    // logicalOp -> TK_OR
    case 80: {
        LogicalOps *ops = malloc(sizeof *ops);
        *ops            = OR;
        return ops;
    }

    // relationalOp -> TK_LT
    case 81: {
        RelationalOps *ops = malloc(sizeof *ops);
        *ops               = LT;
        return ops;
    }

    // relationalOp -> TK_LE
    case 82: {
        RelationalOps *ops = malloc(sizeof *ops);
        *ops               = LE;
        return ops;
    }

    // relationalOp -> TK_EQ
    case 83: {
        RelationalOps *ops = malloc(sizeof *ops);
        *ops               = EQ;
        return ops;
    }

    // relationalOp -> TK_GT
    case 84: {
        RelationalOps *ops = malloc(sizeof *ops);
        *ops               = GT;
        return ops;
    }

    // relationalOp -> TK_GE
    case 85: {
        RelationalOps *ops = malloc(sizeof *ops);
        *ops               = GE;
        return ops;
    }

    // relationalOp -> TK_NE
    case 86: {
        RelationalOps *ops = malloc(sizeof *ops);
        *ops               = NE;
        return ops;
    }

    // returnStmt -> TK_RETURN optionalReturn TK_SEM
    case 87: {
        genericStatement *stmt = malloc(sizeof *stmt);

        *stmt = (genericStatement){
            .tag_stmt_type = STMT_RETURN,
            .stmt          = malloc(sizeof(astStmtReturn)),
        };

        *((astStmtReturn *)stmt->stmt) = (astStmtReturn){
            .returnList = constructRecursively(pt->CHILD2),
        };

        return stmt;
    }

    // optionalReturn -> TK_SQL idList TK_SQR
    case 88: {
        Vector *returnList = vec_init(sizeof(astID *), NULL, NULL, VEC_START_SIZE);

        // iteratively add return list IDs
        TreeNode *pt_copy = pt->CHILD2;
        while (pt_copy != NULL) {
            astID *ele = malloc(sizeof(astID));
            *ele       = (astID){
                      .line_num = pt_copy->CHILD1->t_info.line_no,
                      .id       = str_dup(pt_copy->CHILD1->t_info.lexeme),
            };

            vec_pushBack(returnList, &ele);
            pt_copy = pt_copy->CHILD2->CHILD2;
        }

        return returnList;
    }

    // optionalReturn ->
    case 89:
        return NULL;

    // idList -> TK_ID more_ids
    case 90: {
        astID *node = malloc(sizeof *node);

        *node = (astID){
            .line_num = pt->CHILD1->t_info.line_no,
            .id       = str_dup(pt->CHILD1->t_info.lexeme),
        };

        return node;
    }

    // more_ids -> TK_COMMA idList
    case 91:
        printf("should not be here - 91\n");
        return NULL;

    // more_ids ->
    case 92:
        printf("should not be here - 92\n");
        return NULL;

    // definetypestmt -> TK_DEFINETYPE recOrUnion TK_RUID TK_AS TK_RUID
    case 93: {
        astStmtDefineType *node = malloc(sizeof *node);

        RecOrUnion *ru = constructRecursively(pt->CHILD2);

        *node = (astStmtDefineType){
            .tag_rec_or_union = *ru,
            .ruid             = str_dup(pt->CHILD3->t_info.lexeme),
            .ruid_as          = str_dup(pt->CHILD5->t_info.lexeme),
        };

        free(ru);

        return node;
    }

    // recOrUnion -> TK_RECORD
    case 94: {
        RecOrUnion *ru = malloc(sizeof *ru);
        *ru            = RECORD;
        return ru;
    }

    // recOrUnion -> TK_UNION
    case 95: {
        RecOrUnion *ru = malloc(sizeof *ru);
        *ru            = UNION;
        return ru;
    }

    default:
        printf("Incorrect grammar rule in AST construction\n");
        return NULL; // should not be here
    }
}

void print_astRoot(const AST root) {

    printf("\n------------ AST ------------\n");
    printf(BLU "Traversal Order: top down, breadth first\n\n" RESET);

    Vector *otherFunctions = root->otherFunctions;
    for (int i = 0; i < otherFunctions->size; i++) {
        print_astFunction(*(astFunction **)vec_getAt(otherFunctions, i), 0);
    }

    print_astFunction(root->mainFunction, 0);

    printf("\n");
}

void print_astFunction(const astFunction *node, const int depth) {

    printf("[astFn] ");
    printf("name: %s\n", node->functionName);

    printf("\n\t(inputParams)\n");
    Vector *inputParams = node->inputParams;
    if (inputParams) {
        for (int i = 0; i < inputParams->size; i++) {
            print_astID(*(astID **)vec_getAt(inputParams, i), 1);
        }
    }

    printf("\n\t(outputParams)\n");
    Vector *outputParams = node->outputParams;
    if (outputParams) {
        for (int i = 0; i < outputParams->size; i++) {
            print_astID(*(astID **)vec_getAt(outputParams, i), 1);
        }
    }

    printf("\n\t(statements)\n");
    Vector *statements = node->statements;
    for (int i = 0; i < statements->size; i++) {
        print_genericStatement(*(genericStatement **)vec_getAt(statements, i), 1);
    }

    printf("\n");
}

void print_astID(const astID *node, const int depth) {
    print_tabs(depth);

    printf("[astID] ");
    printf("id: %s\n", node->id);
}

void print_genericStatement(const genericStatement *node, const int depth) {

    switch (node->tag_stmt_type) {
    case STMT_TYPEDEFINITION:
        print_astStmtTypeDefinition(node->stmt, depth);
        break;
    case STMT_DEFINETYPE:
        print_astStmtDefineType(node->stmt, depth);
        break;
    case STMT_DECLARATION:
        print_astStmtDeclaration(node->stmt, depth);
        break;
    case STMT_ASSIGNMENT:
        print_astStmtAssignment(node->stmt, depth);
        break;
    case STMT_ITERATIVE:
        print_astStmtIterative(node->stmt, depth);
        break;
    case STMT_CONDITIONAL:
        print_astStmtConditional(node->stmt, depth);
        break;
    case STMT_IOREAD:
        print_astStmtIORead(node->stmt, depth);
        break;
    case STMT_IOWRITE:
        print_astStmtIOWrite(node->stmt, depth);
        break;
    case STMT_FNCALL:
        print_astStmtFnCall(node->stmt, depth);
        break;
    case STMT_RETURN:
        print_astStmtReturn(node->stmt, depth);
    }

    printf("\n");
}

void print_astStmtTypeDefinition(const astStmtTypeDefinition *node, const int depth) {

    print_tabs(depth);

    printf("[astTypeDefinition] ");

    switch (node->tag_rec_or_union) {
    case RECORD:
        printf("r/u: RECORD ");
        break;
    case UNION:
        printf("r/u: UNION ");
        break;
    }

    printf("\n");

    Vector *fieldDefinitions = node->fieldDefinitions;
    for (int i = 0; i < fieldDefinitions->size; i++) {
        print_astFieldID(*(astFieldID **)vec_getAt(fieldDefinitions, i), depth + 1);
    }

    printf("\n");
}

void print_astStmtDefineType(const astStmtDefineType *node, const int depth) {
    print_tabs(depth);

    printf("[astDefineType] ");

    switch (node->tag_rec_or_union) {
    case RECORD:
        printf("r/u: RECORD ");
        break;
    case UNION:
        printf("r/u: UNION ");
        break;
    }

    printf("\n");
}

void print_astStmtDeclaration(const astStmtDeclaration *node, const int depth) {
    print_tabs(depth);

    printf("[astDeclaration] ");

    printf("global: ");
    if (node->global)
        printf("yes ");
    else
        printf("no ");

    print_astID(node->id, 0);
}

void print_astStmtAssignment(const astStmtAssignment *node, const int depth) {
    print_tabs(depth);

    printf("[astAssignment] ");

    printf("operator: <---\n");

    print_astSingleOrRecID(node->lhs, depth + 1);

    print_astArithmeticExpression(node->rhs, depth + 1);
}

void print_astStmtIterative(const astStmtIterative *node, const int depth) {
    print_tabs(depth);

    printf("[astIterative]\n");

    print_astBooleanExpression(node->precondition, depth + 1);

    printf("\n");

    Vector *statements = node->statements;
    for (int i = 0; i < statements->size; i++) {
        print_genericStatement(*(genericStatement **)vec_getAt(statements, i), depth + 1);
    }

    printf("\n");
}

void print_astStmtConditional(const astStmtConditional *node, const int depth) {
    print_tabs(depth);
    printf("[astConditional]\n");

    print_astBooleanExpression(node->condition, depth + 1);

    Vector *ifStatements = node->ifStatements;
    printf("\n");
    print_tabs(depth + 1);
    printf("(if statements)\n");
    for (int i = 0; i < ifStatements->size; i++) {
        print_genericStatement(*(genericStatement **)vec_getAt(ifStatements, i), depth + 1);
    }

    Vector *elseStatements = node->elseStatements;
    if (elseStatements == NULL)
        return;

    printf("\n");
    print_tabs(depth + 1);
    printf("(else statements)\n");
    for (int i = 0; i < elseStatements->size; i++) {
        print_genericStatement(*(genericStatement **)vec_getAt(elseStatements, i), depth + 1);
    }

    printf("\n");
}

void print_astStmtIORead(const astStmtIORead *node, const int depth) {
    print_tabs(depth);

    printf("[astIORead]\n");
    print_astVar(node, depth + 1);
}

void print_astStmtIOWrite(const astStmtIOWrite *node, const int depth) {
    print_tabs(depth);

    printf("[astIOWrite]\n");
    print_astVar(node, depth + 1);
}

void print_astStmtFnCall(const astStmtFunCall *node, const int depth) {
    print_tabs(depth);

    printf("[astFnCall] ");
    printf("name: %s\n", node->functionName);

    Vector *outputParams = node->outputParams;
    if (outputParams != NULL) {
        print_tabs(depth + 1);
        printf("outputParams:\n");
        for (int i = 0; i < outputParams->size; i++) {
            print_astID(*(astID **)vec_getAt(outputParams, i), depth + 1);
        }
    }

    print_tabs(depth + 1);
    printf("inputParams:\n");
    Vector *inputParams = node->inputParams;
    for (int i = 0; i < inputParams->size; i++) {
        print_astID(*(astID **)vec_getAt(inputParams, i), depth + 1);
    }
}

void print_astStmtReturn(const astStmtReturn *node, const int depth) {
    print_tabs(depth);

    printf("[astReturn]\n");

    Vector *returnList = node->returnList;
    if (returnList == NULL)
        return;

    for (int i = 0; i < returnList->size; i++) {
        print_astID(*(astID **)vec_getAt(returnList, i), depth + 1);
    }
}

void print_astFieldID(const astFieldID *node, const int depth) {
    print_tabs(depth);

    printf("[astField] ");
    printf("id: %s\n", node->id);
}

void print_astSingleOrRecID(const astSingleOrRecID *node, const int depth) {

    print_tabs(depth);

    Vector *fields = node->fields;

    if (fields == NULL) {
        print_astID(node->id, 0);
        return;
    }

    printf("[astRecID]\n");
    print_astID(node->id, depth);
    for (int i = 0; i < fields->size; i++) {
        print_astFieldID(*(astFieldID **)vec_getAt(fields, i), depth + 1);
    }

    printf("\n");
}

void print_astArithmeticExpression(const astArithmeticExpression *node, const int depth) {
    if (node->isVar) {
        print_astVar(node->var, depth);
        return;
    }

    print_tabs(depth);

    printf("[astAE] ");
    printf("operator: ");
    switch (node->operator) {
    case PLUS:
        printf("+ ");
        break;
    case MINUS:
        printf("- ");
        break;
    case MUL:
        printf("* ");
        break;
    case DIV:
        printf("/ ");
        break;
    }

    printf("\n");

    print_astArithmeticExpression(node->lhs, depth + 1);
    print_astArithmeticExpression(node->rhs, depth + 1);
}

void print_astBooleanExpression(const astBooleanExpression *node, const int depth) {

    switch (node->tag_be_type) {
    case LOGICAL:
        print_astBooleanExpressionLogical(node->be, depth);
        break;
    case RELATIONAL:
        print_astBooleanExpressionRelational(node->be, depth);
        break;
    case NEGATION:
        print_astBooleanExpressionNegation(node->be, depth);
        break;
    }
}

void print_astBooleanExpressionLogical(const astBooleanExpressionLogical *node, const int depth) {
    print_tabs(depth);

    printf("[astBE] ");
    printf("operator: ");

    switch (node->operator) {
    case AND:
        printf("&&& ");
        break;
    case OR:
        printf("@@@ ");
        break;
    }

    printf("\n");

    print_astBooleanExpression(node->lhs, depth + 1);
    print_astBooleanExpression(node->rhs, depth + 1);
}

void print_astBooleanExpressionRelational(const astBooleanExpressionRelational *node, const int depth) {
    print_tabs(depth);

    printf("[astBE] ");
    printf("operator: ");

    switch (node->operator) {
    case LT:
        printf("< ");
        break;
    case LE:
        printf("<= ");
        break;
    case GT:
        printf("> ");
        break;
    case GE:
        printf(">= ");
        break;
    case EQ:
        printf("== ");
        break;
    case NE:
        printf("!= ");
        break;
    }

    printf("\n");

    print_astVar(node->lhs, depth + 1);
    print_astVar(node->rhs, depth + 1);
}

void print_astBooleanExpressionNegation(const astBooleanExpressionNegation *node, const int depth) {
    print_tabs(depth);

    printf("[astBE] ");
    printf("operator: ~\n");

    print_astBooleanExpression(node->exp, depth + 1);
}

void print_astVar(const astVar *node, const int depth) {
    print_tabs(depth);

    printf("[astVar] ");

    switch (node->tag_var_or_const) {
    case VAR:
        printf("\n");
        print_astSingleOrRecID(node->var, depth + 1);
        break;
    case INT:
        printf("val: %ld\n", *((int64_t *)node->var));
        break;
    case REAL:
        printf("val: %f\n", *((double *)node->var));
    }
}

void print_tabs(const int n) {
    for (int i = 0; i < n; i++)
        printf("\t");
}

void computeMemoryAllocated(const AST ast, uint32_t *mem, uint32_t *nodes) {

    // root node
    *mem += sizeof(astProgram);

    // other functions
    for (int i = 0; i < ast->otherFunctions->size; i++) {
        astFunction *fn = *(astFunction **)vec_getAt(ast->otherFunctions, i);
        mem_astFunction(fn, mem, nodes);
    }

    // main function
    mem_astFunction(ast->mainFunction, mem, nodes);
}

void mem_astFunction(const astFunction *fn, uint32_t *mem, uint32_t *nodes) {

    *mem += sizeof(astFunction);
    *nodes += 1;

    if (fn->inputParams != NULL) {
        for (int i = 0; i < fn->inputParams->size; i++) {
            astID *id = *(astID **)vec_getAt(fn->inputParams, i);
            mem_astID(id, mem, nodes);
        }
    }

    if (fn->outputParams != NULL) {
        for (int i = 0; i < fn->outputParams->size; i++) {
            astID *id = *(astID **)vec_getAt(fn->outputParams, i);
            mem_astID(id, mem, nodes);
        }
    }

    for (int i = 0; i < fn->statements->size; i++) {
        genericStatement *stmt = *(genericStatement **)vec_getAt(fn->statements, i);
        mem_stmt(stmt, mem, nodes);
    }
}

void mem_astID(const astID *id, uint32_t *mem, uint32_t *nodes) {

    *mem += sizeof(astID);
    *nodes += 1;
}

void mem_stmt(const genericStatement *stmt, uint32_t *mem, uint32_t *nodes) {

    switch (stmt->tag_stmt_type) {

    case STMT_TYPEDEFINITION: {
        *mem += sizeof(astStmtTypeDefinition);
        *nodes += 1;

        astStmtTypeDefinition *td = stmt->stmt;

        for (int i = 0; i < td->fieldDefinitions->size; i++) {
            astFieldID *fid = *(astFieldID **)vec_getAt(td->fieldDefinitions, i);
            mem_astFieldID(fid, mem, nodes);
        }

        break;
    }

    case STMT_DEFINETYPE: {

        *mem += sizeof(astStmtDefineType);
        *nodes += 1;

        break;
    }

    case STMT_DECLARATION: {
        *mem += sizeof(astStmtDeclaration);
        *nodes += 1;

        astStmtDeclaration *dec = stmt->stmt;

        mem_astID(dec->id, mem, nodes);

        break;
    }

    case STMT_ASSIGNMENT: {
        *mem += sizeof(astStmtAssignment);
        *nodes += 1;

        astStmtAssignment *assign = stmt->stmt;

        mem_astSingleOrRecID(assign->lhs, mem, nodes);
        mem_astAE(assign->rhs, mem, nodes);

        break;
    }

    case STMT_ITERATIVE: {
        *mem += sizeof(astStmtIterative);
        *nodes += 1;

        astStmtIterative *it = stmt->stmt;
        mem_astBE(it->precondition, mem, nodes);
        for (int i = 0; i < it->statements->size; i++) {
            genericStatement *stmt = *(genericStatement **)vec_getAt(it->statements, i);
            mem_stmt(stmt, mem, nodes);
        }

        break;
    }

    case STMT_CONDITIONAL: {
        *mem += sizeof(astStmtConditional);
        *nodes += 1;

        astStmtConditional *cond = stmt->stmt;
        mem_astBE(cond->condition, mem, nodes);

        for (int i = 0; i < cond->ifStatements->size; i++) {
            genericStatement *stmt = *(genericStatement **)vec_getAt(cond->ifStatements, i);
            mem_stmt(stmt, mem, nodes);
        }

        if (cond->elseStatements) {
            for (int i = 0; i < cond->elseStatements->size; i++) {
                genericStatement *stmt = *(genericStatement **)vec_getAt(cond->ifStatements, i);
                mem_stmt(stmt, mem, nodes);
            }
        }

        break;
    }

    case STMT_IOREAD: {
        astVar *var = stmt->stmt;
        mem_astVar(var, mem, nodes);

        break;
    }

    case STMT_IOWRITE: {

        astVar *var = stmt->stmt;
        mem_astVar(var, mem, nodes);

        break;
    }
    case STMT_FNCALL: {
        *mem += sizeof(astFunction);
        *nodes += 1;

        astStmtFunCall *fncall = stmt->stmt;

        if (fncall->outputParams != NULL) {
            for (int i = 0; i < fncall->outputParams->size; i++) {
                astID *id = *(astID **)vec_getAt(fncall->outputParams, i);
                mem_astID(id, mem, nodes);
            }
        }

        if (fncall->inputParams != NULL) {
            for (int i = 0; i < fncall->inputParams->size; i++) {
                astID *id = *(astID **)vec_getAt(fncall->inputParams, i);
                mem_astID(id, mem, nodes);
            }
        }

        break;
    }
    case STMT_RETURN: {

        *mem += sizeof(astStmtReturn);
        *nodes += 1;

        astStmtReturn *ret = stmt->stmt;

        if (ret->returnList != NULL) {
            for (int i = 0; i < ret->returnList->size; i++) {
                astID *id = *(astID **)vec_getAt(ret->returnList, i);
                mem_astID(id, mem, nodes);
            }
        }

        break;
    }
    }
}

void mem_astFieldID(const astFieldID *fid, uint32_t *mem, uint32_t *nodes) {
    *mem += sizeof(astFieldID);
    *nodes += 1;
}

void mem_astSingleOrRecID(const astSingleOrRecID *sori, uint32_t *mem, uint32_t *nodes) {

    *mem += sizeof(astSingleOrRecID);
    *nodes += 1;

    if (sori->fields != NULL) {
        for (int i = 0; i < sori->fields->size; i++) {
            astFieldID *fid = *(astFieldID **)vec_getAt(sori->fields, i);
            mem_astFieldID(fid, mem, nodes);
        }
    }
}

void mem_astAE(const astArithmeticExpression *ae, uint32_t *mem, uint32_t *nodes) {

    *mem += sizeof(astArithmeticExpression);
    *nodes += 1;

    if (ae->isVar) {
        mem_astVar(ae->var, mem, nodes);
        return;
    }

    mem_astAE(ae->lhs, mem, nodes);
    mem_astAE(ae->rhs, mem, nodes);
}

void mem_astBE(const astBooleanExpression *be, uint32_t *mem, uint32_t *nodes) {

    *mem += sizeof(astArithmeticExpression);
    *nodes += 1;

    switch (be->tag_be_type) {
    case LOGICAL: {
        astBooleanExpressionLogical *bel = be->be;
        mem_astBE(bel->lhs, mem, nodes);
        mem_astBE(bel->rhs, mem, nodes);

        break;
    }

    case RELATIONAL: {
        astBooleanExpressionRelational *ber = be->be;
        mem_astVar(ber->lhs, mem, nodes);
        mem_astVar(ber->rhs, mem, nodes);

        break;
    }

    case NEGATION: {
        astBooleanExpressionNegation *ben = be->be;
        mem_astBE(ben->exp, mem, nodes);

        break;
    }
    }
}

void mem_astVar(const astVar *var, uint32_t *mem, uint32_t *nodes) {

    *mem += sizeof(astVar);
    *nodes += 1;

    if (var->tag_var_or_const == VAR)
        mem_astSingleOrRecID((astSingleOrRecID *)var->var, mem, nodes);
}


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

AST constructAST(const Nary_tree pt) { return (AST)constructRecursively(pt); }

void *constructRecursively(const Nary_tree pt) {
    uint8_t rule_num = pt->rule_no;

    switch (rule_num) {

    // program -> otherFunctions mainFunction
    case 1: {
        astProgram *node = malloc(sizeof *node);

        *node = (astProgram){
            .otherFunctions = constructRecursively(pt->CHILD1),
            .mainFunction   = constructRecursively(pt->CHILD2),
        };

        return node;
    }

    // mainFunction -> TK_MAIN stmts TK_END
    case 2: {
        astFunction *node = malloc(sizeof *node);

        *node = (astFunction){
            .functionName = str_dup("main"),
            .inputParams  = NULL,
            .outputParams = NULL,
            .statements   = constructRecursively(pt->CHILD2),
        };

        return node;
    }

    // otherFunctions -> function otherFunctions
    case 3: {
        Vector      *functions = vec_init(sizeof(astFunction *), NULL, NULL, VEC_START_SIZE);
        astFunction *node;
        TreeNode    *pt_copy = pt;

        while ((node = constructRecursively(pt_copy->CHILD1)) != NULL) {
            vec_pushBack(functions, &node);
            pt_copy = pt_copy->CHILD2;
        }

        return functions;
    }

    // otherFunctions ->
    case 4:
        return NULL;

    // function -> TK_FUNID input_par output_par TK_SEM stmts TK_END
    case 5: {
        astFunction *node = malloc(sizeof *node);

        *node = (astFunction){
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

    // parameter_list -> dataType TK_ID remaining_list TODO REDO
    case 9: {
        Vector   *param_list = vec_init(sizeof(astID *), NULL, NULL, VEC_START_SIZE);
        TreeNode *pt_copy    = pt;

        while (pt_copy != NULL) {
            astID *node = malloc(sizeof *node);

            *node = (astID){
                .type = constructRecursively(pt_copy->CHILD1),
                .id   = str_dup(pt_copy->CHILD2->t_info.lexeme),
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
        // TODO int

    // primitiveDatatype -> TK_REAL
    case 13:
        // TODO real

    // constructedDatatype -> TK_RECORD TK_RUID
    case 14:
        // TODO

    // constructedDatatype -> TK_UNION TK_RUID
    case 15:
        // TODO

    // constructedDatatype -> TK_RUID
    case 16:
        // TODO

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
            .ruid             = 1, // TODO
            .fieldDefinitions = constructRecursively(pt->CHILD3),
        };

        return stmt;
    }

    // typeDefinition -> TK_UNION TK_RUID fieldDefinitions TK_ENDUNION
    case 25: {
        astStmtTypeDefinition *stmt = malloc(sizeof *stmt);

        *stmt = (astStmtTypeDefinition){
            .tag_rec_or_union = UNION,
            .ruid             = 1, // TODO
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
            .type = NULL, // TODO
            .id   = str_dup(pt->CHILD4->t_info.lexeme),
        };

        return node;
    }

    // fieldType -> primitiveDatatype
    case 28:
        return NULL; // TODO

    // fieldType -> TK_RUID
    case 29:
        return NULL; // TODO

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
            .id     = malloc(sizeof(astID *)),
            .global = (tmp_ptr) ? true : false,
        };

        if (tmp_ptr)
            free(tmp_ptr);

        *(node->id) = (astID){
            .type = NULL, // TODO
            .id   = str_dup(pt->CHILD4->t_info.lexeme),
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
            .id     = malloc(sizeof(astID *)),
            .fields = constructRecursively(pt->CHILD2),
        };

        *(node->id) = (astID){
            .type = NULL, // TODO
            .id   = str_dup(pt->CHILD1->t_info.lexeme),
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
            .type = NULL, // TODO
            .id   = str_dup(pt->CHILD2->t_info.lexeme),
        };

        return NULL;
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
    case 52:
        return constructRecursively(pt->CHILD2);

    // outputParameters ->
    case 53:
        return NULL;

    // inputParameters -> TK_SQL idList TK_SQR
    case 54:
        return constructRecursively(pt->CHILD2);

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
            .stmt          = malloc(sizeof(astVar *)),
        };

        stmt->stmt = constructRecursively(pt->CHILD3);
    }

    // ioStmt -> TK_WRITE TK_OP var TK_CL TK_SEM
    case 60: {
        genericStatement *stmt = malloc(sizeof *stmt);

        *stmt = (genericStatement){
            .tag_stmt_type = STMT_IOWRITE,
            .stmt          = malloc(sizeof(astVar *)),
        };

        stmt->stmt = constructRecursively(pt->CHILD3);
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
    case 68:
        return constructRecursively(pt->CHILD1);

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
            .be          = malloc(sizeof(astBooleanExpressionLogical *)),
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
            .be          = malloc(sizeof(astBooleanExpressionRelational *)),
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
            .be          = malloc(sizeof(astBooleanExpressionNegation *)),
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
            .var              = malloc(sizeof(int64_t *)),
        };

        *((int64_t *)node->var) = pt->CHILD1->t_info.val.val_int;

        return node;
    }

    // var -> TK_RNUM
    case 78: {
        astVar *node = malloc(sizeof *node);

        *node = (astVar){
            .tag_var_or_const = REAL,
            .var              = malloc(sizeof(double *)),
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
        astStmtReturn *node = malloc(sizeof *node);

        *node = (astStmtReturn){
            .returnList = constructRecursively(pt->CHILD2),
        };

        return node;
    }

    // optionalReturn -> TK_SQL idList TK_SQR
    case 88: {
        Vector *returnList = vec_init(sizeof(astID *), NULL, NULL, VEC_START_SIZE);

        // iteratively add return list IDs
        TreeNode *pt_copy = pt->CHILD2;
        while (pt_copy != NULL) {
            astID *ele = constructRecursively(pt_copy->CHILD1);
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
            .type = NULL, // TODO
            .id   = str_dup(pt->CHILD1->t_info.lexeme),
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
            .ruid             = 1, // TODO
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

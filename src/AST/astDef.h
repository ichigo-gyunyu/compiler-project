
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#ifndef ASTDEF_H
#define ASTDEF_H

#include "Utils/utils.h"
#include "Utils/vector.h"

typedef struct astProgram                     astProgram;
typedef struct astFunction                    astFunction;
typedef struct astStatements                  astStatements;
typedef struct astStmtTypeDefinition          astStmtTypeDefinition;
typedef struct astFieldID                     astFieldID;
typedef struct astStmtDeclaration             astStmtDeclaration;
typedef struct astStmtAssignment              astStmtAssignment;
typedef struct astID                          astID;
typedef struct astSingleOrRecID               astSingleOrRecID;
typedef struct astStmtIterative               astStmtIterative;
typedef struct astStmtConditional             astStmtConditional;
typedef struct astStmtFunCall                 astStmtFunCall;
typedef struct astStmtDefineType              astStmtDefineType;
typedef struct astStmtReturn                  astStmtReturn;
typedef struct astArithmeticExpression        astArithmeticExpression;
typedef struct astBooleanExpression           astBooleanExpression;
typedef struct astBooleanExpressionLogical    astBooleanExpressionLogical;
typedef struct astBooleanExpressionRelational astBooleanExpressionRelational;
typedef struct astBooleanExpressionNegation   astBooleanExpressionNegation;
typedef struct astVar                         astVar;
typedef struct astVar                         astStmtIORead;
typedef struct astVar                         astStmtIOWrite;

struct astProgram {
    Vector      *otherFunctions; // Vector<astFunction *>
    astFunction *mainFunction;

    void (*print)(astProgram *node);
};

struct astFunction {
    char   *functionName;
    Vector *inputParams;  // Vector<astID *>
    Vector *outputParams; // Vector<astID *>
    Vector *statements;   // Vector<genericStatement *>
};

typedef enum StatementTypeID {
    STMT_TYPEDEFINITION,
    STMT_DEFINETYPE,
    STMT_DECLARATION,
    STMT_ASSIGNMENT,
    STMT_ITERATIVE,
    STMT_CONDITIONAL,
    STMT_IOREAD,
    STMT_IOWRITE,
    STMT_FNCALL,
} StatementTypeID;

typedef struct genericStatement {
    StatementTypeID tag_stmt_type;
    void           *stmt; // any statement type, generic
} genericStatement;

typedef enum RecOrUnion { RECORD, UNION } RecOrUnion;

struct astStmtTypeDefinition {
    RecOrUnion tag_rec_or_union;
    int        ruid;
    Vector    *fieldDefinitions; // Vector<astFieldID *> (size >= 2)
};

struct astStmtDefineType {
    RecOrUnion tag_rec_or_union;
    int        ruid;
};

struct astFieldID {
    void *type; // TODO
    char *id;
};

struct astStmtDeclaration {
    astID *id;
    bool   global;
};

struct astID {
    void *type; // TODO
    char *id;
};

typedef enum VarOrConst { VAR, INT, REAL } VarOrConst;

struct astVar {
    VarOrConst tag_var_or_const;
    void      *var; // astSingleOrRecID *, int64_t * or double *
};

typedef enum ArithmeticOps { PLUS, MINUS, MUL, DIV } ArithmeticOps;

struct astArithmeticExpression {
    bool    isVar;
    astVar *var;

    ArithmeticOps            operator;
    astArithmeticExpression *lhs;
    astArithmeticExpression *rhs;
};

typedef enum BooleanExpressionType { LOGICAL, RELATIONAL, NEGATION } BooleanExpressionType;

struct astBooleanExpression {
    BooleanExpressionType tag_be_type;
    void                 *be;
};

typedef enum LogicalOps { AND, OR } LogicalOps;

struct astBooleanExpressionLogical {
    LogicalOps            operator;
    astBooleanExpression *lhs;
    astBooleanExpression *rhs;
};

typedef enum RelationalOps { LT, LE, GT, GE, EQ, NE } RelationalOps;

struct astBooleanExpressionRelational {
    RelationalOps operator;
    astVar       *lhs;
    astVar       *rhs;
};

struct astBooleanExpressionNegation {
    astBooleanExpression *exp;
};

struct astSingleOrRecID {
    astID  *id;
    Vector *fields; // Vector<astFieldID *> (NULL if not record id)
};

struct astStmtAssignment {
    astSingleOrRecID        *lhs;
    astArithmeticExpression *rhs;
};

struct astStmtFunCall {
    Vector *outputParams; // Vector<astID *>
    char   *functionName;
    Vector *inputParams; // Vector<astID *> (size >= 1)
};

struct astStmtIterative {
    astBooleanExpression *precondition;
    Vector               *statements; // Vector<genericStatement *>
};

struct astStmtConditional {
    astBooleanExpression *condition;
    Vector               *ifStatements;   // Vector<genericStatement *>
    Vector               *elseStatements; // Vector<genericStatement *> (can be NULL)
};

struct astStmtReturn {
    Vector *returnList; // Vector<astID *> (can be NULL)
};

typedef astProgram *AST;

#endif // ASTDEF_H

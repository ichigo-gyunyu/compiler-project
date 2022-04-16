
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#ifndef SYMBOLTABLEDEF_H
#define SYMBOLTABLEDEF_H

#include "Utils/hashtable.h"
#include "Utils/utils.h"
#include "Utils/vector.h"

#define WIDTH_INT  2
#define WIDTH_REAL 4

typedef char   *PrimitiveType;
typedef Vector *RecordType; // Vector<TypeExpression *>

typedef struct TypeExpression {
    bool  is_primitive;
    void *prim_or_rec;
} TypeExpression;

typedef enum UsageType { LOCAL, GLOBAL, INPARAM, OUTPARAM } UsageType;

typedef struct SymbolTableEntry {
    Vector         *type_name; // Vector<char *>, NULL if primitive type
    TypeExpression *type_expression;
    uint32_t        width;
    bool            is_global;
    uint32_t        offset;
    UsageType       usage;
} SymbolTableEntry;

typedef struct SymbolTable {
    char      *scope;
    uint32_t   total_width;
    Hashtable *table; // <char *, SymbolTableEntry *>
} SymbolTable;

typedef struct RecordInfoEntry {
    Vector         *type_name; // Vector<char *>
    TypeExpression *type_expression;
    uint32_t        width;
    char           *ruid;
} RecordInfoEntry;

typedef Hashtable *RecordInfo; // <char *, RecordInfoEntry *>

#endif // SYMBOLTABLEDEF_H

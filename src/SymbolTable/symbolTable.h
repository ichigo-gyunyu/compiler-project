
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "AST/ast.h"
#include "SymbolTable/symbolTableDef.h"

void printSymbolTable(const AST ast);
void printGlobalVars(const AST ast);
void printActivationRecordInfo(const AST ast);
void printRecordInfo(const AST ast);
void initRecordInfo(const AST ast);
void constructSymbolTables(const AST ast, const bool print_all, const bool print_entry);

extern Vector    *ruids;         // Vector <char *>
extern RecordInfo rinfo;         // <char *, RecordInfoEntry *>
extern Hashtable *rec_defns;     // <char *, astStmtTypeDefinition *>
extern Hashtable *aliases;       // <char *, Vector<char *>>
extern Vector    *symbol_tables; // Vector <SymbolTable *>

#endif // SYMBOLTABLE_H

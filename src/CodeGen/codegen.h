
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#ifndef CODEGEN_H
#define CODEGEN_H

#include "AST/ast.h"
#include "SymbolTable/symbolTable.h"

/**
 * Partially implemented code generation module.
 * Works for single function programs involving integers.
 * Nested (3+) Boolean expressions needs work (TODO).
 */

void genCode(const AST ast, const char *asm_file);

#endif // CODEGEN_H

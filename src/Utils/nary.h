
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#ifndef NARY_H
#define NARY_H

#include "Lexer/lexer.h"
#include "utils.h"

typedef struct TreeNode {
    bool      is_leaf;
    TokenInfo t_info;
    char     *node_symbol;

    struct TreeNode *next_sibling;
    struct TreeNode *first_child;
    struct TreeNode *parent;
} TreeNode;

typedef TreeNode *Nary_tree;

TreeNode *nary_newNode(TokenInfo t, char *node_symbol, bool is_leaf);
TreeNode *nary_addChild(TreeNode *node, TokenInfo t, char *node_symbol, bool is_leaf);
void      nary_free(TreeNode *root);
void      nary_printInorder(TreeNode *root, FILE **fp);

#endif

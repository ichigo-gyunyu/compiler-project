#ifndef NARY_H
#define NARY_H

#include "utils.h"
#include "Lexer/lexer.h"

typedef struct TreeNode {
    bool  is_leaf;
    TokenInfo t_info;
    char *node_symbol;
    int line_no;

    struct TreeNode *next_sibling;
    struct TreeNode *first_child;
    struct TreeNode *parent;
} TreeNode;

typedef TreeNode *Nary_tree;

TreeNode *nary_newNode(TokenInfo t, char  *node_symbol, bool is_leaf);
TreeNode *nary_addChild(TreeNode *node, TokenInfo t, char  *node_symbol, bool is_leaf);
void      nary_free(TreeNode *root);
void nary_printInorder(TreeNode *root, FILE **fp);

#endif

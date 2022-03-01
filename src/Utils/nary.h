#ifndef NARY_H
#define NARY_H

#include "utils.h"

typedef struct TreeNode {
    int   val;
    bool  is_leaf;
    uint  line_num;
    char *lexeme;
    char *token_name;

    struct TreeNode *next_sibling;
    struct TreeNode *first_child;
    struct TreeNode *parent;
} TreeNode;

typedef TreeNode *Nary_tree;

TreeNode *nary_newNode(int val, bool is_leaf, char *lexeme, uint line_num, char *token_name);
TreeNode *nary_addChild(TreeNode *node, int val, bool is_leaf, char *lexeme, uint line_num, char *token_name);
void      nary_printInorder(TreeNode *root, FILE **fp);
void      nary_free(TreeNode *root);

#endif

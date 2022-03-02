
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "nary.h"

TreeNode *nary_newNode(TokenInfo t, char *node_symbol, bool is_leaf) {
    TreeNode *node     = calloc(1, sizeof *node);
    node->t_info       = t;
    node->node_symbol  = node_symbol;
    node->is_leaf      = is_leaf;
    node->next_sibling = NULL;
    node->first_child  = NULL;
    node->parent       = NULL;

    return node;
}

TreeNode *addSibling(TreeNode *node, TokenInfo t, char *node_symbol, bool is_leaf) {
    TreeNode *tmp = nary_newNode(t, node_symbol, is_leaf);

    while (node->next_sibling)
        node = node->next_sibling;

    node->next_sibling = tmp;

    return tmp;
}

TreeNode *nary_addChild(TreeNode *node, TokenInfo t, char *node_symbol, bool is_leaf) {
    TreeNode *tmp;
    if (!node->first_child) {
        tmp               = nary_newNode(t, node_symbol, is_leaf);
        node->first_child = tmp;
    } else
        tmp = addSibling(node->first_child, t, node_symbol, is_leaf);

    tmp->parent = node;
    return tmp;
}

void nary_free(TreeNode *root) {
    if (!root)
        return;

    if (root->first_child)
        nary_free(root->first_child);

    if (root->next_sibling)
        nary_free(root->next_sibling);

    free(root->node_symbol);
    free(root->t_info.lexeme);
    free(root);
}

void nary_printInorder(TreeNode *root, FILE **fp) {
    if (!root)
        return;

    nary_printInorder(root->first_child, fp);

    char *isleaf;
    if (root->is_leaf)
        isleaf = "yes";
    else
        isleaf = "no";

    char *par;
    if (root->parent)
        par = root->parent->node_symbol;
    else
        par = "ROOT";

    // Insert number
    if (root->is_leaf) {
        if (root->t_info.tk_type == TK_NUM)
            fprintf(*fp, "%10s %3d %30s %10lld %30s %4s %15s\n", root->t_info.lexeme, root->t_info.line_no,
                    getTokenTypeName(root->t_info.tk_type), root->t_info.val.val_int, par, isleaf, root->node_symbol);

        else if (root->t_info.tk_type == TK_RNUM)
            fprintf(*fp, "%10s %3d %30s %10.6f %30s %4s %15s\n", root->t_info.lexeme, root->t_info.line_no,
                    getTokenTypeName(root->t_info.tk_type), root->t_info.val.val_real, par, isleaf, root->node_symbol);

        else
            fprintf(*fp, "%10s %3d %30s %10s %30s %4s %15s\n", root->t_info.lexeme, root->t_info.line_no,
                    getTokenTypeName(root->t_info.tk_type), "NaN", par, isleaf, root->node_symbol);

    } else
        fprintf(*fp, "%10s %3d %30s %10s %30s %4s %15s\n", "----", root->t_info.line_no, "----", "NaN", par, isleaf,
                root->node_symbol);

    if (root->first_child) {
        TreeNode *temp = root->first_child->next_sibling;
        while (temp) {
            nary_printInorder(temp, fp);
            temp = temp->next_sibling;
        }
    }
}

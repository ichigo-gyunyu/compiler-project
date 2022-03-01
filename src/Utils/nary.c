#include "nary.h"

TreeNode *nary_newNode(int val, bool is_leaf, char *lexeme, uint line_num, char *token_name) {
    TreeNode *node     = calloc(1, sizeof *node);
    node->val          = val;
    node->is_leaf      = is_leaf;
    node->lexeme       = lexeme;
    node->line_num     = line_num;
    node->token_name   = token_name;
    node->next_sibling = NULL;
    node->first_child  = NULL;
    node->parent       = NULL;

    return node;
}

TreeNode *addSibling(TreeNode *node, int val, bool is_leaf, char *lexeme, uint line_num, char *token_name) {
    TreeNode *tmp = nary_newNode(val, is_leaf, lexeme, line_num, token_name);

    while (node->next_sibling)
        node = node->next_sibling;

    node->next_sibling = tmp;

    return tmp;
}

TreeNode *nary_addChild(TreeNode *node, int val, bool is_leaf, char *lexeme, uint line_num, char *token_name) {
    TreeNode *tmp;
    if (!node->first_child) {
        tmp               = nary_newNode(val, is_leaf, lexeme, line_num, token_name);
        node->first_child = tmp;
    } else
        tmp = addSibling(node->first_child, val, is_leaf, lexeme, line_num, token_name);

    tmp->parent = node;
    return tmp;
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
        par = root->parent->token_name;
    else
        par = "ROOT";

    fprintf(*fp, "%10s %3d %30s %30s %4s %15s\n", root->lexeme, root->line_num, root->token_name, par, isleaf,
            root->token_name);

    if (root->first_child) {
        TreeNode *temp = root->first_child->next_sibling;
        while (temp) {
            nary_printInorder(temp, fp);
            temp = temp->next_sibling;
        }
    }
}

/* int main() {
    TreeNode *n1 = nary_newNode(1, 0, "j", 0, "ds");
    nary_addChild(n1, 2, 0, "j", 0, "ds");
    nary_addChild(n1, 3, 0, "j", 0, "ds");
    nary_addChild(n1, 4, 1, "j", 0, "ds");
    nary_addChild(n1->first_child, 5, 1, "j", 0, "ds");
    nary_addChild(n1->first_child, 6, 1, "j", 0, "ds");
    nary_addChild(n1->first_child->next_sibling, 7, 1, "j", 0, "ds");
    nary_addChild(n1->first_child->next_sibling, 8, 1, "j", 0, "ds");

    FILE *fp = fopen("output/parsetree.txt", "w");
    nary_printInorder(n1, &fp);
    fclose(fp);
} */

#include "nary.h"

TreeNode *newNode(int val, bool is_leaf) {
    TreeNode *node     = malloc(sizeof(*node));
    node->val          = val;
    node->is_leaf      = is_leaf;
    node->next_sibling = NULL;
    node->first_child  = NULL;
    node->num_child    = 0;

    return node;
}

TreeNode *addSibling(TreeNode *node, int val, bool is_leaf) {
    TreeNode *temp = newNode(val, is_leaf);

    while (node->next_sibling)
        node = node->next_sibling;

    node->next_sibling = temp;

    return temp;
}

TreeNode *nary_addChild(TreeNode *node, int val, bool is_leaf) {
    if (!node->first_child)
        return node->first_child = newNode(val, is_leaf);
    else {
        return addSibling(node->first_child, val, is_leaf);
    }
}

#include "nary.h"


TreeNode *newNode(int val, bool is_leaf){
    TreeNode* node = malloc(sizeof(*node));
    node->val = val;
    node->is_leaf = is_leaf;
    node->next_sibling = NULL;
    node->first_child = NULL;
    node->num_child = 0;

    return node;
}

void addSibling(TreeNode *node, int val, bool is_leaf){
    TreeNode *temp = newNode(val, is_leaf);

    while(node->next_sibling)
        node = node->next_sibling;

    node->next_sibling = temp;
}

void addChild(TreeNode *node, int val, bool is_leaf){
    if(!node->first_child)
        node->first_child = newNode(val, is_leaf);
    else{
        addSibling(node->first_child, val, is_leaf);
    }
}


int main(){
    TreeNode *root = newNode(5, 0);
    // TreeNode *node1 = newNode(1,0);
    // TreeNode *node2 = newNode(2,1);
    // TreeNode *node3 = newNode(7,1);

    addChild(root, 8,0);
    addChild(root, 6,0);
    addChild(root->first_child, 1,1);
    printf("%d\n", root->val);
    printf("%d\n", root->first_child->val);
    printf("%d\n", root->first_child->first_child->val);
    printf("%d\n", root->first_child->next_sibling->val);
}

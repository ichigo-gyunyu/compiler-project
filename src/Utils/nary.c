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

void printInorder(TreeNode *root){
    if(!root)
        return;
    
    printInorder(root->first_child);
    printf("%d\n", root->val);

    if(root->first_child){
        TreeNode* temp = root->first_child->next_sibling;
        while(temp){
            printInorder(temp);
            temp = temp->next_sibling;
        }
    }

}

int main(){
    TreeNode* n1 = newNode(1,0);
    nary_addChild(n1, 2, 0);
    nary_addChild(n1, 3, 0);
    nary_addChild(n1, 4, 1);
    nary_addChild(n1->first_child, 5, 1);
    nary_addChild(n1->first_child, 6, 1);
    nary_addChild(n1->first_child->next_sibling, 7, 1);
    nary_addChild(n1->first_child->next_sibling, 8, 1);

    printInorder(n1);

}

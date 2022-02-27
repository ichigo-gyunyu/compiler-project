#include "utils.h"

typedef struct TreeNode{
    
    int num_child;
    int val;
    bool is_leaf;

    struct TreeNode *next_sibling;
    struct TreeNode *first_child;

}TreeNode;
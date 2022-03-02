#include "nary.h"

TreeNode *nary_newNode(TokenInfo t, char  *node_symbol, bool is_leaf) {
    TreeNode *node     = calloc(1, sizeof *node);
    node->t_info = t;
    node->node_symbol = node_symbol;
    node->is_leaf      = is_leaf;
    node->next_sibling = NULL;
    node->first_child  = NULL;
    node->parent       = NULL;

    return node;
}

TreeNode *addSibling(TreeNode *node, TokenInfo t, char  *node_symbol, bool is_leaf) {
    TreeNode *tmp = nary_newNode(t, node_symbol, is_leaf);

    while (node->next_sibling)
        node = node->next_sibling;

    node->next_sibling = tmp;

    return tmp;
}

TreeNode *nary_addChild(TreeNode *node, TokenInfo t, char  *node_symbol, bool is_leaf){
    TreeNode *tmp;
    if (!node->first_child) {
        tmp               = nary_newNode(t, node_symbol, is_leaf);
        node->first_child = tmp;
    } else
        tmp = addSibling(node->first_child, t, node_symbol, is_leaf);

    tmp->parent = node;
    return tmp;
}


void nary_free(TreeNode *root){
    if(!root)
        return;

    if(root->first_child)
        nary_free(root->first_child);
    
    if(root->next_sibling)
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

    //Insert number
    if(root->is_leaf){
        if(root->t_info.tk_type == TK_NUM)
            fprintf(*fp, "%10s %3d %30s %10lld %30s %4s %15s\n", root->t_info.lexeme, root->t_info.line_no, 
            getTokenTypeName(root->t_info.tk_type), root->t_info.val.val_int, root->parent->node_symbol, 
            isleaf, root->node_symbol);
        
        else if(root->t_info.tk_type == TK_RNUM)
            fprintf(*fp, "%10s %3d %30s %.10f %30s %4s %15s\n", root->t_info.lexeme, root->t_info.line_no, 
            getTokenTypeName(root->t_info.tk_type), root->t_info.val.val_real, root->parent->node_symbol, 
            isleaf, root->node_symbol);

        else
            fprintf(*fp, "%10s %3d %30s %10s %30s %4s %15s\n", root->t_info.lexeme, root->t_info.line_no, 
                getTokenTypeName(root->t_info.tk_type), "NaN", root->parent->node_symbol, 
                isleaf, root->node_symbol);
        
    }
    else
        fprintf(*fp, "%10s %3d %30s %10s %30s %4s %15s\n", "----", root->t_info.line_no, 
            "----", "NaN", root->parent->node_symbol, 
            isleaf, root->node_symbol);

    
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

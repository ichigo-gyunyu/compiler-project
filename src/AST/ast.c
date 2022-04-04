
/**
 * Group 2
 * Sanjeet Kapadia   2018B4A70137P
 * Lingesh Kumaar    2018B4A70857P
 * Aman Mishra       2018B4A70877P
 * Sidharth Varghese 2019A7PS1133P
 * Edara Bala Mukesh 2019A7PS0081P
 */

#include "ast.h"

#define CHILD1 first_child
#define CHILD2 first_child->next_sibling
#define CHILD3 first_child->next_sibling->next_sibling
#define CHILD4 first_child->next_sibling->next_sibling->next_sibling
#define CHILD5 first_child->next_sibling->next_sibling->next_sibling->next_sibling
#define CHILD6 first_child->next_sibling->next_sibling->next_sibling->next_sibling->next_sibling
#define CHILD7 first_child->next_sibling->next_sibling->next_sibling->next_sibling->next_sibling->next_sibling

void *constructRecursively(const Nary_tree pt);

AST constructAST(const Nary_tree pt) { return (AST)constructRecursively(pt); }

void *constructRecursively(const Nary_tree pt) {
    uint8_t rule_num = pt->rule_no;

    switch (rule_num) {

    // program -> otherFunctions mainFunction
    case 1: {
        astProgram *node = malloc(sizeof *node);

        *node = (astProgram){
            .otherFunctions = constructRecursively(pt->CHILD1),
            .mainFunction   = constructRecursively(pt->CHILD2),
        };

        return node;
    }

    // mainFunction -> TK_MAIN stmts TK_END
    case 2: {
        astFunction *node = malloc(sizeof *node);

        *node = (astFunction){
            .functionName = str_dup("main"),
            .inputParams  = NULL,
            .outputParams = NULL,
            .statements   = constructRecursively(pt->CHILD2),
        };

        return node;
    }

    // otherFunctions -> function otherFunctions
    case 3: {
        Vector      *functions = vec_init(sizeof(astFunction *), NULL, NULL, VEC_START_SIZE);
        astFunction *node;
        Nary_tree    pt_copy = pt;

        while ((node = constructRecursively(pt_copy->CHILD1)) != NULL) {
            vec_pushBack(functions, &node);
            pt_copy = pt_copy->CHILD2;
        }

        return functions;
    }

    // otherFunctions ->
    case 4: {
        return NULL;
    }

    // function -> TK_FUNID input_par output_par TK_SEM stmts TK_END
    case 5: {
        astFunction *node = malloc(sizeof *node);

        *node = (astFunction){
            .functionName = str_dup(pt->CHILD1->t_info.lexeme),
            .inputParams  = constructRecursively(pt->CHILD2),
            .outputParams = constructRecursively(pt->CHILD3),
            .statements   = constructRecursively(pt->CHILD5),
        };

        return node;
    }

    // input_par -> TK_INPUT TK_PARAMETER TK_LIST TK_SQL parameter_list TK_SQR
    case 6: {
        return constructRecursively(pt->CHILD5);
    }

    // output_par -> TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL parameter_list TK_SQR
    case 7: {
        return constructRecursively(pt->CHILD5);
    }

    // output_par ->
    case 8: {
        return NULL;
    }

    // parameter_list -> dataType TK_ID remaining_list
    case 9: {
    }
    }

    return NULL;
}

/*********************************************************************/
/* File: stdlib.c                                                    */
/* Implementation of the standard functions.                         */
/* Author: Minjie Zha                                                */
/*********************************************************************/

#include "globals.h"
#include "stdlib.h"

#define FUNCTION_NUM 4

StandardFun standardFunctions[FUNCTION_NUM] = {
    { "Y",  "(lambda f (lambda x f (x x )) (lambda x f (x x)))"},
    { "not","(lambda p (lambda x (lambda y p y x)))"},
    { "or", "(lambda p (lambda q p p q))"},
    { "and","(lambda p (lambda q p q p))"}
};

StandardFun* lookupStandardFun(const char* name) {
    int i;
    for(i=0;i<FUNCTION_NUM;i++) {
        if(strcmp(name,standardFunctions[i].name)==0) {
            return &standardFunctions[i];
        }
    }
    return NULL;
}

extern TreeNode* tree;  // use the tree for parser
TreeNode* expandStandardFun(StandardFun* fun) {
    useStringBuffer(fun->expr);
    yyparse();
    deleteStringBuffer();
    return tree;
}

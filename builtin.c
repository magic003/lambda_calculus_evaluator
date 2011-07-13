/**********************************************************************/
/* File: builtin.c                                                    */
/* Implementation of builtin functions.                               */
/* Author: Minjie Zha                                                 */
/**********************************************************************/
#include "globals.h"
#include "util.h"
#include "builtin.h"

// definitions of expand functions
static TreeNode* expandByName(const char* name) {
    TreeNode *tree = newTreeNode(AbsK);
    TreeNode *x = newTreeNode(IdK);
    x->name = stringCopy("x");
    tree->children[0] = x;

    TreeNode *body = newTreeNode(AbsK);
    TreeNode *y = newTreeNode(IdK);
    y->name = stringCopy("y");
    body->children[0] = y;
    TreeNode *primi = newTreeNode(PrimiK);
    primi->name = stringCopy(name);
    TreeNode *x1 = newTreeNode(IdK);
    x1->name = stringCopy("x");
    TreeNode *y1 = newTreeNode(IdK);
    y1->name = stringCopy("y");
    primi->children[0] = x1;
    primi->children[1] = y1;
    body->children[1] = primi;

    tree->children[1] = body;

    return tree;
}

static TreeNode* expandPlus() {
    return expandByName("+");
}

static TreeNode* expandMinus() {
    return expandByName("-");
}

static TreeNode* expandTimes() {
    return expandByName("*");
}

static TreeNode* expandOver() {
    return expandByName("/");
}

static TreeNode* expandMod() {
    return expandByName("%");
}

static TreeNode* expandPow() {
    return expandByName("^");
}

static TreeNode* expandLt() {
    return expandByName("<");
}

static TreeNode* expandEq() {
    return expandByName("=");
}

static TreeNode* expandGt() {
    return expandByName(">");
}

static TreeNode* expandLe() {
    return expandByName("<=");
}

static TreeNode* expandNe() {
    return expandByName("!=");
}

static TreeNode* expandGe() {
    return expandByName(">=");
}
// End of expand functions

#define FUNCTION_NUM 12

BuiltinFun builtinFunctions[FUNCTION_NUM] = {
    { "+", expandPlus},
    { "-", expandMinus},
    { "*", expandTimes},
    { "/", expandOver},
    { "%", expandMod},
    { "^", expandPow},
    { "<", expandLt},
    { "=", expandEq},
    { ">", expandGt},
    { "<=",expandLe},
    { "!=",expandNe},
    { ">=",expandGe}
};

BuiltinFun* lookupBuiltinFun(const char* name) {
    int i;
    for(i=0;i<FUNCTION_NUM;i++) {
        if(strcmp(name,builtinFunctions[i].name)==0) {
            return &builtinFunctions[i];
        }
    }
    return NULL;
}

BuiltinFun* builtinFuns(int *size) {
    *size = FUNCTION_NUM;
    return builtinFunctions;
}

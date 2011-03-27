/*********************************************************************/
/* File: primitive.c                                                 */
/* Implements the primitive operations.                              */
/* Author: Minjie Zha                                                */
/*********************************************************************/

#include "globals.h"
#include "util.h"
#include "primitive.h"

// primitive functions
static TreeNode* plus(TreeNode* node) {
    TreeNode* result = newTreeNode(ConstK);
    result->value = node->children[0]->value + node->children[1]->value;
    return result;
}

static TreeNode* minus(TreeNode* node) {
    TreeNode* result = newTreeNode(ConstK);
    result->value = node->children[0]->value - node->children[1]->value;
    return result;
}

static TreeNode* times(TreeNode* node) {
    TreeNode* result = newTreeNode(ConstK);
    result->value = node->children[0]->value * node->children[1]->value;
    return result;
}

static TreeNode* over(TreeNode* node) {
    TreeNode* result = newTreeNode(ConstK);
    result->value = node->children[0]->value / node->children[1]->value;
    return result;
}

static TreeNode* mod(TreeNode* node) {
    TreeNode* result = newTreeNode(ConstK);
    result->value = node->children[0]->value % node->children[1]->value;
    return result;
}
// end of primitive functions

#define NUM 5
typedef TreeNode* (*PrimiFun)(TreeNode* node);
static struct {
    char* name;
    PrimiFun fun;
} primitiveFunctions[NUM] = {
    {"+",plus},
    {"-",minus},
    {"*",times},
    {"/",over},
    {"%",mod}
};

TreeNode* evalPrimitive(TreeNode *node) {
    int i;
    for(i=0;i<NUM;i++) {
        if(strcmp(node->name,primitiveFunctions[i].name)==0) {
            return (primitiveFunctions[i].fun)(node);
        }
    }

    fprintf(errOut,"Unsupported primitive function: %s\n",node->name);
    return node;
}

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

static TreeNode* power(TreeNode* node) {
    TreeNode* result = newTreeNode(ConstK);
    int b = node->children[0]->value;
    int p = node->children[1]->value;
    int i;
    for(i=0,result->value=1;i<p;i++) {
        result->value *= b;
    }
    return result;
}

// util functions for construct true/false trees
static TreeNode* boolNode(const char* ret) {
    TreeNode* result = newTreeNode(AbsK);
    result->children[0] = newTreeNode(IdK);
    result->children[0]->name = stringCopy("x");

    TreeNode* body = newTreeNode(AbsK);
    body->children[0] = newTreeNode(IdK);
    body->children[0]->name = stringCopy("y");
    body->children[1] = newTreeNode(IdK);
    body->children[1]->name = stringCopy(ret);

    result->children[1] = body;
    return result;
}

static TreeNode* trueNode() {
    return boolNode("x");
}

static TreeNode* falseNode() {
    return boolNode("y");
}

static TreeNode* lt(TreeNode* node) {
    if(node->children[0]->value<node->children[1]->value) {
        return trueNode();
    }
    return falseNode();
}

static TreeNode* eq(TreeNode* node) {
    if(node->children[0]->value==node->children[1]->value) {
        return trueNode();
    }
    return falseNode();
}

static TreeNode* gt(TreeNode* node) {
    if(node->children[0]->value>node->children[1]->value) {
        return trueNode();
    }
    return falseNode();
}

static TreeNode* le(TreeNode* node) {
    if(node->children[0]->value<=node->children[1]->value) {
        return trueNode();
    }
    return falseNode();
}

static TreeNode* ne(TreeNode* node) {
    if(node->children[0]->value!=node->children[1]->value) {
        return trueNode();
    }
    return falseNode();
}

static TreeNode* ge(TreeNode* node) {
    if(node->children[0]->value>=node->children[1]->value) {
        return trueNode();
    }
    return falseNode();
}
// end of primitive functions

#define NUM 12
typedef TreeNode* (*PrimiFun)(TreeNode* node);
static struct {
    char* name;
    PrimiFun fun;
} primitiveFunctions[NUM] = {
    {"+",plus},{"-",minus},{"*",times},{"/",over},{"%",mod},{"^",power},
    {"<",lt},{"=",eq},{">",gt},{"<=",le},{"!=",ne},{">=",ge}
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

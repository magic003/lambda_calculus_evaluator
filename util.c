/************************************************************/
/* File: util.c                                             */
/* Implementation of the utilities.                         */
/* Author: Minjie Zha                                       */
/************************************************************/

#include "globals.h"
#include "util.h"

TreeNode * newTreeNode(ExprKind kind) {
    TreeNode * node = (TreeNode *) malloc(sizeof(TreeNode));
    if(node == NULL) {
        fprintf(errOut,"Out of memory.\n");
    }else {
        node->kind = kind;
        node->name = NULL;
        int i;
        for(i=0;i<MAXCHILDREN;i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

void deleteTreeNode(TreeNode* node) {
    if(node!=NULL) {
        deleteTreeNode(node->children[0]);
        node->children[0] = NULL;
        deleteTreeNode(node->children[1]);
        node->children[1] = NULL;
        if(node->name!=NULL) {
            free(node->name);
            node->name=NULL;
        }
    }
}

char * stringCopy(const char* s) {
    char* t;

    if(s == NULL) {
        return NULL;
    }
    t = malloc(strlen(s)+1);
    if(t == NULL) {
        fprintf(errOut, "Out of memory.\n");
    }else {
        strcpy(t,s);
    }
    return t;
}

void printSpaces(int n) {
    int i;
    for(i=0;i<n;i++) {
        fprintf(out," ");
    }
}

void printTree(TreeNode * tree) {
    static int indentno = 0;
    int i;
    if(tree==NULL) return;

    switch(tree->kind) {
        case IdK:
            fprintf(out,"Identifier: %s\n",tree->name);
            break;
        case AbsK:
            fprintf(out,"Abstraction:\n");
            break;
        case AppK:
            fprintf(out,"Application:\n");
            break;
        default:
            fprintf(out,"Unknown expression kind.\n");
    }
    for(i=0;i<MAXCHILDREN;i++) {
        indentno+=2;
        if(tree->children[i]!=NULL) {
            printSpaces(indentno);
            printTree(tree->children[i]);
        }
        indentno-=2;
    }
}

void printExpression(TreeNode* expr) {
    if(expr==NULL) return;

    switch(expr->kind) {
        case IdK:
            fprintf(out,"%s",expr->name);
            break;
        case AbsK:
            fprintf(out,"(lambda ");
            printExpression(expr->children[0]);
            fprintf(out," ");
            printExpression(expr->children[1]);
            fprintf(out,")");
            break;
        case AppK:
            printExpression(expr->children[0]);
            fprintf(out," ");
            printExpression(expr->children[1]);
            break;
        default:
            fprintf(out,"Unknown expression kind.\n");
    }
}

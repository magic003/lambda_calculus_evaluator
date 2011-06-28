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

void deleteTree(TreeNode* tree) {
    if(tree!=NULL) {
        deleteTree(tree->children[0]);
        tree->children[0] = NULL;
        deleteTree(tree->children[1]);
        tree->children[1] = NULL;
        if(tree->name!=NULL) {
            free(tree->name);
            tree->name=NULL;
        }
        free(tree);
    }
}

void deleteTreeNode(TreeNode *node) {
    if(node!=NULL) {
        if(node->name!=NULL) {
            free(node->name);
        }
        free(node);
    }
}

TreeNode *duplicateTree(TreeNode* tree) {
    if(tree!=NULL) {
        TreeNode *result = newTreeNode(tree->kind);
        result->name = stringCopy(tree->name);
        result->value = tree->value;
        result->children[0] = duplicateTree(tree->children[0]);
        result->children[1] = duplicateTree(tree->children[1]);
        return result;
    }
    return NULL;
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

static void printSpaces(int n, FILE* stream) {
    int i;
    for(i=0;i<n;i++) {
        fprintf(stream," ");
    }
}

void printTree(TreeNode * tree, FILE* stream) {
    static int indentno = 0;
    int i;
    if(tree==NULL) return;

    switch(tree->kind) {
        case IdK:
            fprintf(stream,"Identifier: %s\n",tree->name);
            break;
        case ConstK:
            fprintf(stream,"Constant: %d\n",tree->value);
            break;
        case AbsK:
            fprintf(stream,"Abstraction:\n");
            break;
        case AppK:
            fprintf(stream,"Application:\n");
            break;
        case PrimiK:
            fprintf(stream,"Primitive: %s\n",tree->name);
            break;
        default:
            fprintf(stream,"Unknown expression kind.\n");
    }
    for(i=0;i<MAXCHILDREN;i++) {
        indentno+=2;
        if(tree->children[i]!=NULL) {
            printSpaces(indentno,stream);
            printTree(tree->children[i],stream);
        }
        indentno-=2;
    }
}

void printExpression(TreeNode* expr, FILE* stream) {
    if(expr==NULL) return;

    switch(expr->kind) {
        case IdK:
            fprintf(stream,"%s",expr->name);
            break;
        case ConstK:
            fprintf(stream,"%d", expr->value);
            break;
        case AbsK:
            fprintf(stream,"(lambda ");
            printExpression(expr->children[0],stream);
            fprintf(stream," ");
            printExpression(expr->children[1],stream);
            fprintf(stream,")");
            break;
        case AppK:
            printExpression(expr->children[0],stream);
            fprintf(stream," ");
            if(expr->children[1]->kind==AppK
                || expr->children[1]->kind==PrimiK) {
                fprintf(stream,"(");
            }
            printExpression(expr->children[1],stream);
            if(expr->children[1]->kind==AppK
                || expr->children[1]->kind==PrimiK) {
                fprintf(stream,")");
            }
            break;
        case PrimiK:
            if(expr->children[0]->kind==AppK 
                || expr->children[0]->kind==PrimiK) {
                fprintf(stream,"(");
            }
            printExpression(expr->children[0],stream);
            if(expr->children[0]->kind==AppK
                || expr->children[0]->kind==PrimiK) {
                fprintf(stream,")");
            }
            fprintf(stream," ");
            fprintf(stream,"`%s`",expr->name);
            fprintf(stream," ");
            if(expr->children[1]->kind==AppK 
                || expr->children[1]->kind==PrimiK) {
                fprintf(stream,"(");
            }
            printExpression(expr->children[1],stream);
            if(expr->children[1]->kind==AppK
                || expr->children[1]->kind==PrimiK) {
                fprintf(stream,")");
            }
            break;
        default:
            fprintf(stream,"Unknown expression kind.\n");
    }
}

int isValue(TreeNode *expr) {
    return expr!=NULL 
        && (expr->kind==ConstK || expr->kind==AbsK);
}

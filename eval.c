/***************************************************************/
/* File: eval.c                                                */
/* Implementation of evaluation the expression.                */
/* Author: Minjie Zha                                          */
/***************************************************************/
#include "globals.h"
#include "util.h"
#include "varset.h"
#include "eval.h"

TreeNode * evaluate(TreeNode *expr) {
    if(expr!=NULL) {
        switch(expr->kind) {
            case IdK:
                return expr;
            case AbsK:
                expr->children[1] = evaluate(expr->children[1]);
                return expr;
            case AppK:
                expr->children[0] = evaluate(expr->children[0]);
                expr->children[1] = evaluate(expr->children[1]);
                return betaReduction(expr);
            default:
                fprintf(errOut,"Unkown expression kind.\n");
        }
    }
    return expr;
}

/* Gets the free variables in the expression. */ 
static VarSet * FV(TreeNode *expr) {
    VarSet* set = NULL;
    VarSet* set1 = NULL;
    VarSet* set2 = NULL;
    switch(expr->kind) {
        case IdK:
            set = newVarSet();
            addVar(set,expr->name);
            break;
        case AbsK:
            set = FV(expr->children[1]);
            deleteVar(set,expr->children[0]->name);
            break;
        case AppK:
            set = newVarSet();
            set1 = FV(expr->children[0]);
            set2 = FV(expr->children[1]);
            unionVarSet(set,set1,set2);
            deleteVarSet(set1);
            deleteVarSet(set2);
            break;
        default:
            fprintf(errOut,"Unknown expression type.\n");
    }
    return set;
}

/* Performs substitution on the expression. */
static TreeNode *substitute(TreeNode *expr, TreeNode *var, TreeNode *sub) {
    if(expr==NULL || var==NULL || sub==NULL) return expr;

    if(var->kind!=IdK) {
        fprintf(errOut,"The replaced expression is not a variable.\n");
        return expr;
    }
    const char * parname = NULL;
    TreeNode * result = NULL;
    int deleteSub = 0;
    switch(expr->kind) {
        case IdK:
            if(strcmp(expr->name,var->name)==0) {
                return sub;
            }else {
                return expr;
            }
        case AbsK:
            parname = expr->children[0]->name;
            if(strcmp(parname,var->name)!=0) {
                VarSet* set = FV(sub); 
                while(contains(set,parname)) {  // do alpha conversion
                    expr = alphaConversion(expr);
                    parname = expr->children[0]->name;
                }
                result = substitute(expr->children[1],var,sub);
                if(result!=expr->children[1]) {
                    deleteTreeNode(expr->children[1]);
                }else if(expr->children[1]->kind==IdK) {
                    deleteTreeNode(sub);
                }
                expr->children[1] = result;
                deleteVarSet(set);
            }
            return expr;
        case AppK:
            result = substitute(expr->children[0],var,sub);
            if(result!=expr->children[0]) {
                deleteTreeNode(expr->children[0]);
            }else if(expr->children[0]->kind==IdK) {
                deleteSub = 1;
            }
            expr->children[0] = result;
            result = substitute(expr->children[1],var,sub);
            if(result!=expr->children[1]) {
                deleteTreeNode(expr->children[1]);
            }else if(deleteSub && expr->children[1]->kind==IdK) {
                deleteTreeNode(sub);
            }
            expr->children[1] = result;
            deleteSub = 0;
            return expr;
        default:
            fprintf(errOut,"Unknown expression type.\n");
    }
    return expr;
}

TreeNode * alphaConversion(TreeNode *expr) {
    if(expr->kind!=AbsK) {
        fprintf(errOut,"Alpha conversion can only be applied to abstraction expression.\n");
        return expr;
    }

    VarSet* set = FV(expr->children[1]);
    char * name = strdup(expr->children[0]->name);
    // pick a new name
    while(strcmp(name,expr->children[0]->name)==0 ||  contains(set,name)==1) {
        // replace the last character with a different alphabet.
        // TODO this may not work if all alphabets are used
        char lastchar = name[strlen(name)-1];
        name[strlen(name)-1] = 'a' + (lastchar+1-'a')%('z'-'a'+1);
    }
    TreeNode *var = newTreeNode(IdK);
    var->name = name;
    TreeNode *result = substitute(expr->children[1], expr->children[0], var);
    if(result!=expr->children[1]) {
        deleteTreeNode(expr->children[1]);
    }
    expr->children[1] = result;
    deleteTreeNode(expr->children[0]);
    expr->children[0] = var;
    return expr;
}

TreeNode * betaReduction(TreeNode *expr) {
    if(expr->kind!=AppK) {
        fprintf(errOut,"Beta reduction can only be applied to application expression.\n");
        return expr;
    }
    TreeNode* left = expr->children[0];
    if(left->kind==IdK || left->kind==AppK) {
        return expr;
    }else if(left->kind==AbsK) {
        TreeNode* result = substitute(left->children[1],left->children[0],expr->children[1]);
        if(result!=left->children[1]) {
            deleteTreeNode(left->children[1]);
            left->children[1] = NULL;
        }
        deleteTreeNode(left->children[0]);
        return result;
    }
    return expr;
}


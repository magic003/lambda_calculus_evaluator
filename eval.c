/***************************************************************/
/* File: eval.c                                                */
/* Implementation of evaluation the expression.                */
/* Author: Minjie Zha                                          */
/***************************************************************/
#include "globals.h"
#include "util.h"
#include "varset.h"
#include "builtin.h"
#include "primitive.h"
#include "stdlib.h"
#include "eval.h"

/* Tests if the expression is a value. */
static int isValue(TreeNode *expr) {
    return expr!=NULL 
        && (expr->kind==IdK || expr->kind==ConstK || expr->kind==AbsK);
}

/* Search a function in builtin library and standard library by name. */
static TreeNode* resolveFunction(const char* name) {
    BuiltinFun* fun = NULL;
    StandardFun* stdFun = NULL;
    if((fun=lookupBuiltinFun(name))!=NULL) {
        return (fun->expandFun)();
    } else if((stdFun=lookupStandardFun(name))!=NULL) {
        return expandStandardFun(stdFun);
    }
    return NULL;
}

TreeNode * evaluate(TreeNode *expr) {
    TreeNode* state = expr;
    TreeNode** previous = NULL;
    TreeNode* current = NULL;
    while(state!=NULL && !isValue(state)) {
        previous = &state;
        current = state;
        while(current!=NULL) {
            if(current->kind==AppK) {   // applications
                if(!isValue(current->children[0])) {
                    previous = &current->children[0];
                    current = current->children[0];
                } else if(current->children[0]->kind==ConstK) {
                    fprintf(errOut, "Error: cannot apply a constant to any argument.\n");
                    fprintf(errOut, "\t\t");
                    printExpression(current,errOut);
                    deleteTree(state);
                    return NULL;
                }else if(current->children[0]->kind==IdK) {
                    // find function from builtin and standard library
                    TreeNode* fun = resolveFunction(current->children[0]->name);
                    if(fun==NULL) {
                        fprintf(errOut, "Error: %s is not a predefined function.\n", current->children[0]->name);
                        deleteTree(state);
                        return NULL;
                    }
                    deleteTree(current->children[0]);
                    current->children[0] = fun;
                    break;
                }   else {
                    current=betaReduction(current);
                    *previous = current;
                    break;
                }
            }else if(current->kind==PrimiK) {  // primitive application
                if(!isValue(current->children[0])) {
                    previous = &current->children[0];
                    current = current->children[0];
                }else if(!isValue(current->children[1])) {
                    previous = &current->children[1];
                    current = current->children[1];
                }else { // reduce the current node
                    // only perform primitive operation if operands are constants
                    if(current->children[0]->kind==ConstK 
                        && current->children[1]->kind==ConstK) {
                        TreeNode* tmp  = evalPrimitive(current);
                        deleteTree(current);
                        current = tmp;
                        *previous = current;
                        break;
                    } else {
                        fprintf(errOut, "Error: %s can only be applied on constants.\n", current->name);
                        deleteTree(state);
                        return NULL;
                    }
                }
            }else {
                    fprintf(errOut,"Error: Cannot evaluate unkown expression kind.\n");
                    deleteTree(state);
                    return NULL;
            }
        }
        #ifdef DEBUG
            // print intermediate steps
            if(!isValue(state)) {
                fprintf(out,"-> ");
                printExpression(state,out);
                fprintf(out,"\n");
            }
        #endif
    }
    return state;
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
        case ConstK:
            set = newVarSet();
            break;
        case AbsK:
            set = FV(expr->children[1]);
            deleteVar(set,expr->children[0]->name);
            break;
        case AppK:
        case PrimiK:
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
    switch(expr->kind) {
        case IdK:
            if(strcmp(expr->name,var->name)==0) {
                deleteTree(expr);
                return duplicateTree(sub);
            }else {
                return expr;
            }
        case ConstK:
            return expr;
        case AbsK:
            parname = expr->children[0]->name;
            if(strcmp(parname,var->name)!=0) {
                VarSet* set = FV(sub); 
                while(contains(set,parname)) {  // do alpha conversion
                    expr = alphaConversion(expr);
                    parname = expr->children[0]->name;
                }
                result = substitute(expr->children[1],var,sub);
                expr->children[1] = result;
                deleteVarSet(set);
            }
            return expr;
        case AppK:
        case PrimiK:
            result = substitute(expr->children[0],var,sub);
            expr->children[0] = result;
            result = substitute(expr->children[1],var,sub);
            expr->children[1] = result;
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
    char *name = NULL;
    int len = strlen(expr->children[0]->name);
    int attempts = 0;
    // pick a new name
    do {
        if(name!=NULL) free(name);  // free the last attempt
        attempts++;
        name = malloc(len+attempts);
        strcpy(name,expr->children[0]->name);
        int a;
        // append '_' to the original name
        for(a=0;a<attempts;a++) {
            strcat(name,"_");
        }
    } while(strcmp(name,expr->children[0]->name)==0 || contains(set,name)==1);

    TreeNode *var = newTreeNode(IdK);
    var->name = name;
    TreeNode *result = substitute(expr->children[1], expr->children[0], var);
    expr->children[1] = result;
    deleteTree(expr->children[0]);
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
        // If result == left->children[1], prevent deleting the result;
        // If result != left->children[1], already deleted in substitute(), so
        // prevent double free.
        left->children[1] = NULL;
        deleteTree(expr);
        return result;
    }
    return expr;
}


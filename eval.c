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
#include "stdlib.h" // standard library
#include "cc_machine.h"
#include "eval.h"

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

/* Perform reductions to the expression of type application or primitive. 
 * Returns 1 if successful, otherwise failed.  
 */
static int performReduction(State* state) {
    if(state->controlStr->kind==AppK) {
        if(state->controlStr->children[0]->kind==ConstK) {
            fprintf(errOut, "Error: cannot apply a constant to any argument.\n");
            fprintf(errOut, "\t\t");
            printExpression(state->controlStr,errOut);
            cc_cleanup(state);
            return 0;
        }else if(state->controlStr->children[0]->kind==IdK) {
            // find function from builtin and standard library
            TreeNode* fun = resolveFunction(state->controlStr->children[0]->name);
            if(fun==NULL) {
                fprintf(errOut, "Error: %s is not a predefined function.\n", state->controlStr->children[0]->name);
                cc_cleanup(state);
                return 0;
            }
            deleteTree(state->controlStr->children[0]);
            state->controlStr->children[0] = fun;
        } else {
            TreeNode *tmp = betaReduction(state->controlStr);
            if(state->context!=NULL) {
                if(state->context->expr->children[0]==state->controlStr) {
                    state->context->expr->children[0] = tmp;
                }else {
                    state->context->expr->children[1] = tmp;
                }
            }
            state->controlStr = tmp;
        }
    }else if(state->controlStr->kind==PrimiK) {
        // only perform primitive operation if operands are constants
        if(state->controlStr->children[0]->kind==ConstK 
            && state->controlStr->children[1]->kind==ConstK) {
            TreeNode* tmp  = evalPrimitive(state->controlStr);
            if(state->context!=NULL) {
                if(state->context->expr->children[0]==state->controlStr) {
                    state->context->expr->children[0] = tmp;
                } else {
                    state->context->expr->children[1] = tmp;
                }
            }
            deleteTree(state->controlStr);
            state->controlStr = tmp;
        } else {
            fprintf(errOut, "Error: %s can only be applied on constants.\n", state->controlStr->name);
            cc_cleanup(state);
            return 0;
        }
    }else {
        fprintf(errOut,"Error: Cannot evaluate unkown expression kind.\n");
        cc_cleanup(state);
        return 0;
    }
    return 1;
}

TreeNode * evaluate(TreeNode *expr) {
    State * state = cc_newState();
    state->controlStr = expr;

    Context * ctx = NULL;
    while(!cc_canTerminate(state)) {
        if(isValue(state->controlStr)) {
            // control string is the right child
            if(state->controlStr==state->context->expr->children[1]) {
                // pop an expression from the context
                state->controlStr = state->context->expr;
                ctx = state->context;
                state->context = state->context->next;
                cc_deleteContext(ctx);
                ctx = NULL;
                if(!performReduction(state)) {
                    return NULL;
                }
            } else {
                state->controlStr = state->context->expr->children[1];
            }
        }else {
            if(!isValue(state->controlStr->children[0])
                || !isValue(state->controlStr->children[1])) {
                // push the current expression into context
                ctx = cc_newContext();
                ctx->expr = state->controlStr;
                ctx->next = state->context;
                state->context = ctx;
                if(!isValue(state->controlStr->children[0])) {
                    state->controlStr = state->controlStr->children[0];
                }else {
                    state->controlStr = state->controlStr->children[1];
                }
            } else { // evaluate control string
                if(!performReduction(state)) {
                    return NULL;
                }
            }
        }
        #ifdef DEBUG
            // print intermediate steps
            if(!cc_canTerminate(state)) {
                fprintf(out,"-> ");
                printExpression(cc_getProgram(state),out);
                fprintf(out,"\n");
            }
        #endif
    }

    TreeNode* result = state->controlStr;
    cc_deleteState(state);
    return result;
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


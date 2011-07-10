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
#include "cek_machine.h"
#include "eval.h"

static TreeNode* resolveFunction(const char* name);
static int performReduction(State* state);
static Closure* lookupVariable(const char *name, Environment *env);
static TreeNode* resolveFreeVariables(TreeNode *expr, Environment *env);
static VarSet * FV(TreeNode *expr);
static TreeNode *substitute(TreeNode *expr, TreeNode *var, TreeNode *sub);
static void attachChild(const State *state, TreeNode *expr);

TreeNode * evaluate(TreeNode *expr) {
    State * state = cek_newState();
    state->closure = cek_newClosure(expr,NULL);

    int error = 0;
    Continuation * ctn = NULL;
    Closure *closure = NULL;
    Environment *env = NULL;
    while(!cek_canTerminate(state)) {
        if(state->closure->expr->kind==IdK) {
            // Find mapped closure from the evironment
            closure = lookupVariable(state->closure->expr->name,state->closure->env);
            if(closure==NULL) {
                // find function from builtin and standard library
                TreeNode* fun = resolveFunction(state->closure->expr->name);
                if(fun==NULL) {
                    fprintf(errOut, "Error: %s is not a defined variable or function.\n", state->closure->expr->name);
                    error = 1;
                    break;
                }
                // Replace the function name with function definition
                deleteTreeNode(state->closure->expr);
                state->closure->expr = fun;
                attachChild(state,fun); /* attach fun to parent */
            } else {
                /*
                 * Should not use the found closure directly, because it will
                 * be deleted after evaluted and the mapped id may be used
                 * again afterwards. So does the expression in it.
                 */
                closure = cek_newClosure(duplicateTree(closure->expr),closure->env);
                // replace the closure with mapped closure
                cek_deleteClosure(state->closure);
                state->closure = closure;
                attachChild(state,closure->expr); /* attach to parent */
            }
        } else if(isValue(state->closure->expr)) {
            if(state->continuation==NULL) {
                // if the control string is an abstraction, need to substitute
                // free variables in it using the environment for it.
                TreeNode *tmp = resolveFreeVariables(state->closure->expr,state->closure->env);
                if(tmp==NULL) {
                    // delete children
                    deleteTree(state->closure->expr->children[0]);
                    deleteTree(state->closure->expr->children[1]);
                    error = 1;
                } else {
                    state->closure->expr = tmp;
                }
                break;
            } else if(state->continuation->tag==ArgKK) {
                // Pop the current continuation
                ctn = state->continuation;
                state->continuation = ctn->next;
                // add the argument value to an environment
                /*
                 * The left child of the application may not be an
                 * abstraction, so name is uncertain now. Leave it
                 * as NULL and set it in performReduction().
                 */
                env = cek_newEnvironment(NULL,cek_newClosure(ctn->closure->expr->children[1],ctn->closure->env),state->closure->env);
                // delete the current closure
                state->closure->expr = NULL;
                cek_deleteClosure(state->closure);

                state->closure = cek_newClosure(ctn->closure->expr,env);
                if(!performReduction(state)) {
                    error = 1;
                    // should delete the continuation
                    ctn->closure->expr = NULL;
                    cek_deleteClosure(ctn->closure);
                    cek_deleteContinuation(ctn);
                    break;
                }
                // delete the continuation
                cek_deleteClosure(ctn->closure);
                cek_deleteContinuation(ctn);
                ctn = NULL;
            } else {    // OprKK or OpdKK
                // pop the current continuation
                ctn = state->continuation;
                state->continuation = ctn->next;
                // delete current closure and set from continuation
                state->closure->expr = NULL; /* prevent to be deleted */
                cek_deleteClosure(state->closure);
                state->closure = ctn->closure;
                if(!performReduction(state)) {
                    error = 1;
                    // should delete the continuation
                    cek_deleteContinuation(ctn);
                    break;
                }
                // delete the continuation
                cek_deleteClosure(ctn->closure);
                cek_deleteContinuation(ctn);
                ctn = NULL;
            }
        } else {
            if(state->closure->expr->kind==AppK) { 
                // push an ArgKK continuation
                ctn = cek_newContinuation(ArgKK);
            }else if(state->closure->expr->kind==PrimiK) { 
                // push OpdKK continuation
                ctn = cek_newContinuation(OpdKK);
            }
            ctn->closure = state->closure;
            ctn->next = state->continuation;
            state->continuation = ctn;
            state->closure = cek_newClosure(state->closure->expr->children[0],state->closure->env);
            ctn = NULL;
        }
    }

    TreeNode* result = NULL;
    if(!error) {
        result = state->closure->expr;
        state->closure->expr = NULL;
    }
    cek_cleanup(state);
    return result;
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

/* == Definitions of the local functions. */
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

/* 
 * Perform reductions on the expression of type application or primitive. 
 * Return 1 is successful, otherwise failed.
 */
static int performReduction(State* state) {
    if(state->closure->expr->kind==AppK) {
        if(state->closure->expr->children[0]->kind==ConstK) {
            fprintf(errOut, "Error: cannot apply a constant to any argument.\n");
            fprintf(errOut, "Expression:\t");
            printExpression(state->closure->expr,errOut);
            fprintf(errOut,"\n");
            deleteTree(state->closure->expr->children[0]);
            return 0;
        }else if(state->closure->expr->children[0]->kind==IdK) {
            // find function from builtin and standard library
            TreeNode* fun = resolveFunction(state->closure->expr->children[0]->name);
            if(fun==NULL) {
                fprintf(errOut, "Error: %s is not a predefined function.\n", state->closure->expr->children[0]->name);
                return 0;
            }
            // replace the expression with function definition
            deleteTree(state->closure->expr->children[0]);
            state->closure->expr->children[0] = fun;
        } else {
            /*
             * Set the name for current environment. It was left as NULL
             * when creating the environment, because the left child is not
             * guaranteed to be an abstraction so the parameter may not exist.
             */
            state->closure->env->name = stringCopy(state->closure->expr->children[0]->children[0]->name);
            /* 
             * Don't perform beta-reduction, just set body as control string.
             * The argument has already been put into the environment.
             */
            TreeNode *tmp = state->closure->expr->children[0]->children[1];
            /* 
             * Detach body from its parent, and attach to a new parent.
             */
            state->closure->expr->children[0]->children[1] = NULL;
            attachChild(state,tmp); /* attach to parent. */
            // delete the abstraction part, the other part will be deleted
            // when delete the closure
            deleteTree(state->closure->expr->children[0]);
            // set body as control string
            state->closure->expr = tmp;
        }
    }else if(state->closure->expr->kind==PrimiK) {
        if(!isValue(state->closure->expr->children[0])) {
            Continuation *ctn = cek_newContinuation(OpdKK);
            ctn->closure = cek_newClosure(state->closure->expr,state->closure->env);
            state->closure->expr = NULL; /* prevent from being deleted */
            ctn->next = state->continuation;
            state->continuation = ctn;
            state->closure = cek_newClosure(ctn->closure->expr->children[0],state->closure->env);
            ctn = NULL;
        } else if(!isValue(state->closure->expr->children[1])) {
            Continuation *ctn = cek_newContinuation(OprKK);
            ctn->closure = cek_newClosure(state->closure->expr,state->closure->env);
            state->closure->expr = NULL; /* prevent from being deleted */
            ctn->next = state->continuation;
            state->continuation = ctn;
            state->closure = cek_newClosure(ctn->closure->expr->children[1],state->closure->env);
            ctn = NULL;
        } else if(state->closure->expr->children[0]->kind==ConstK 
            && state->closure->expr->children[1]->kind==ConstK) {
            // only perform primitive operation if operands are constants
            TreeNode *tmp  = evalPrimitive(state->closure->expr);
            attachChild(state,tmp); /* attach to parent */
            /* 
             * Second operand is kept in an environment and it will be
             * deleted when delete the environment. So only need to delete
             * first operand here.
             */
            deleteTree(state->closure->expr->children[0]);
            deleteTree(state->closure->expr->children[1]);
            
            state->closure = cek_newClosure(tmp,NULL);
        } else {
            fprintf(errOut, "Error: %s can only be applied on constants.\n", state->closure->expr->name);
            // delete first operand, the other operand will be deleted
            // when delete the closure
            deleteTree(state->closure->expr->children[0]);
            deleteTree(state->closure->expr->children[1]);
            return 0;
        }
    }else {
        fprintf(errOut,"Error: Cannot evaluate unkown expression kind.\n");
        return 0;
    }
    return 1;
}

/*
 * Lookup closure by name in the environment or its parent.
 */
static Closure* lookupVariable(const char *name, Environment *env) {
    if(env==NULL) return NULL;
    if(strcmp(name,env->name)==0) {
        return env->closure;
    }
    return lookupVariable(name,env->parent);
}

/*
 * Substitutes free variables in the expression using mappings in the
 * environment.
 */
static TreeNode* resolveFreeVariables(TreeNode *expr, Environment *env) {
    // get the free variable set
    VarSet* set = FV(expr);
    VarSetList *list = vs_asList(set);
    deleteVarSet(set);

    TreeNode *result = expr;
    Closure *closure = NULL;
    VarSetList *l = list;
    while(l!=NULL) {
        closure = lookupVariable(l->name,env);
        if(closure==NULL) {
            fprintf(errOut,"Error: Variable %s is not defined.\n",l->name);
            result = NULL;
            break;
        } else {
            TreeNode *tmp = resolveFreeVariables(closure->expr,closure->env);
            if(tmp==NULL) {
                result = NULL;
                break;
            }
            TreeNode *var = newTreeNode(IdK);
            var->name = stringCopy(l->name);
            result = substitute(result,var,tmp);
            deleteTree(var);
        }
        l = l->next;
    }
    vs_deleteVarSetList(list);
    return result;
}

/* 
 * A new expression may be created after evaluating the control string,
 * so need to attach the new expression to the old one's parent. This
 * function determines whether to attach it as left or right child by 
 * checking the type of top continuation. If the top continuation is NULL, 
 * it does nothing.
 */
static void attachChild(const State *state, TreeNode *expr) {
    if(state->continuation!=NULL) {
        if(state->continuation->tag==ArgKK
            || state->continuation->tag==OpdKK) {
            state->continuation->closure->expr->children[0] = expr;
        }else if(state->continuation->tag==OprKK) {
            state->continuation->closure->expr->children[1] = expr;
        }
    }
}

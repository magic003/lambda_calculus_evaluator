/*****************************************************************/
/* File: cek_machine.c                                           */
/* Implementation of the CEK machine.                            */
/* Author: Minjie Zha                                            */
/*****************************************************************/

#include "globals.h"
#include "util.h"
#include "cek_machine.h"

/*
 * Some design notes:
 *  - Expression is splitted into pieces and  kept in continuations, so 
 *    don't need to worry about the parent/child relationships when deleting
 *    or adding an expressioin.
 *  - Deleting a closure won't delete the expression in it. It is the 
 *    programmer's responsibility to delete it.
 *  - Environment is shared among closures, while closures are not shared. 
 *    So after each step, the closure should be deleted explicitly, but
 *    an environment is only deleted when its refCount is 0.
 */

State* cek_newState(void) {
    State* st = (State*) malloc(sizeof(State));
    st->closure = NULL;
    st->continuation = NULL;
    return st;
}

void cek_deleteState(State* state) {
    if(state==NULL) return;
    free(state);
}

Environment* cek_newEnvironment(const char *name, Closure *closure, Environment *parent) {
    Environment *env = malloc(sizeof(Environment));
    env->name = NULL;
    if(name!=NULL) {
        env->name = strdup(name);
    }
    env->closure = closure;
    env->parent = parent;
    env->refCount = 0;
    if(parent!=NULL) {
        parent->refCount += 1;
    }
    return env;
}

void cek_deleteEnvironment(Environment *env) {
    if(env==NULL) return;

    free(env->name);
    // delete closure
    deleteTree(env->closure->expr);
    cek_deleteClosure(env->closure);
    Environment *parent = env->parent;
    free(env);
    if(parent!=NULL) {
        parent->refCount -= 1;
        if(parent->refCount==0) {
            cek_deleteEnvironment(parent);
        }
    }
}

Closure* cek_newClosure(TreeNode *expr, Environment *env) {
    Closure *closure = malloc(sizeof(Closure));
    closure->expr = expr;
    closure->env = env;
    if(env!=NULL) {
        env->refCount += 1;
    }
    return closure;
}

void cek_deleteClosure(Closure *closure) {
    if(closure->env!=NULL) {
        closure->env->refCount -= 1;
        if(closure->env->refCount==0) {
            cek_deleteEnvironment(closure->env);
        }
    }
    free(closure);
}

Continuation* cek_newContinuation(ContinuationKind tag) {
    Continuation* ctn = (Continuation*) malloc(sizeof(Continuation));
    ctn->tag = tag;
    ctn->closure = NULL;
    ctn->next = NULL;
    return ctn;
}

void cek_deleteContinuation(Continuation* continuation) {
    free(continuation);
}

void cek_cleanup(State* state) {
    // delete the current closure
    deleteTree(state->closure->expr);
    cek_deleteClosure(state->closure);
    // delete all continuations
    Continuation* ctn = NULL;
    while((ctn=state->continuation)!=NULL) {
        state->continuation = ctn->next;
        deleteTree(ctn->closure->expr);
        cek_deleteClosure(ctn->closure);
        cek_deleteContinuation(ctn);
    }
    // delete the state
    cek_deleteState(state);
}

int cek_canTerminate(State* state) {
    return isValue(state->closure->expr) && state->continuation==NULL
        && (state->closure->expr->kind==ConstK || state->closure->env==NULL);
}

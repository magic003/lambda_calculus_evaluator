/*****************************************************************/
/* File: ck_machine.c                                            */
/* Implementation of the CK machine.                             */
/* Author: Minjie Zha                                            */
/*****************************************************************/

#include "globals.h"
#include "util.h"
#include "ck_machine.h"

State* ck_newState(void) {
    State* st = (State*) malloc(sizeof(State));
    st->controlStr = NULL;
    st->continuation = NULL;
    return st;
}

void ck_deleteState(State* state) {
    free(state);
}

Continuation* ck_newContinuation(ContinuationKind tag) {
    Continuation* ctn = (Continuation*) malloc(sizeof(Continuation));
    ctn->tag = tag;
    ctn->expr = NULL;
    ctn->next = NULL;
    return ctn;
}

void ck_deleteContinuation(Continuation* continuation) {
    free(continuation);
}

void ck_cleanup(State* state) {
    // delete the expression tree first
    deleteTree(ck_getProgram(state));
    // delete all continuations
    Continuation* ctn = NULL;
    while((ctn=state->continuation)!=NULL) {
        state->continuation = ctn->next;
        ck_deleteContinuation(ctn);
    }
    // delete the state
    ck_deleteState(state);
}

TreeNode* ck_getProgram(State* state) {
    if(state->continuation==NULL) {
        return state->controlStr;
    }

    Continuation* ctn = state->continuation;
    // Find the last continuation, which should contain the root node.
    while(ctn->next!=NULL) {
        ctn = ctn->next;
    }
    return ctn->expr;
}

int ck_canTerminate(State* state) {
    return isValue(state->controlStr) && state->continuation==NULL;
}

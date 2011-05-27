/*****************************************************************/
/* File: cc_machine.c                                            */
/* Implementation of the CC machine.                             */
/* Author: Minjie Zha                                            */
/*****************************************************************/

#include "globals.h"
#include "util.h"
#include "cc_machine.h"

State* cc_newState(void) {
    State* st = (State*) malloc(sizeof(State));
    st->controlStr = NULL;
    st->context = NULL;
    return st;
}

void cc_deleteState(State* state) {
    free(state);
}

Context* cc_newContext(void) {
    Context* ctx = (Context*) malloc(sizeof(Context));
    ctx->expr = NULL;
    ctx->next = NULL;
    return ctx;
}

void cc_deleteContext(Context* context) {
    free(context);
}

void cc_cleanup(State* state) {
    // delete the expression tree first
    deleteTree(cc_getProgram(state));
    // delete all contexts
    Context* ctx = NULL;
    while((ctx=state->context)!=NULL) {
        state->context = ctx->next;
        cc_deleteContext(ctx);
    }
    // delete the state
    cc_deleteState(state);
}

TreeNode* cc_getProgram(State* state) {
    if(state->context==NULL) {
        return state->controlStr;
    }

    Context* ctx = state->context;
    // Find the last context, which should contain the root node.
    while(ctx->next!=NULL) {
        ctx = ctx->next;
    }
    return ctx->expr;
}

int cc_canTerminate(State* state) {
    return isValue(state->controlStr) && state->context==NULL;
}

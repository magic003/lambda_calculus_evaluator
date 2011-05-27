/*****************************************************************/
/* File: cc_machine.h                                            */
/* Interfaces of the CC machine.                                 */
/* Author: Minjie Zha                                            */
/*****************************************************************/

#ifndef _CC_MACHINE_H_
#define _CC_MACHINE_H_

/* Use a LIFO list to represent the context. */
typedef struct contextStruct {
    TreeNode * expr;
    struct contextStruct * next;
} Context;

/* Machine state is a pair of control string and the context. */
typedef struct stateStruct {
    TreeNode * controlStr;
    Context * context;
} State;

/* Allocates a new state. */
State* cc_newState(void);
/* Free a state. */
void cc_deleteState(State* state);

/* Allocates a new context. */
Context* cc_newContext(void);
/* Free a context. */
void cc_deleteContext(Context* context);

/* Free all memory used by this machine. */
void cc_cleanup(State* state);

/* Get the complete program that is currently evaluated in this machine. */
TreeNode* cc_getProgram(State* state);

/* Return if the machine reaches a terminate state. */
int cc_canTerminate(State* state);
#endif

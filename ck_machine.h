/*****************************************************************/
/* File: ck_machine.h                                            */
/* Interfaces of the CK machine.                                 */
/* Author: Minjie Zha                                            */
/*****************************************************************/

#ifndef _CK_MACHINE_H_
#define _CK_MACHINE_H_

/* Kind of each continuation. */
typedef enum {
    FunKK, ArgKK, OprKK, OpdKK
} ContinuationKind;

/* Use a LIFO list to represent the continuation. */
typedef struct continuationStruct {
    ContinuationKind tag;
    TreeNode * expr;
    struct continuationStruct * next;
} Continuation;

/* Machine state is a pair of control string and the context. */
typedef struct stateStruct {
    TreeNode * controlStr;
    Continuation * continuation;
} State;

/* Allocates a new state. */
State* ck_newState(void);
/* Free a state. */
void ck_deleteState(State* state);

/* Allocates a new continuation. */
Continuation* ck_newContinuation(ContinuationKind tag);
/* Free a continuation. */
void ck_deleteContinuation(Continuation* continuation);

/* Free all memory used by this machine. */
void ck_cleanup(State* state);

/* Get the complete program that is currently evaluated in this machine. */
TreeNode* ck_getProgram(State* state);

/* Return if the machine reaches a terminate state. */
int ck_canTerminate(State* state);
#endif

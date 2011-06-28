/*****************************************************************/
/* File: cek_machine.h                                           */
/* Interfaces of the CEK machine.                                */
/* Author: Minjie Zha                                            */
/*****************************************************************/

#ifndef _CEK_MACHINE_H_
#define _CEK_MACHINE_H_

/* Definitions of environment and closure. */
struct envStruct;
struct closureStruct;

struct envStruct {
    char *name;
    struct closureStruct *closure;
    struct envStruct *parent;
    int refCount;   /* Number of references. Just for memory management. */
};

struct closureStruct {
    TreeNode *expr;
    struct envStruct *env;
};

typedef struct envStruct Environment;
typedef struct closureStruct Closure;

/* Kind of each continuation. */
typedef enum {
    FunKK, ArgKK, OprKK, OpdKK
} ContinuationKind;

/* Use a LIFO list to represent the continuation. */
typedef struct continuationStruct {
    ContinuationKind tag;
    Closure * closure;
    struct continuationStruct * next;
} Continuation;

/* 
 * Machine state is a pair of closure and continuation. 
 */
typedef struct stateStruct {
    Closure *closure;
    Continuation * continuation;
} State;

/* Allocates a new state. */
State* cek_newState(void);
/* Free a state. */
void cek_deleteState(State* state);

/* Allocates a new environment with parent environment specified. */
Environment* cek_newEnvironment(const char *name, Closure *closure, Environment *parent);
/* Free an environment. */
void cek_deleteEnvironment(Environment *env);

/* Allocates a new closure. */
Closure* cek_newClosure(TreeNode* expr, Environment *env);
/* 
 * Free a closure, and the expression and environment in it are also freed. 
 */
void cek_deleteClosure(Closure *closure);

/* Allocates a new continuation. */
Continuation* cek_newContinuation(ContinuationKind tag);
/* Free a continuation. */
void cek_deleteContinuation(Continuation* continuation);

/* Free all memory used by this machine. */
void cek_cleanup(State* state);

/* Return if the machine reaches a terminate state. */
int cek_canTerminate(State* state);
#endif

/********************************************************************/
/* File: stdlib.h                                                   */
/* Definitions of the standard library.                             */
/* Author: Minjie Zha                                               */
/********************************************************************/

#ifndef _STDLIB_H_
#define _STDLIB_H_

typedef struct {
    char* name;
    char* expr;
} StandardFun;

/* Look for standard function by name. */
StandardFun* lookupStandardFun(const char* name);

/* Expand the standard function to a tree. */
TreeNode* expandStandardFun(StandardFun* fun);
#endif

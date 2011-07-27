/****************************************************************/
/* File: builtin.h                                              */
/* Definitions of the builtin functioins.                       */
/* Author: Minjie Zha                                           */
/****************************************************************/

#ifndef _BUILTIN_H_
#define _BUILTIN_H_

// function pointer to expand function
typedef TreeNode* (*ExpandFun)();

// structure holds the function properties
typedef struct BuiltinFunStructure {
    char* name;
    ExpandFun expandFun;
}  BuiltinFun;

/* Look for builtin function by name. */
BuiltinFun* lookupBuiltinFun(const char* name);

/* Get all builtin functions. */
BuiltinFun* builtinFuns(int *size);
#endif

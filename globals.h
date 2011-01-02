/***************************************************************/
/* File: globals.h                                             */
/* Definition of types and variables. This should be included  */
/* before any other included files.                            */
/* Author: Minjie Zha                                          */
/***************************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef YYPARSER
#include "y.tab.h"

#endif

/* expression types */
typedef enum { IdK, AbsK, AppK } ExprKind;

#define MAXCHILDREN 2
/* tree nodes */
typedef struct treeNode {
    ExprKind kind;
    char * name;    // only for IdK
    struct treeNode * children[MAXCHILDREN];
} TreeNode;

extern FILE* in;
extern FILE* out;
extern FILE* errOut;

extern char* yytext;
#endif

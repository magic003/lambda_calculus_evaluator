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
typedef enum { IdK, ConstK, AbsK, AppK, PrimiK } ExprKind;

#define MAXCHILDREN 2
/* tree nodes */
typedef struct treeNode {
    ExprKind kind;
    // NOTE: cannot use Union here because name is checked to free strings
    char * name;    // only for IdK
    int value;      // only for integers
    struct treeNode * children[MAXCHILDREN];
} TreeNode;

extern FILE* in;
extern FILE* out;
extern FILE* errOut;

// lex and yacc definitions
extern char* yytext;
extern int yyerror(char*);
extern int yylex(void);
extern void useStringBuffer(const char*);
extern void deleteStringBuffer();
extern int yyparse();
#endif

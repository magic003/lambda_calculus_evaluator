/*************************************************************/
/* File: util.h                                              */
/* Definition of utilities for lambda calculus evaluator.    */
/* Author: Minjie Zha                                        */
/*************************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

/* allocates a memory space for tree node. */
TreeNode * newTreeNode(ExprKind kind);

/* reallocates the memory space for a tree node. */
void deleteTreeNode(TreeNode *node);

/* copies a string. */
char * stringCopy(const char* s);

/* prints the syntax tree. */
void printTree(TreeNode * tree);

/* prints the expression in a human readable format. */
void printExpression(TreeNode *expr);
#endif

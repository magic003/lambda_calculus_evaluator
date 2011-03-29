/*************************************************************/
/* File: util.h                                              */
/* Definition of utilities for lambda calculus evaluator.    */
/* Author: Minjie Zha                                        */
/*************************************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

/* allocates a memory space for tree node. */
TreeNode * newTreeNode(ExprKind kind);

/* reallocates the memory space for a tree. */
void deleteTree(TreeNode *tree);

/* duplicates the tree by allocating a new memory space. */
TreeNode * duplicateTree(TreeNode *tree);

/* copies a string. */
char * stringCopy(const char* s);

/* prints the syntax tree. */
void printTree(TreeNode * tree, FILE* stream);

/* prints the expression in a human readable format. */
void printExpression(TreeNode *expr, FILE* stream);
#endif

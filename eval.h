/**************************************************************/
/* File: eval.h                                               */
/* Definition of the expression evaluation.                   */
/* Author: Minjie Zha                                         */
/**************************************************************/
#ifndef _EVAL_H_
#define _EVAL_H_
/* Evaluates the expression. */
TreeNode * evaluate(TreeNode *expr);

/* Perform alpha conversion on the expression. */
TreeNode * alphaConversion(TreeNode *expr);

/* Perform beta reduction on the expression. */
TreeNode * betaReduction(TreeNode *expr);

#endif

/*****************************************************************/
/* File: test.c                                                  */
/* Test driver of the lambda calculus evaluator.                 */
/* Author: Minjie Zha                                            */
/*****************************************************************/

#include "globals.h"
#include "util.h"
#include "eval.h"

#define PARSE_DUMP 0

TreeNode * tree = NULL;

FILE* out;
FILE* errOut;

int main(int argc, char* argv[]) {
    out = stdout;
    errOut = stderr;
    
    char* exprs[] = {"x","X","(lambda x x)","(lambda x y)",
                    "(lambda x (lambda y y))",
                    "(lambda x (lambda y x))",
                    "(lambda x (lambda y y) z)",
                    "x y",
                    "x (lambda y y)",
                    "(lambda x x) y",
                    "(lambda x x) (lambda y y)",
                    "(lambda x x) (lambda y y) z",
                    "(lambda x x x)",
                    "(lambda x x x) (lambda x x x)",
                    "(lambda x (lambda x x))",
                    "(lambda x (lambda y x))",
                    "(lambda x (lambda y y))",
                    "(lambda p (lambda q p q p))",
                    "(lambda p (lambda q p p q))",
                    "(lambda p (lambda a (lambda b p b a)))",
                    "(lambda p (lambda a (lambda b p a b)))",
                    "(lambda x (lambda y (lambda f f x y)))",
                    "(lambda p p (lambda x (lambda y x)))",
                    "(lambda p p (lambda x (lambda y y)))",
                    "(lambda x (lambda y (lambda z y)))",
                    "(lambda p (lambda x (lambda y (lambda a (lambda b b)))))"
                    };
    int i;
    for(i=0;i<26;i++) {
        fprintf(out,"Expression: %s\n",exprs[i]);
        yy_scan_string(exprs[i]);
        yyparse();
        if(PARSE_DUMP) {
            printTree(tree);
            fprintf(out,"\n");
        }
        tree = evaluate(tree);
        fprintf(out,"\t->  ");
        printExpression(tree);
        fprintf(out,"\n");
        deleteTreeNode(tree);
        tree=NULL;
    }

    fprintf(out,"\nTest alpha conversion:\n");
    const char * expr = "(lambda x (lambda x x))";
    fprintf(out,"Expression: %s\n",expr);
    yy_scan_string(expr);
    yyparse();
    tree = alphaConversion(tree);
    fprintf(out,"\t->  ");
    printExpression(tree);
    fprintf(out,"\n");
    deleteTreeNode(tree);
    tree=NULL;

    expr = "(lambda x (lambda y x))";
    fprintf(out,"Expression: %s\n",expr);
    yy_scan_string(expr);
    yyparse();
    tree = alphaConversion(tree);
    fprintf(out,"\t->  ");
    printExpression(tree);
    fprintf(out,"\n");
    deleteTreeNode(tree);
    tree=NULL;

    return 0;
}

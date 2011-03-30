/*****************************************************************/
/* File: test.c                                                  */
/* Test driver of the lambda calculus evaluator.                 */
/* Author: Minjie Zha                                            */
/*****************************************************************/

#include "globals.h"
#include "util.h"
#include "eval.h"

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
                    "(lambda p (lambda x (lambda y (lambda a (lambda b b)))))",
                    "(x)",
                    "((lambda x x))",
                    "((x))",
                    "((lambda x x) y)",
                    "(x x)",
                    "((x x))",
                    "(((lambda x x) u) v)",
                    "(u ((lambda x x) v))",
                    "((lambda x x) ((lambda y y) z))",
                    "(lambda f (lambda x f (f x)))",
                    "(lambda f (lambda x f (f (f x))))",
                    "(lambda n (lambda f (lambda x f (n f x))))",
                    "(lambda m (lambda n (lambda f (lambda x m f (n f x)))))",
                    "(lambda n (lambda f (lambda x n (lambda g (lambda h h (g f))) (lambda u x) (lambda u u))))",
                    "(lambda g (lambda x g (x x)) (lambda x g (x x)))",
                    "(lambda g (lambda x g (x x)) (lambda x g (x x))) g",
                    "A","ab","abc","aAa","AB","ABC","AaZ","var","_","__","_a",
                    "a_","A_a","_a_","(lambda name name)","say hello","_ _",
                    "-1","-50","0","100","(lambda x 10)","(lambda x x) 1",
                    "(lambda x x) -10","+ 1 1","(+ 2 2)","+ 1","+ -1 +1",
                    "(lambda x + x 1)","+","(lambda x (lambda y + x y))",
                    "- 1 1","* 1 1","/ 1 1","% 1 1","+ (+ 1 2) 3", "+ y",
                    "* (+ 1 2) 3","^ 2 4","< 1 2","> 1 2","= 2 2","<= 1 2",
                    ">= 1 2","!= 2 2",
                    "(lambda x (lambda y + (* x x) (* y y))) 3 4"
                    };
    int i;
    for(i=0;i<88;i++) {
        fprintf(out,"Expression: %s\n",exprs[i]);
        useStringBuffer(exprs[i]);
        yyparse();
        #ifdef DEBUG
            fprintf(errOut,"Parse tree =>\n");
            printTree(tree,errOut);
            fprintf(errOut,"\n");
        #endif
        tree = evaluate(tree);
        fprintf(out,"\t->  ");
        printExpression(tree,out);
        fprintf(out,"\n");
        deleteTree(tree);
        tree=NULL;
        deleteStringBuffer();
    }

    char *exprs1[] = {"(lambda x (lambda x x))",
            "(lambda x (lambda x_ x))",
            "(lambda az az)", "(lambda aZ aZ)","(lambda a_ a_)",
            "(lambda z (lambda z z))", "(lambda aZ (lambda aZ aZ))",
            "(lambda a_ (lambda a_ a_))", "(lambda aZ (lambda aZ_ aZ))",
            "(lambda a_ (lambda a__ a_))"
            };

    fprintf(out,"\nTest alpha conversion:\n");
    for(i=0;i<10;i++) {
        fprintf(out,"Expression: %s\n",exprs1[i]);
        useStringBuffer(exprs1[i]);
        yyparse();
        tree = alphaConversion(tree);
        fprintf(out,"\t->  ");
        printExpression(tree,out);
        fprintf(out,"\n");
        deleteTree(tree);
        tree=NULL;
        deleteStringBuffer();
    }

    return 0;
}

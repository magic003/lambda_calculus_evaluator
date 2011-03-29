/*****************************************************************/
/* File: main.c                                                  */
/* Implements the interactive interface of this lambda calculus  */
/* evaluator.                                                    */
/* Author: Minjie Zha                                            */
/*****************************************************************/

#include "globals.h"
#include "eval.h"
#include "util.h"

FILE* in;
FILE* out;
FILE* errOut;

TreeNode * tree = NULL;    // used in the parser

#define BUFF_SIZE 255

int main(int argc, char* argv[]) {
    
    in = stdin;
    out = stdout;
    errOut = stderr;

    char buff[BUFF_SIZE];

    fprintf(out,"Welcome to Lambda Calculus Evaluator.\n");
    fprintf(out,"Press Ctrl+C to quit.\n\n");
    while(1) {
        fprintf(out,"> ");
        fgets(buff,BUFF_SIZE-1,in);
        useStringBuffer(buff);
        yyparse();
        #ifdef DEBUG 
            fprintf(errOut,"Parse tree =>\n");
            printTree(tree,errOut);
            fprintf(errOut,"\n");
        #endif
        
        tree = evaluate(tree);
        fprintf(out,"-> ");
        printExpression(tree,out);
        deleteTree(tree);
        tree=NULL;
        deleteStringBuffer();
        buff[0] = EOF;
        fprintf(out,"\n\n");
    }
    return 0;
}

/***************************************************************/
/* File: eval.y                                                */
/* Yacc definition file for lambda calculus evaluator.         */
/* Author: Minjie Zha                                          */
/***************************************************************/

%{
#define YYPARSER

#include "globals.h"
#include "util.h"

#define YYSTYPE TreeNode *

extern YYSTYPE tree;
%}

%token  LAMBDA
%token  INT
%token  ID

%start expression_list

%%

expression_list : expression_list expression
                    {
                        $$ = newTreeNode(AppK);
                        $$->children[0] = $1;
                        $$->children[1] = $2;
                        tree = $$;
                    }
                |
                 expression
                    {
                        tree = $1;
                    }
                ;

expression      : ID    
                    {
                        $$ = newTreeNode(IdK);
                        $$->name = stringCopy(yytext);
                    }
                | INT
                    {
                        $$ = newTreeNode(ConstK);
                        $$->value = atoi(yytext);
                    }
                | '(' LAMBDA ID 
                    {
                        $$ = newTreeNode(IdK);
                        $$->name = stringCopy(yytext);
                    } 
                    expression_list ')'
                    {
                        $$ = newTreeNode(AbsK);
                        $$->children[0] = $4;
                        $$->children[1] = $5;
                    }
                | '(' expression_list ')'
                    {
                        $$ = $2;
                    }
                ;

%%

int yyerror(char *message) {
    fprintf(errOut,"%s\n",message);
    fprintf(errOut,"\ttoken: %s\n",yytext);
}

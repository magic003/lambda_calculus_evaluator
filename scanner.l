/************************************************************/
/* File: eval.l                                             */
/* Lex definition file for labmda calculus evaluator.       */
/* Author: Minjie Zha                                       */
/************************************************************/

%{
#include "globals.h"
%}

lambda      "lambda"
identifier  [a-z]
whitespace  [ \t]+
newline     [\r\n]+

%%

    /* reserved words */
{lambda}        {return LAMBDA;}

    /* identifier */
{identifier}    {return ID;}

{whitespace}    /* do nothing. */;
{newline}       ;

.               {return yytext[0];}

%%

int yywrap() {
    return 1;
}

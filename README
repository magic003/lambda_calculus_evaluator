### A Lambda Calculus Evaluator

#### Introduction
This is a simple lambda calculus evaluator, which supports the following concrete syntax:
    Expression := identifier 
                | (lambda identifier Expression)
                | Expression Expression

To keep it really pure, not constants are supported currently. It applies alpha-conversions and beta-reductions to reduce the expressions.

This evaluator is implemented using C. Lex and Yacc are used to generate the scanner and parser.

#### Prerequisites
Besides the base build tools, GCC and Make, an implementation for Lex and Yacc are necessary to compile and run this evaluator.

The Flex and Bison are used on my platform. You may change it to yours in the Makefile.

#### Build & Run
Change the build related variables in the Makefile to your specific environment.

Build the program using Make:
$ make

Run the evaluator using:
$ ./main

Quit the evaluator using Ctrl+C.

#### Contact
Zha Minjie <minjiezha@gmail.com>

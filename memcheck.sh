#!/bin/sh
#
# This is script that checks the memory leaks using Valgrind.
#

make debug
valgrind --tool=memcheck --leak-check=yes test

#!/bin/sh
#
# This is script that checks the memory leaks using Valgrind.
#

make debug

exprs=("x" "X" "(lambda x x)" "(lambda x y)" 
                "(lambda x (lambda y y))" 
                "(lambda x (lambda y x))" 
                "(lambda x (lambda y y) z)" 
                "x y" 
                "x (lambda y y)" 
                "(lambda x x) y" 
                "(lambda x x) (lambda y y)" 
                "(lambda x x) (lambda y y) z" 
                "(lambda x x) (lambda y y) 1" 
                "(lambda x x x)" 
                "(lambda x (lambda x x))" 
                "(lambda x (lambda y x))" 
                "(lambda x (lambda y y))" 
                "(lambda p (lambda q p q p))" 
                "(lambda p (lambda q p p q))" 
                "(lambda p (lambda a (lambda b p b a)))" 
                "(lambda p (lambda a (lambda b p a b)))" 
                "(lambda x (lambda y (lambda f f x y)))" 
                "(lambda p p (lambda x (lambda y x)))" 
                "(lambda p p (lambda x (lambda y y)))" 
                "(lambda x (lambda y (lambda z y)))" 
                "(lambda p (lambda x (lambda y (lambda a (lambda b b)))))" 
                "(x)" 
                "((lambda x x))" 
                "((x))" 
                "((lambda x x) y)" 
                "(x x)" 
                "((x x))" 
                "(((lambda x x) u) v)" 
                "(u ((lambda x x) v))" 
                "((lambda x x) ((lambda y y) z))" 
                "((lambda x x) ((lambda y y) 1))" 
                "(lambda f (lambda x f (f x)))" 
                "(lambda f (lambda x f (f (f x))))" 
                "(lambda n (lambda f (lambda x f (n f x))))" 
                "(lambda m (lambda n (lambda f (lambda x m f (n f x)))))" 
                "(lambda n (lambda f (lambda x n (lambda g (lambda h h (g f))) (lambda u x) (lambda u u))))" 
                "(lambda g (lambda x g (x x)) (lambda x g (x x)))" 
                "A" "ab" "abc" "aAa" "AB" "ABC" "AaZ" "var" "_" "__" "_a" 
                "a_" "A_a" "_a_" "(lambda name name)" "say hello" "_ _" 
                "-1" "-50" "0" "100" "(lambda x 10)" "(lambda x x) 1" 
                "(lambda x x) -10" "+ 1 1" "(+ 2 2)" "+ 1" "+ -1 +1" 
                "(lambda x + x 1)" "+" "(lambda x (lambda y + x y))" 
                "- 1 1" "* 1 1" "/ 1 1" "% 1 1" "+ (+ 1 2) 3"  "+ y" 
                "* (+ 1 2) 3" "^ 2 4" "< 1 2" "> 1 2" "= 2 2" "<= 1 2" 
                ">= 1 2" "!= 2 2" "1 1" "x 1" 
                "(lambda x (lambda y + (* x x) (* y y))) 3 4" 
                "(lambda x (lambda y y x)) 1 (lambda x x)" 
                "(lambda x (lambda y x (x y)) (x 1)) (lambda x x)" 
                "+ (lambda x x) 1" 
                "Y (lambda t (lambda n (= n 1) 1 (* n (t (- n 1))))) 3" 
                "Y (lambda t (lambda n (= n 1) 1 (+ n (t (- n 1))))) 4" 
                "Y (lambda t (lambda n (or (= n 1) (= n 2)) 1 (+ (t (- n 1)) (t (- n 2))))) 7" 
                "(and (not (= 2 3)) (= 2 2))" 
                "(or (= 1 1) ((lambda x x x) (lambda x x x)))")

ERROR_CODE=5
for expr in "${exprs[@]}"
do
    valgrind --tool=memcheck --leak-check=yes --error-exitcode=$ERROR_CODE ./test "$expr"
    if [ $? -eq $ERROR_CODE ]
    then
        echo "Memory leaks when evaluating: $expr"
        exit
    fi
done

echo "Success! No memory leak found."
